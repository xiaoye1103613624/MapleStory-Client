# r39 — Minigame ↔ PLAYER_INTERACTION

- `TradeHandler` ROOM(5) + type=1 → 开 `UIMinigame`
- 出站 READY(0x3A) / START(0x3D) / EXIT(0x0A)
- 入站 READY/UNREADY/START 更新 tip
