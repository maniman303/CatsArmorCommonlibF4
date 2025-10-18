Scriptname CATS:CatsPlayerRefBase extends ReferenceAlias Native Const Hidden

Function RegisterForHeadgearEquipEvent()
	RegisterForExternalEvent("HeadgearEquipEvent", "OnHeadgearEquipEvent")
EndFunction

Event OnHeadgearEquipEvent(int actorFormId) native
