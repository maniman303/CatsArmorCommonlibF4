Scriptname SCAT:ScatPlayerRef extends SCAT:ScatPlayerRefBase

Quest Property ScatQuest Auto Const

Perk Property HeadgearPerk Auto Const

;Bool isMenuOpen

Event OnPlayerLoadGame()
    (ScatQuest as SCAT:GivePlayerHolotape).TryToGiveItems()
	
	SCAT:ScriptExtender.Trace("Player loaded the game")
	
	;isMenuOpen = false
	
	RegisterForHeadgearEquipEvent()
	
	;RegisterForMenuOpenCloseEvent("ContainerMenu")
	
	;AddInventoryEventFilter((ScatQuest as SCAT:GivePlayerHolotape).HeadgearKey)
EndEvent

Event OnHeadgearEquipEvent()
	SCAT:ScriptExtender.Trace("Received HeadgearEquipEvent.")
	(ScatQuest as SCAT:GivePlayerHolotape).UpdateHeadgearOfNearbyActors()
EndEvent

;Event OnMenuOpenCloseEvent(string asMenuName, bool abOpening)
;    if (asMenuName != "ContainerMenu")
;      return
;   endif
;	
;	isMenuOpen = abOpening
;	
;	if (!abOpening)
;		return
;	endif
;	
;	while isMenuOpen && Game.GetPlayer().HasPerk(HeadgearPerk)
;		Utility.WaitMenuMode(0.05)
;		(ScatQuest as SCAT:GivePlayerHolotape).UpdateHeadgearOfNearbyActors()
;	endwhile
;endEvent

;Event OnItemAdded(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akSourceContainer)
;	if (isMenuOpen || akSourceContainer == None)
;		return
;	endif
;	
;	(ScatQuest as SCAT:GivePlayerHolotape).UpdateHeadgearOfNearbyActors(akBaseItem)
;endEvent

; Event OnItemEquipped(Form akBaseObject, ObjectReference akReference)
	
; EndEvent

; Event OnItemUnequipped(Form akBaseObject, ObjectReference akReference)
	
; EndEvent

; Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
	
; EndEvent
