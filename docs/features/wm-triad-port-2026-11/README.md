# 三端差距对照 · W-M1…W-M5 落地

本端 × Angel(v83 IDB) × MinMS 差分：只补真缺深度，不整文件粘贴，不回退已闭环。

## 实现摘要

| 波次 | 内容 | 状态 |
| --- | --- | --- |
| W-M1 | `UIEnchantSkill` 充实；背包正槽 `ScrollEquipPacket`；ScrollResult 钩 | 代码合入 |
| W-M2 | 商城 `BtBuy`/`BtGift` → `UIYesNo` 确认；无效价目禁用 | 代码合入；对照 `商城.png` |
| W-M3 | `UICooldownList` + cooltime 秒→ms | 代码合入 |
| W-M4 | UserList 内嵌 PartyHP `GAUGE_FILL_W=63` | 代码合入 |
| W-M5 | AcceptDecline 用 Angel YESNO 的 BtYes/BtNo 槽 | 代码合入；对照 `NPC对话.png` |

## 证据

- 门禁：`notes/00-evidence-gate.md`
- 真机：`notes/01-live-verify.md`（构建通过；进图受幽灵进程干扰，待清 47560 后复验）

## 协议

改动触及 `SCROLL_EQUIP` 发送点 → 已重生 `docs/features/s9-protocol-align/protocol/opcode-matrix.json`。
