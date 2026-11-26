# AGENTS.md

This file documents the Fallout 4 / CommonLibF4 debugging setup for this repository. Read it before touching hook addresses or trying to diagnose runtime behavior.

## 1. Environment and critical paths

| Purpose | Path |
| --- | --- |
| Repository / working directory | `D:\Programowanie\Fallout\CatsArmorCommonlibF4` |
| Fallout 4 root | `D:\SteamLibrary\steamapps\common\Fallout 4` |
| Game executable | `D:\SteamLibrary\steamapps\common\Fallout 4\Fallout4.exe` |
| F4SE loader | `D:\SteamLibrary\steamapps\common\Fallout 4\f4se_loader.exe` |
| Game Data directory | `D:\SteamLibrary\steamapps\common\Fallout 4\Data` |
| Installed plugin DLL | `D:\SteamLibrary\steamapps\common\Fallout 4\Data\F4SE\Plugins\CatsArmorPlugin.dll` |
| Runtime plugin log | `C:\Users\maniman303\Documents\My Games\Fallout4\F4SE\CatsArmorPlugin.log` |
| Ghidra root | `D:\Tools\ghidra_12.2_PUBLIC` |
| Ghidra headless launcher | `D:\Tools\ghidra_12.2_PUBLIC\support\analyzeHeadless.bat` |
| Ghidra project directory | `C:\Users\maniman303\AppData\Local\Temp\opencode\BethesdaGhidraScripts\ghidraprojects\BethesdaGhidraScripts` |
| Ghidra project name | `BethesdaGhidraScripts/f4/ae` |
| Ghidra scripts | `C:\Users\maniman303\AppData\Local\Temp\opencode\ghidra_scripts` |
| Ghidra output files | `C:\Users\maniman303\AppData\Local\Temp\opencode\gh_out` |
| Approved temp area | `C:\Users\maniman303\AppData\Local\Temp\opencode` |
| CommonLibF4 headers | `lib\commonlibf4\include` |
| CommonLibF4 shared REL headers | `lib\commonlibf4\lib\commonlib-shared\include` |
| CommonLibF4 shared sources | `lib\commonlibf4\lib\commonlib-shared\src` |
| Hook implementation | `src\HairHooks.h`, `src\HairHooks.cpp` |
| Existing vtable-hook style | `src\Hooks.h`, `src\Hooks.cpp` |
| Precompiled header | `src\PCH.h` |
| Installer | `data\install-cats.ps1` |

Shell commands run in PowerShell on Windows. Use dedicated tools for file edits/searches; use the shell for builds and Ghidra headless runs.

## 2. Runtime and address model

- Runtime: **Fallout 4 1.11.240**, **F4SE 0.7.9**, CommonLibF4.
- Ghidra image base: **`0x140000000`**.
- Ghidra addresses returned by scripts are already absolute (`0x14xxxxxxx`). Do **not** add the runtime image base again.
- `RVA = absolute Ghidra address - 0x140000000`.
- `REL::Offset` takes an **RVA**, not an absolute address:

```cpp
static inline REL::THook Hook{ REL::Offset{ 0x506733 }, 0, thunk };
```

- `REL::Offset::address()` uses `GetModuleHandleA(nullptr)`, i.e. the Fallout 4 EXE base.
- Runtime ASLR changes the image base per launch; the `0x7FF724000000` value seen in traces is only one observed base. Never hardcode it.

## 3. Hair / beard visibility architecture (the important part)

Hair is **not** hidden through `NiAVObject::SetAppCulled` and does not require string/editor-ID matching. Every head part mesh is a `BSSubIndexTriShape` whose subsegments are tagged with the owning biped slot:

```text
subsegment user index = 0x1E + biped slot
```

Relevant slots:

- `BIPED_OBJECT::kHairTop = 0`
- `BIPED_OBJECT::kHairLong = 1`
- `BIPED_OBJECT::kBeard = 18`
- Prefer `race->data.hairObject` / `race->data.beardObject` for classification when race-aware code is appropriate.

There are **three independent visibility paths**. All three must be covered to force hair/beard rendering:

### A. Main hair / beard head parts

