# r42 — Omok/MatchCard 落子 + 活动场包

## Minigame
- 出站：`MinigameOmokPlacePacket`(0x3E) / `MinigameMatchCardSelectPacket`(0x3F)
- 入站：TradeHandler 同步更新 `UIMinigameTable`
- 桌面点击粗映射 15×15 / 翻牌 slot

## 活动
- `COCONUT_SCORE`(286) / `SNOWBALL_STATE`(281) → `UIPartyRace` 分数/阶段
- `OX_QUIZ`(145) → `UISpeedQuiz` + FloatNotice
- `HORNTAIL_CAVE`(302) → PartyRace stage tip

## 构建
`EXITCODE=0` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
