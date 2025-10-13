Scriptname SCAT:ScatPlayerRef extends ReferenceAlias

Quest Property ScatQuest Auto Const

Perk Property HeadgearPerk Auto Const

Event OnPlayerLoadGame()
    (ScatQuest as SCAT:GivePlayerHolotape).TryToGiveItems()
EndEvent

; Event OnItemEquipped(Form akBaseObject, ObjectReference akReference)
	
; EndEvent

; Event OnItemUnequipped(Form akBaseObject, ObjectReference akReference)
	
; EndEvent

; Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
	
; EndEvent
