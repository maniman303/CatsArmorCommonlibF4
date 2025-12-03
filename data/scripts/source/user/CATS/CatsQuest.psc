Scriptname CATS:CatsQuest extends Quest

GlobalVariable Property IsModLoaded Auto Const

Worldspace Property PrewarWorldspace Auto Const

Form[] Property GiveItems Auto Const
{ Add any items you want to give the player when it is safe to do so. This will occur either immediately after the mod is installed, or if they start a new game, it will delay unil after they have a pipboy. }

ObjectMod[] Property CatsMods Auto Const
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

Keyword Property ActorProcessing Auto Const

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
		; CATS:ScriptExtender.Trace("Registered player teleport.")
		RegisterForPlayerTeleport()
	else
		if (IsModLoaded.GetValueInt() == 1)
			return
		endif

		IsModLoaded.SetValueInt(1)

		CATS:ScriptExtender.Trace("Distribute items.")
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
		Debug.Notification("CATS: Selected armor type " + bipedIndex + " is not equipped")
		CATS:ScriptExtender.Trace("Selected armor type " + bipedIndex + " is not equipped.")
		return
	endIf
	
	Actor player = Game.GetPlayer()

	if !player.IsEquipped(baseObject)
		CATS:ScriptExtender.Notification("CATS: " + baseObject.GetName() + " is not equipped")
		CATS:ScriptExtender.Trace(baseObject.GetName() + " is not equipped.")
		return
	endif
	
	int itemCount = player.GetItemCount(baseObject)
	
	if itemCount > 1
		CATS:ScriptExtender.Notification("CATS: You have too many " + baseObject.GetName())
		CATS:ScriptExtender.Trace("You have too many " + baseObject.GetName() + ".")
		return
	endIf

	player.UnequipItem(baseObject, abSilent = true)
	
	ObjectMod pieceMod = CatsMods[modIndex]
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
	
	; CATS:ScriptExtender.Trace("Found headgear at index: " + slots[index - 1] + ".")

	return item
EndFunction

Form Function GetEquippedArmor(int index)
	Actor player = Game.GetPlayer()
	Actor:WornItem item = player.GetWornItem(index)
	
	if item == None || item.Item == None
		;CATS:ScriptExtender.Trace("Item is none.")
		return None
	endIf
	
	if !(item.Item is Armor)
		;CATS:ScriptExtender.Trace("Item is not armor.")
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

    ;CATS:ScriptExtender.Trace("Running headgear effect on: " + akTarget.GetDisplayName())
	;CATS:ScriptExtender.Trace("[" + akTarget.GetDisplayName() + "] IsHidden: " + isHidden)

    if (akTarget.WornHasKeyword(HairLongKey) && !isHidden)
		res = res || !akTarget.IsEquipped(HairLong)
        akTarget.EquipItem(HairLong, abPreventRemoval = true, abSilent = true)
    else
		res = res || akTarget.IsEquipped(HairLong)
        akTarget.UnequipItem(HairLong, abSilent = true)
    endif

    if (akTarget.WornHasKeyword(HairTopKey) && !isHidden)
		res = res || !akTarget.IsEquipped(HairTop)
        akTarget.EquipItem(HairTop, abPreventRemoval = true, abSilent = true)
    else
		res = res || akTarget.IsEquipped(HairTop)
        akTarget.UnequipItem(HairTop, abSilent = true)
    endif

    if (akTarget.WornHasKeyword(HairBeardKey) && !isHidden)
		res = res || !akTarget.IsEquipped(HairBeard)
        akTarget.EquipItem(HairBeard, abPreventRemoval = true, abSilent = true)
    else
		res = res || akTarget.IsEquipped(HairBeard)
        akTarget.UnequipItem(HairBeard, abSilent = true)
    endif

    if (checkAbility && HeadgearAbility != NONE && akTarget.HasSpell(HeadgearAbility))
        ; CATS:ScriptExtender.Trace("Removing one shot ability.")
        akTarget.RemoveSpell(HeadgearAbility)
    endif
	
	return res
EndFunction

Actor Function GetValidActor(ObjectReference aTarget)
	if (aTarget == None || !(aTarget is Actor))
		return None
	endIf
	
	Actor npc = aTarget as Actor
	if (npc.IsDeleted() || npc.IsDisabled())
		return None
	endIf
	
	return npc
EndFunction

Function UpdateHeadgearOfNearbyActors()
	Actor Player = Game.GetPlayer()
	ObjectReference[] kActors = Player.FindAllReferencesWithKeyword(ActorProcessing, 450)
	
	int actorsLength = kActors.length
	
	;CATS:ScriptExtender.Trace("Found " + actorsLength + " actors")
	
	if (Player.HasKeyword(ActorProcessing))
		Player.RemoveKeyword(ActorProcessing)
		ValidateHeadgearHair(Player, false)
		Player.QueueUpdate(true, 0xC)
		
		CATS:ScriptExtender.Trace("Player updated.")
	endif
	
	int i = 0
	while i < actorsLength
		Actor npc = GetValidActor(kActors[i])
		if (npc != None)
			npc.RemoveKeyword(ActorProcessing)
			
			if (ValidateHeadgearHair(npc, false))
				npc.QueueUpdate(true, 0xC)
				CATS:ScriptExtender.Trace("Update appearance of [" + npc.GetDisplayName() + "].")
			endif
		endif
		
		i = i + 1
	endwhile
EndFunction
