# Evidence summary (2026-11-23)

## Lethal knockback
- IDA `CMob::ShowHitEffect@0xc671e0`: SetOneTimeAction only; no client shove.
- Root cause: Journey HIT `KBFORCE` sustained force + delayed KILL_MOB.
- Fix: drop HIT hforce; lethal/`hp_est` skip; clear velocity on die/kill.

## HP/MP alert
- MinMS: update flash animation every frame while pct low.
- Fix: continuous update/draw while below warn (hp/mp > 0); no one-shot enter gate.

## Teleport / Flash Jump
- MinMS: non-attack skills skip `can_attack`; SKILL key on press only; NW id `14101004`.
- Keep s9 `USE_SKILL(91)`; local displacement via `apply_use_movement`.
- Macro blacklist unchanged (skill key / quickslot path).

## Build / deploy
- `EXITCODE=0` Release|x64|v142 `/m:1` isolated OutDir.
- Live `MapleStory.exe` locked by running clients → wrote `MapleStory.oneshotfix.exe`.
