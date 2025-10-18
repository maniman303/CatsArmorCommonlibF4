Scriptname CATS:HeadgearMagicEffect extends ActiveMagicEffect

Quest Property CatsQuest Auto Const

Event OnEffectStart(Actor akTarget, Actor akCaster)
    (CatsQuest as CATS:CatsQuest).ValidateHeadgearHair(akTarget)
EndEvent
