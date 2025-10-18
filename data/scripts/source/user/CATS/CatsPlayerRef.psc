Scriptname CATS:CatsPlayerRef extends CATS:CatsPlayerRefBase

Quest Property CatsQuest Auto Const

Perk Property HeadgearPerk Auto Const

Event OnAliasInit()
    InitPlayer()
EndEvent

Event OnPlayerLoadGame()
    InitPlayer()
EndEvent

Function InitPlayer()
	(CatsQuest as CATS:CatsQuest).TryToGiveItems()
	
	CATS:ScriptExtender.Trace("Player loaded the game.")
	
	RegisterForHeadgearEquipEvent()
EndFunction

Event OnHeadgearEquipEvent(int actorFormId)
	; if (aTarget == None)
	; 	return
	; endif
	
	; if (!(aTarget is Actor))
	; 	return
	; endif

	; CATS:ScriptExtender.Trace("Received HeadgearEquipEvent [" + actorFormId + "].")
	
	(CatsQuest as CATS:CatsQuest).UpdateHeadgearOfNearbyActors()
EndEvent
