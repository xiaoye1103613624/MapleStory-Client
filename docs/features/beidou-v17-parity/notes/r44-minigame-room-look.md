# r44 — Minigame ROOM look 浅解

`TradeHandler` ROOM(5)+type=1：
1. slot byte
2. `skip_charlook`
3. owner string + gameType（1 Omok / 2 MatchCard）
4. 可选 wins/ties/losses/points
5. 开 `UIMinigame` + `UIMinigameTable` 并 `set_game_kind`

失败则吞剩余，不炸包。