`FUN_140506460` (RVA `0x506460`) computes the hide flag for hair, hair-long and beard from the actor's worn slot mask, then calls `FUN_14064E480` (RVA `0x64E480`) for each.

| RVA | Meaning |
| --- | --- |
| `0x506733` | `FUN_14064E480` call for hair |
| `0x5067A3` | `FUN_14064E480` call for hair-long |
| `0x506863` | `FUN_14064E480` call for beard |

`FUN_14064E480(node, slot, hide)` walks the node's geometry and applies the hide flag to subsegments tagged with `0x1E + slot`.

- `hide = true` -> `FUN_1416C34B0` -> **hide** (increment hidden count).
- `hide = false` -> `FUN_1416C3320` -> **show** (clear hidden count).

### B. Extra parts / hairlines

Hairlines are often `BGSHeadPart::HeadPartType::kMisc` head parts stored in the **`extraParts` array of the hair/beard head part**.

`FUN_14064E570` (RVA `0x64E570`) applies visibility to those extra parts. Its only external callers are inside `FUN_140506460`:

| RVA | Meaning |
| --- | --- |
| `0x506768` | hair extra parts |
| `0x5067D8` | hair-long extra parts |
| `0x506898` | beard extra parts |
| `0x64E6B8` | recursive call inside `FUN_14064E570` itself (do not hook this unless you actually need recursion-level interception) |

`FUN_14064E570` takes an ad-hoc stack context by pointer:

```cpp
struct HeadPartContext
{
    RE::NiAVObject* face;  // +0x00
    std::int32_t    slot;  // +0x08
    bool            hide;  // +0x0C
};
static_assert(sizeof(HeadPartContext) == 0x10);
```

This is **not** an upstream CommonLibF4 type. It is three locals laid out together on `FUN_140506460`'s stack. Model it with a local struct and `static_assert`s; do not scatter `reinterpret_cast` pointer arithmetic.

Important: the caller tests the return value (`CMP EAX, 1`) after `FUN_14064E570`. A call-site thunk **must return the original's value**.

### C. Biped pass re-hiding extra slots

After the head-part pass, the equipment update runs `FUN_140359BA0` (RVA `0x359BA0`), which calls `FUN_14035E3B0` (RVA `0x35E3B0`) per slot. That path can hide extra hair/beard slots that share the worn headgear's ARMA through `FUN_140360540` (RVA `0x360540`).

| RVA | Meaning |
| --- | --- |
| `0x35E37F` | `FUN_14035E3B0` call inside recursive walker `FUN_14035E2F0` |
| `0x35EEBA` | `FUN_14035E3B0` call after a clone rebuild in `FUN_14035E960` |
| `0x35E796` | `FUN_140360540` call inside `FUN_14035E3B0` (the re-hide path) |

At `0x35E796`, arguments are:

```text
RCX = face node
RDX = pointer to a pointer to int; **RDX is the slot
```

### Current active hooks in `HairHooks`

- `HeadPartVisibility`: `0x506733`, `0x5067A3`, `0x506863` (main hair / long / beard)
- `HeadPartExtra`: `0x506768`, `0x5067D8`, `0x506898` (hairlines / extra parts)
- `HeadPartHide`: `0x35E796` (biped re-hide)

`ShouldHide(actor, slot, engineHide)` in `HairHooks.cpp` is the single decision point. User code currently drives it from `Files`, `Setup`, and `ActorManager` keyword checks. **Preserve that logic.**

## 4. Useful engine functions and RVAs

| Address | RVA | Purpose |
| --- | --- | --- |
| `0x140359BA0` | `0x359BA0` | Biped visibility update for one actor |
| `0x14035E2F0` | `0x35E2F0` | Recursive part-clone walker |
| `0x14035E3B0` | `0x35E3B0` | Per-slot subsegment visibility decision |
| `0x14035E960` | `0x35E960` | Rebuild/apply visibility after clone change |
| `0x140360540` | `0x360540` | Hide a slot's subsegments across the face-node tree |
| `0x140506460` | `0x506460` | Hair / hair-long / beard head-part visibility decider |
| `0x14064E480` | `0x64E480` | Apply hide/show to main head part node |
| `0x14064E570` | `0x64E570` | Apply hide/show to extra parts (hairline) |
| `0x14051F530` | `0x51F530` | Build worn biped slot mask from inventory |
| `0x1416C3320` | `0x16C3320` | Show subsegment (clear hidden byte) |
| `0x1416C34B0` | `0x16C34B0` | Hide subsegment (increment hidden byte) |
| `0x1416C3860` | `0x16C3860` | Find subsegment by user index |

