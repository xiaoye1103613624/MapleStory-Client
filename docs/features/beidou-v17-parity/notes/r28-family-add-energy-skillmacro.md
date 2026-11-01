# r28 — Family ADD/Invite + EnergyBar + SkillMacro store

## Family
- `ADD_FAMILY`(147) via SoftKey on BtJunior
- `ACCEPT_FAMILY`(150) via YesNo on `FAMILY_JOIN_REQUEST`(97)
- Handle `FAMILY_RESULT`(96) / `FAMILY_JOIN_REQUEST_RESULT`(98) tips

## W1 Skill / Energy
- `SkillMacros` store filled by `SKILL_MACROS`(124); SkillBook macro panel shows names
- `UIEnergyBar` HUD from `ENERGY_CHARGE` buff + `UIWindow.img/EnergyBar`

## Deferred
- Family precept edit packet
- SkillMacro drag/save outbound (`CHANGE_SKILL_MACRO`)
- Energy gage fill polish vs REA coords
