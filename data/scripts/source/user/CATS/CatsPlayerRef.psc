Scriptname CATS:CatsPlayerRef extends CATS:CatsPlayerRefBase

Quest Property CatsQuest Auto Const

Event OnAliasInit()
    InitPlayer()
EndEvent

Event OnPlayerLoadGame()
    InitPlayer()
EndEvent

Function InitPlayer()
	(CatsQuest as CATS:CatsQuest).TryToGiveItems()
	
	CATS:ScriptExtender.Trace("Player loaded the game.")
EndFunction
