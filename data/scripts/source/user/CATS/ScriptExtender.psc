Scriptname CATS:ScriptExtender Native

int[] Function GetArmorBipedSlots(Armor armorForm) native global

Function Trace(String message) native global

Function Notification(String message) native global

Function AddItemsFormList(FormList fList) global
	if (fList == None)
		return
	endIf
	
	Actor player = Game.GetPlayer()
	
	int size = fList.GetSize()
	int i = 0
	
	while i < size
		Form item = fList.GetAt(i)
		if (item != None)
			player.AddItem(item)
		endIf
		
		i = i + 1
	endWhile
EndFunction
