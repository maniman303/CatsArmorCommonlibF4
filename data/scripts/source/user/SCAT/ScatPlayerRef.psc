Scriptname SCAT:ScatPlayerRef extends ReferenceAlias

Quest Property ScatQuest Auto Const

Perk Property HeadgearPerk Auto Const

Keyword Property ActorTypeNPC Auto Const

Bool isMenuOpen

Event OnPlayerLoadGame()
    (ScatQuest as SCAT:GivePlayerHolotape).TryToGiveItems()
	
	SCAT:ScriptExtender.Trace("Player loaded the game")
	
	isMenuOpen = false
	
	RegisterForMenuOpenCloseEvent("ContainerMenu")
EndEvent

Event OnMenuOpenCloseEvent(string asMenuName, bool abOpening)
    if (asMenuName != "ContainerMenu")
        return
    endif
	
	isMenuOpen = abOpening
	
	if (!abOpening)
		return
	endif
	
	while isMenuOpen && Game.GetPlayer().HasPerk(HeadgearPerk)
		Utility.WaitMenuMode(0.1)
		UpdateHeadgearOfNearbyActors()
	endwhile
endEvent

Function UpdateHeadgearOfNearbyActors()
	Actor Player = Game.GetPlayer()
	ObjectReference[] kActors = Player.FindAllReferencesWithKeyword(ActorTypeNPC, 2048.0)

	int playerIndex = kActors.Find(Player as Actor)
	kActors.Remove(playerIndex)
	
	int actorsLength = kActors.length
	int i = 0
	while i < actorsLength
		ObjectReference object = kActors[i]
		if (object is Actor)
			Actor npc = object as Actor
			;SCAT:ScriptExtender.Trace("Found actor [" + npc.GetDisplayName() + "]")
			if ((ScatQuest as SCAT:GivePlayerHolotape).ValidateHeadgearHair(npc, false))
				npc.QueueUpdate(true, 0xC)
			endif
		endif
		
		i = i + 1
	endwhile
EndFunction

; Event OnItemEquipped(Form akBaseObject, ObjectReference akReference)
	
; EndEvent

; Event OnItemUnequipped(Form akBaseObject, ObjectReference akReference)
	
; EndEvent

; Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
	
; EndEvent
