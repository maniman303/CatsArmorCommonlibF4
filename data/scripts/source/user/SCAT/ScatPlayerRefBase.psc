Scriptname SCAT:ScatPlayerRefBase extends ReferenceAlias Native Const Hidden

Function RegisterForHeadgearEquipEvent()
	RegisterForExternalEvent("HeadgearEquipEvent", "OnHeadgearEquipEvent")
EndFunction

Event OnHeadgearEquipEvent() native
