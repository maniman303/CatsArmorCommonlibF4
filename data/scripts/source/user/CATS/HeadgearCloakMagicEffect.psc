Scriptname CATS:HeadgearCloakMagicEffect extends ActiveMagicEffect

Perk Property HeadgearPerk Auto Const

Event OnEffectStart(Actor akTarget, Actor akCaster)
    CATS:ScriptExtender.Trace("Found target: " + akTarget.GetDisplayName())

    if (!akTarget.HasPerk(HeadgearPerk))
        akTarget.AddPerk(HeadgearPerk)
    endif
EndEvent
