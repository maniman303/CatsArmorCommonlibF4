Scriptname Scat:HeadgearMagicEffect extends ActiveMagicEffect

Keyword Property HeadgearHiddenKey Auto Const
Keyword Property HeadgearKey Auto Const

Keyword Property HairLongKey Auto Const
Keyword Property HairTopKey Auto Const
Keyword Property HairBeardKey Auto Const

Armor Property HairLong Auto Const
Armor Property HairTop Auto Const
Armor Property HairBeard Auto Const

Spell Property Ability Auto Const

Event OnEffectStart(Actor akTarget, Actor akCaster)
    if (akTarget == NONE)
        return
    endif

    bool isHidden = akTarget.WornHasKeyword(HeadgearHiddenKey)

    ; SCAT:ScriptExtender.Trace("Running headgear effect on: " + akTarget.GetDisplayName())

    if (akTarget.WornHasKeyword(HairLongKey) && !isHidden)
        akTarget.EquipItem(HairLong, abSilent = true)
    else
        akTarget.UnequipItem(HairLong, abSilent = true)
    endif

    if (akTarget.WornHasKeyword(HairTopKey) && !isHidden)
        akTarget.EquipItem(HairTop, abSilent = true)
    else
        akTarget.UnequipItem(HairTop, abSilent = true)
    endif

    if (akTarget.WornHasKeyword(HairBeardKey) && !isHidden)
        akTarget.EquipItem(HairBeard, abSilent = true)
    else
        akTarget.UnequipItem(HairBeard, abSilent = true)
    endif

    if (Ability != NONE && akTarget.HasSpell(Ability))
        ; SCAT:ScriptExtender.Trace("Removing one shot ability.")
        akTarget.RemoveSpell(Ability)
    endif
EndEvent
