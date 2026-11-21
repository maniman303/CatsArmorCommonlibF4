Scriptname CATS:CatsQuest extends Quest

GlobalVariable Property IsModLoaded Auto Const

Worldspace Property PrewarWorldspace Auto Const

Form[] Property GiveItems Auto Const
{ Add any items you want to give the player when it is safe to do so. This will occur either immediately after the mod is installed, or if they start a new game, it will delay unil after they have a pipboy. }

ObjectMod Property HeadgearMod Auto Const
ObjectMod Property TorsoMod Auto Const
ObjectMod Property LeftArmMod Auto Const
ObjectMod Property RightArmMod Auto Const
ObjectMod Property LeftLegMod Auto Const
ObjectMod Property RightLegMod Auto Const
ObjectMod Property EyeMod Auto Const

Keyword Property HeadgearHiddenKey Auto Const
Keyword Property HeadgearKey Auto Const
Keyword Property TorsoKey Auto Const
Keyword Property LeftArmKey Auto Const
Keyword Property RightArmKey Auto Const
Keyword Property LeftLegKey Auto Const
Keyword Property RightLegKey Auto Const
Keyword Property EyeKey Auto Const

Message Property TooManyMsg Auto Const

Event OnQuestInit()
	TryToGiveItems()
EndEvent

Event OnPlayerTeleport()
	TryToGiveItems()
EndEvent

Function TryToGiveItems()
	Actor player = Game.GetPlayer()

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

ObjectMod Function GetArmorMod(int bipedIndex)
	if bipedIndex == 11
		return TorsoMod
	elseif bipedIndex == 12
		return LeftArmMod
	elseif bipedIndex == 13
		return RightArmMod
	elseif bipedIndex == 14
		return LeftLegMod
	elseif bipedIndex == 15
		return RightLegMod
	elseif bipedIndex == 16
		return HeadgearMod
	elseif bipedIndex == 17
		return EyeMod
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

Form Function GetEquippedHeadgear()
	int[] slots = new int[5]
	slots[0] = 16
	slots[1] = 28
	slots[2] = 17
	slots[3] = 0
	slots[4] = 1
	
	Form item = None
	int index = 0
	while index < 5 && (item == None || !item.HasKeyword(HeadgearKey))
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

Function TransformArmorPiece(int modIndex, bool shouldRemove)
	int bipedIndex = 11 + modIndex
	Form baseObject = GetEquippedArmor(bipedIndex)
	
	if bipedIndex == 16 && baseObject == None
		baseObject = GetEquippedHeadgear()
	endIf

	if baseObject == None
		Debug.Notification("CATS: Armor type " + bipedIndex + " is not equipped")
		CATS:ScriptExtender.Trace("Armor type " + bipedIndex + " is not equipped.")
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
		String text = CATS:ScriptExtender.GetMessageTitle(TooManyMsg)
		text = CATS:ScriptExtender.StringReplace(text, "{}", baseObject.GetName())
		CATS:ScriptExtender.Notification(text)
		CATS:ScriptExtender.Trace(text)
		return
	endIf

	player.UnequipItem(baseObject, abSilent = true)
	
	ObjectMod pieceMod = GetArmorMod(bipedIndex)
	if pieceMod == None
		CATS:ScriptExtender.Trace("Cannot find proper item mod.")
		return
	endIf
	
	if shouldRemove
		player.RemoveModFromInventoryItem(baseObject, pieceMod)
	else
		player.AttachModToInventoryItem(baseObject, pieceMod)
	endIf
	
	player.EquipItem(baseObject, abSilent = true)
EndFunction
