;BEGIN FRAGMENT CODE - Do not edit anything between this and the end comment
Scriptname SCAT:Fragments:Terminals:scat_term_settings Extends Terminal Hidden Const

;BEGIN FRAGMENT Fragment_Terminal_01
Function Fragment_Terminal_01(ObjectReference akTerminalRef)
;BEGIN CODE
HeadgearSupport.SetValue(1.0)
;END CODE
EndFunction
;END FRAGMENT

;BEGIN FRAGMENT Fragment_Terminal_02
Function Fragment_Terminal_02(ObjectReference akTerminalRef)
;BEGIN CODE
HeadgearSupport.SetValue(0.0)
;END CODE
EndFunction
;END FRAGMENT

;END FRAGMENT CODE - Do not edit anything between this and the begin comment

GlobalVariable Property HeadgearSupport Auto Const

GlobalVariable Property BaldCapSupport Auto Const
