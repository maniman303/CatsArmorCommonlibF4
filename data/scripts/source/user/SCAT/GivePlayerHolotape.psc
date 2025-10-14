Scriptname SCAT:GivePlayerHolotape extends Quest

GlobalVariable Property IsModLoaded Auto Const

Worldspace Property PrewarWorldspace Auto Const

Form[] Property GiveItems Auto Const
{ Add any items you want to give the player when it is safe to do so. This will occur either immediately after the mod is installed, or if they start a new game, it will delay unil after they have a pipboy. }

ObjectMod[] Property ScatMods Auto Const
{ Array of hidden mods: Torso, Arm Left, Arm Right, Leg Left, Leg Right }

Keyword Property HeadgearHiddenKey Auto Const
Keyword Property HeadgearKey Auto Const
Keyword Property TorsoKey Auto Const
Keyword Property LeftArmKey Auto Const
Keyword Property RightArmKey Auto Const
Keyword Property LeftLegKey Auto Const
Keyword Property RightLegKey Auto Const

Keyword Property HairLongKey Auto Const
Keyword Property HairTopKey Auto Const
Keyword Property HairBeardKey Auto Const

Armor Property HairLong Auto Const
Armor Property HairTop Auto Const
Armor Property HairBeard Auto Const

Spell Property HeadgearAbility Auto Const

Perk Property HeadgearPerk Auto Const

Event OnQuestInit()
	TryToGiveItems()
EndEvent

Event OnPlayerTeleport()
	TryToGiveItems()
EndEvent

Function TryToGiveItems()
	Actor player = Game.GetPlayer()

	if (!player.HasPerk(HeadgearPerk))
		player.AddPerk(HeadgearPerk)
	endif

	Worldspace playerWorldspace = player.GetWorldspace()
	if (playerWorldspace == PrewarWorldspace || PlayerWorldspace == None)
		; SCAT:ScriptExtender.Trace("Registered player teleport.")
		RegisterForPlayerTeleport()
	else
		if (IsModLoaded.GetValueInt() == 1)
			return
		endif

		IsModLoaded.SetValueInt(1)

		SCAT:ScriptExtender.Trace("Distribute items.")
		UnregisterForPlayerTeleport()
		
		int i = 0
		while(i < GiveItems.Length)
			if (player.GetItemCount(GiveItems[i]) == 0)
				player.AddItem(GiveItems[i], abSilent = true)
			endif
			
			i += 1
		endWhile
	endif
EndFunction

Keyword Function GetArmorKeyword(int bippedIndex)
	if bippedIndex == 41
		return TorsoKey
	elseif bippedIndex == 42
		return LeftArmKey
	elseif bippedIndex == 43
		return RightArmKey
	elseif bippedIndex == 44
		return LeftLegKey
	elseif bippedIndex == 45
		return RightLegKey
	endif
	
	return None
EndFunction

Function HideArmorPiece(int modIndex)
	TransformArmorPiece(modIndex, false)
EndFunction

Function ShowArmorPiece(int modIndex)
	TransformArmorPiece(modIndex, true)
EndFunction

Function TransformHeadgear()
	Actor player = Game.GetPlayer()
	TransformArmorPiece(5, player.WornHasKeyword(HeadgearHiddenKey))
EndFunction

Function TransformArmorPiece(int modIndex, bool shouldRemove)
	int bipedIndex = 11 + modIndex
	Form baseObject = GetEquippedArmor(bipedIndex)
	
	if bipedIndex == 16 && baseObject == None
		baseObject = GetEquippedHeadgear()
	endIf

	if baseObject == None
		Debug.Notification("SCAT: Selected armor type " + bipedIndex + " is not equipped")
		SCAT:ScriptExtender.Trace("Selected armor type " + bipedIndex + " is not equipped.")
		return
	endIf
	
	Actor player = Game.GetPlayer()

	if !player.IsEquipped(baseObject)
		Debug.Notification("SCAT: " + baseObject.GetName() + " is not equipped")
		SCAT:ScriptExtender.Trace(baseObject.GetName() + " is not equipped.")
		return
	endif
	
	int itemCount = player.GetItemCount(baseObject)
	
	if itemCount > 1
		Debug.Notification("SCAT: You have too many " + baseObject.GetName())
		SCAT:ScriptExtender.Trace("You have too many " + baseObject.GetName() + ".")
		return
	endIf

	player.UnequipItem(baseObject, abSilent = true)
	
	ObjectMod pieceMod = ScatMods[modIndex]
	if shouldRemove
		player.RemoveModFromInventoryItem(baseObject, pieceMod)
	else
		player.AttachModToInventoryItem(baseObject, pieceMod)
	endIf
	
	player.EquipItem(baseObject, abSilent = true)
EndFunction

Form Function GetEquippedHeadgear()
	int[] slots = new int[5]
	slots[0] = 16
	slots[1] = 28
	slots[2] = 17
	slots[3] = 0
	slots[4] = 1
	
	Form item = None
	int index = 0
	while index < 5 && item == None
		item = GetEquippedArmor(slots[index])
	
		index = index + 1
	endWhile
	
	; SCAT:ScriptExtender.Trace("Found headgear at index: " + slots[index - 1] + ".")

	return item
EndFunction

Form Function GetEquippedArmor(int index)
	Actor player = Game.GetPlayer()
	Actor:WornItem item = player.GetWornItem(index)
	
	if item == None
		return None
	endIf
	
	if !item.Item is Armor
		return None
	endIf
	
	return item.Item
EndFunction

bool Function ValidateHeadgearHair(Actor akTarget, bool checkAbility = true)
    if (akTarget == NONE)
        return false
    endif

	bool res = false
    bool isHidden = akTarget.WornHasKeyword(HeadgearHiddenKey)

    ;SCAT:ScriptExtender.Trace("Running headgear effect on: " + akTarget.GetDisplayName())
	;SCAT:ScriptExtender.Trace("[" + akTarget.GetDisplayName() + "] IsHidden: " + isHidden)

    if (akTarget.WornHasKeyword(HairLongKey) && !isHidden)
		res = res || !akTarget.IsEquipped(HairLong)
        akTarget.EquipItem(HairLong, abSilent = checkAbility)
    else
		res = res || akTarget.IsEquipped(HairLong)
        akTarget.UnequipItem(HairLong, abSilent = checkAbility)
    endif

    if (akTarget.WornHasKeyword(HairTopKey) && !isHidden)
		res = res || !akTarget.IsEquipped(HairTop)
        akTarget.EquipItem(HairTop, abSilent = checkAbility)
    else
		res = res || akTarget.IsEquipped(HairTop)
        akTarget.UnequipItem(HairTop, abSilent = checkAbility)
    endif

    if (akTarget.WornHasKeyword(HairBeardKey) && !isHidden)
		res = res || !akTarget.IsEquipped(HairBeard)
        akTarget.EquipItem(HairBeard, abSilent = checkAbility)
    else
		res = res || akTarget.IsEquipped(HairBeard)
        akTarget.UnequipItem(HairBeard, abSilent = checkAbility)
    endif

    if (checkAbility && HeadgearAbility != NONE && akTarget.HasSpell(HeadgearAbility))
        ; SCAT:ScriptExtender.Trace("Removing one shot ability.")
        akTarget.RemoveSpell(HeadgearAbility)
    endif
	
	return res
EndFunction