Other call sites / chains that were useful:

- `FUN_140359BA0` call in equip handler: `0x140659541` (RVA `0x659541`).
- `FUN_140506460` calls: `0x140658E27` (RVA `0x658E27`, equip path; skips the player's biped) and `0x1406F1571` (RVA `0x6F1571`, reference/head-update path; covers the player).
- `Actor::OnHeadInitialized`: `0x140CA7420`; calls `FUN_1406D14A0(animData, 1, 1)` and sets face-node dirty bit `0x100`.

## 5. Data layouts

### `RE::BipedAnim` (`RE/B/BipedAnim.h`)

- `root`: `+0x08`
- `object[44]`: `+0x10`
- `bufferedObjects[44]`: `+0xF30`
- `actorRef`: `+0x1E50`
- `sizeof`: `0x1E58`
- `GetRequester()` returns `ObjectRefHandle`; `.get()` resolves to a `NiPointer<TESObjectREFR>`.

### `RE::BIPOBJECT` (`RE/B/BIPOBJECT.h`), size `0x58`

- `parent` (`BGSObjectInstance`): `+0x00`
- `modExtra`: `+0x10`
- `armorAddon`: `+0x18`
- `part`: `+0x20`
- `skinTexture`: `+0x28`
- `partClone`: `+0x30`

Relative to the **BipedAnim base**, for slot `N`:

```text
armorAddon = biped + N * 0x58 + 0x28
part       = biped + N * 0x58 + 0x30
skinTexture= biped + N * 0x58 + 0x38
partClone  = biped + N * 0x58 + 0x40
```

### `RE::TESRace` (`RE/R/TESRace.h`)

- `RACE_DATA` starts at `+0x180`
- `headObject`: `+0x1B0`
- `hairObject`: `+0x1B4`
- `beardObject`: `+0x1B8`

### `RE::TESNPC` (`RE/T/TESNPC.h`)

- `TESRaceForm` base: `+0x1B0`
- `formRace` pointer: `+0x1B8`
- `GetHeadParts()` returns `std::span<BGSHeadPart*>`.

### `RE::BGSHeadPart` (`RE/B/BGSHeadPart.h`)

- `flags`: `+0x70`
- `type`: `+0x74` (`REX::TEnumSet<HeadPartType, std::int32_t>`)
- `extraParts`: `+0x78` (`BSTArray<BGSHeadPart*>`)
- `formEditorID`: `+0x170` (do **not** use editor IDs in this project)

Head part types relevant here:

```cpp
kMisc        = 0x0  // hairlines often use this, but are extraParts of hair/beard
kHair        = 0x3
kFacialHair  = 0x4
```

To verify an extra part belongs to an actor's hair/beard, iterate `npc->GetHeadParts()`, filter `kHair` / `kFacialHair`, then search `part->extraParts` for the target pointer.

### `RE::NiAVObject`

- `flags`: `+0x108`
- `userData`: `+0x110`
- `SetAppCulled` vtable slot: `0x2D`
- `GetObjectByName` vtable slot: `0x2E`

Actor resolution from a node uses the existing `GetNodeActor(RE::NiAVObject*)` in `HairHooks.cpp`: walk `parent` chain, inspect `userData`, test `Is<RE::Actor>()`. It has been verified in runtime logs. Do not replace it with editor-ID/name matching.

`TESObjectREFR::GetFaceNodeSkinned()` is the proper accessor for the face node (virtual slot `+0x408`).

## 6. Ghidra workflow

### Correct headless invocation

```powershell
& "D:\Tools\ghidra_12.2_PUBLIC\support\analyzeHeadless.bat" `
  "C:\Users\maniman303\AppData\Local\Temp\opencode\BethesdaGhidraScripts\ghidraprojects\BethesdaGhidraScripts" `
  "BethesdaGhidraScripts/f4/ae" `
  -process "Fallout4.exe" -noanalysis -readOnly `
  -scriptPath "C:\Users\maniman303\AppData\Local\Temp\opencode\ghidra_scripts" `
  -postScript <Script>.java
```

Filter stdout in PowerShell:

```powershell
... 2>&1 |
  Select-String -Pattern "<Script>.java>" |
  ForEach-Object { $_.Line -replace '^.*<Script>\.java> ', '' } |
  Out-String -Width 250
```

### Pitfalls that cost time

- Use `-noanalysis -readOnly`. Full analysis is unnecessary and slow.
- Ghidra often exits with code **1 even when the script succeeded**. Check the output file or stdout, not just the exit code.
- Many `.text` regions have no recognized containing function. `getFunctionContaining(address)` returns null. Use a backward scan for an enclosing function.
- `InstructionIterator` plus a backward scan is the reliable approach.
- Write script output with `PrintWriter` to `gh_out\<name>.txt`. Script `println` can be truncated.
- Function bodies often report `size=1`; do not trust the size.
- `decAt("1416c32b0")` may resolve to the enclosing `FUN_1416c3150`; mid-function names such as `func_0x0001416c32b0` are raw addresses, not independent functions.
- Treat Ghidra addresses as absolute (`0x14...`). Convert to RVA only once, when creating `REL::Offset`.

### Reusable scripts / outputs

| Resource | Use |
| --- | --- |
| `ghidra_scripts\QueryE.java` | Decompile helper with `enclosing()` backward scan; writes to a file |
| `ghidra_scripts\QueryF.java` | Related decompile/xref helper |
| `gh_out\filter.txt` | `FUN_14035E3B0` decompile |
| `gh_out\segapi.txt` | Subsegment show/hide/find primitives |
| `gh_out\driver.txt` | `FUN_140359BA0`, `FUN_14035DC20`, `FUN_14035E960` |
| `gh_out\bipedfilter.txt` | `FUN_140360540` and xrefs |
| `gh_out\guard.txt` | `FUN_140658C50`, `FUN_140659330`, `FUN_1406F12E0` |
| `gh_out\playerpath.txt` | Player/reference-update chain |
| `gh_out\mask.txt` | `FUN_14051F530`, `FUN_140D27280` |
| `gh_out\extrapart.txt` | `FUN_14064E570` decompile and callers |
| `gh_out\syms.txt`, `syms2.txt` | Symbol scans |

The Fallout 4 EXE is stripped. There are **no original symbol names** for these functions; `FUN_<address>` is all that exists. Use descriptive local names instead of searching forever for an upstream name.

## 7. Hook implementation rules

1. **Do not hook ordinary function prologues with `REL::THook`.** `THook::Detect()` only handles existing CALL/JMP instructions (`0xE8`, `0xE9`, `0xFF`). A normal prologue starts with something else and produces "could not determine hook type". Hook a direct **CALL site** of the desired function instead.
2. **Inline static hooks must be odr-used.** F4SE drains `REL::FHookStore` during `F4SE::Init()`. Force emission during static initialization with `std::addressof` anchors in the `.cpp`:

```cpp
[[maybe_unused]] const void* const g_hookAnchors[]{
    std::addressof(HeadPartVisibility::HookHair),
    std::addressof(HeadPartExtra::HookBeard),
    // ...
};
```

3. A call-site thunk replaces the original call. Invoke the original **exactly once** with the final desired arguments. Multiple hooks targeting the same original can share one trampoline for pass-through.
4. Preserve the original return value when the caller uses it. `FUN_14064E570` does; `FUN_14064E480` / `FUN_140360540` callers do not.
5. Windows x64 argument registers: `RCX`, `RDX`, `R8`, `R9` (third/fourth byte values in `R8B`/`R9B`).
6. Prefer modifying an input flag in place over reimplementing an entire engine function.
7. Prefer typed structs over `reinterpret_cast` pointer math.
8. Keep internal helpers (`ShouldHide`, `GetNodeActor`, etc.) in `HairHooks.cpp`; do not add unnecessary declarations to the header.
9. No unnamed namespaces. Keep hair code in `HairHooks.h/.cpp` and follow the surrounding `src\Hooks.h` style.
10. The user edits `HairHooks.cpp` directly. Read it before writing; use targeted edits and preserve their `ShouldHide`, logging, and keyword-check logic.

## 8. Forbidden / rejected approaches

Do not repeat these:

- **Do not inspect or snoop the repository's `.esp` / `.esl` files.** This includes `data\Cats Armor.esl`.
- **No string matching**, no editor IDs, no model basenames, no `NodeMatchesHeadPart`. Hair/beard identity is structural (biped slots, race data, head part types, `extraParts` pointers).
- Do not reintroduce probe/diagnostic code: `IsReadable`, `Mix`, `FloatBits`, `CensusNode`, `ProbeActorTree`, `FaceGenUpdate`, `<Windows.h>`.
- Do not retarget `NiAVObject::SetAppCulled`; it was proven not to be the hair hide path.
- Do not probe `UpdateDownwardPass`; it is too broad and generated ~55 MB of log.
- Do not hook hot subsegment primitives (`FUN_1416C3320` / `FUN_1416C34B0`) as a first solution; they lack actor context and run for every subsegment.
- Do not use post-hoc "re-show on next equip" logic. The override must be applied at the actual hide/show decision.
- Do not add dedupe caches / `SlotLogState` / `ShouldLog` unless explicitly requested; the user removed them as overengineering.
- Do not add wrapper helpers like `InvokeOriginal` when calling the trampoline directly is sufficient.
- Do not assume only slot 0 matters. Hair-long (1) and beard (18) have their own paths, and hairlines are extra parts.
- Do not hook only `FUN_14064E480`; extra parts (`FUN_14064E570`) and the biped re-hide (`FUN_140360540`) are separate paths.

## 9. Build, install and runtime verification

### Configure and build

```powershell
xmake f -m releasedbg -y
xmake
```

Build output:

```text
build\windows\x64\releasedbg\CatsArmorPlugin.dll
```

`xmake` installs to:

```text
D:\SteamLibrary\steamapps\common\Fallout 4\Data\F4SE\Plugins\CatsArmorPlugin.dll
```

### Static verification after a hook change

Check that the intended RVA constants were emitted in the DLL and that removed RVAs are gone. For example:

```powershell
$b = [System.IO.File]::ReadAllBytes($dll)
# Search for the 4-byte little-endian RVA constant.
```

The previous session verified these RVAs with a small PowerShell byte scan. Presence of the immediate is a quick way to catch an inline hook that was never emitted.

### Runtime testing

Do not launch/play the game on your own unless asked. The user runs it (normally via `f4se_loader.exe`) and updates the log. After a test, inspect:

```text
C:\Users\maniman303\Documents\My Games\Fallout4\F4SE\CatsArmorPlugin.log
```

Check:

- `HairSlot:` lines.
- `state=hidden` and `state=shown` transitions.
- Slots `0`, `1`, `18` (or race-derived slots).
- `[hide blocked]` vs `[hide allowed]`.
- Player form ID is `0x00000014`.
- Equipped `item` / `arma` IDs are populated only when the slot is occupied.
- Hairlines should now be handled through `HeadPartExtra`; if one still fails, determine whether it is an `extraPart` or the `kMeatcaps` (type 7 / scalp) path. Note that `kHeadRear` is type 9, not type 7.

The plugin may log `Plugin 'Cats Armor.esl' is missing.` in environments where the ESL is absent. This was previously out of scope. Do not inspect the ESL itself and do not get sidetracked by this unless the user asks.

## 10. Installer script

`data\install-cats.ps1` currently:

- Checks administrator rights first.
- Takes `-GamePath` = Fallout 4 root directory (the one containing `Fallout4.exe`).
- Validates `Fallout4.exe` exists.
- Derives `$DataPath = Join-Path $GamePath 'Data'` and uses that for destinations.
- Creates/backs up symlinks for meshes and scripts.

Usage:

```powershell
.\install-cats.ps1 -GamePath 'D:\SteamLibrary\steamapps\common\Fallout 4'
```

Do not change it back to accepting the Data directory.
