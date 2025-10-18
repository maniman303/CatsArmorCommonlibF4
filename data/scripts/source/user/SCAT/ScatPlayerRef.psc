Scriptname SCAT:ScatPlayerRef extends SCAT:ScatPlayerRefBase

Quest Property ScatQuest Auto Const

Perk Property HeadgearPerk Auto Const

Event OnAliasInit()
    InitPlayer()
EndEvent

Event OnPlayerLoadGame()
    InitPlayer()
EndEvent

Function InitPlayer()
	(ScatQuest as SCAT:GivePlayerHolotape).TryToGiveItems()
	
	SCAT:ScriptExtender.Trace("Player loaded the game.")
	
	RegisterForHeadgearEquipEvent()
EndFunction

Event OnHeadgearEquipEvent(int actorFormId)
	; if (aTarget == None)
	; 	return
	; endif
	
	; if (!(aTarget is Actor))
	; 	return
	; endif

	; SCAT:ScriptExtender.Trace("Received HeadgearEquipEvent [" + actorFormId + "].")
	
	(ScatQuest as SCAT:GivePlayerHolotape).UpdateHeadgearOfNearbyActors(actorFormId)
EndEvent
