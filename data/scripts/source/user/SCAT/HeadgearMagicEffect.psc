Scriptname Scat:HeadgearMagicEffect extends ActiveMagicEffect

Quest Property ScatQuest Auto Const

Event OnEffectStart(Actor akTarget, Actor akCaster)
    (ScatQuest as SCAT:GivePlayerHolotape).ValidateHeadgearHair(akTarget)
EndEvent
