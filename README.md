# Cats Armor plugin

This is a Cats Armor plugin, responsible for dynamic patching of armors, and perk and spell distribution onto NPCs.

### OG version
Repo for Fallout 4 OG version can be found [here](https://github.com/maniman303/CatsArmorCommonlibF4-OG).

### Requirements
* [XMake](https://xmake.io) [2.8.2+]
* C++23 Compiler (MSVC or Clang-CL)

### Getting Started
```bat
git clone --recurse-submodules https://github.com/maniman303/CatsArmorCommonlibF4.git
cd CatsArmorCommonlibF4
```

## VS Code Setup

Run
```bat
xmake project -k compile_commands -y
```

Then move `compile_commands.json` to `.vscode` folder.

### Build
To build the project, run the following command:
```bat
xmake build -y
```

> ***Note:*** *This will generate a `build/windows/` directory in the **project's root directory** with the build output.*

### Build Output (Optional)
If you want to redirect the build output, set one of the following environment variables:

- Path to a Fallout 4 install folder: `XSE_FO4_GAME_PATH`

- Path to a Mod Manager mods folder: `XSE_FO4_MODS_PATH`

### Project Generation (Optional)
If you want to generate a Visual Studio project, run the following command:
```bat
xmake project -k vsxmake
```

> ***Note:*** *This will generate a `vsxmakeXXXX/` directory in the **project's root directory** using the latest version of Visual Studio installed on the system.*

### Upgrading Packages (Optional)
If you want to upgrade the project's dependencies, run the following commands:
```bat
xmake repo --update
xmake require --upgrade
```
