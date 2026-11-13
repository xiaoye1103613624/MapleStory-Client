# Wave1 Cygnus4 resource import (2026-11-12)

## Client imgs (GMS)
Deployed to `E:\MXD\BeiDou-Client-xy\Data\Skill\`:
- 1112.img 14611674 (from Skill_001.wz)
- 1212.img 33456673 (from Skill_002.wz)
- 1312.img 10918214 (from Skill_003.wz)
- 1412.img 12165278 (from Skill_004.wz)
- 1512.img 9725047 (from Skill_006.wz)

Pipeline: orange-wz LATEST load → create_img GMS → copy_paste info/skill → save_as → live replace.

## Skill.nx
Rebuilt via orange-wz WZ pack of all 76 Skill imgs + NoLifeWzToNx.
- before ~179MB → after 396550805
- verify: `1112.img/skill/11121000` has icon 32x32; `1111.img/skill/11111006` retained

## s9 Skill XML
Copied from MapleServer192 Skill.wz (non-empty).

## String
- s9 `String.wz/Skill.img.xml`: +208 Cygnus4 entries (incl. 11121000 希纳斯骑士)
- client String.img/nx: mutate path blocked by MCP session/type; s9 names usable; client names follow-up if skillbook blank

Backups: `tools/_staging/wave1_cygnus4/`
