# char-stats v4 字段映射（实现对照）

依据：`layout_cn.json`、BeiDou-ijl15 `StatDetail`/`0x175`、`E:\MXD\扩展改动\伤害玩法`。

## 资源节点（UIWindow.img/Stat）

| 节点 | 尺寸 | 用途 |
| --- | --- | --- |
| `backgrnd` | 300×392 | 主栏 CN（默认） |
| `backgrnd_en` | 300×392 | 主栏 EN |
| `backgrnd2` | 352×392 | 详情 CN |
| `backgrnd2_en` | 352×392 | 详情 EN |

Locale：`Configuration::get_ui_language() == "en-US"` 时切 EN 底板。

## 坐标验收

- 表：[`IO/UITypes/UIStatsLayoutV4.h`](../../../IO/UITypes/UIStatsLayoutV4.h)
- 墨迹顶 = 值框顶 + 3；值文字 x = 值框左 + 4
- 详情原点 = 主窗 + `(306, 0)`（gap=6）

## 主栏

| UI 标签 | 数据源 | 缺省 |
| --- | --- | --- |
| 名称/职业/等级/名声/HP/MP/AP/EXP/四维 | CharStats / MapleStat | — |
| 公会 | 暂无 | `-` |
| 伤害 | mindamage ~ maxdamage | — |
| EP | 无 | `-` |
| 物理减伤 | `CombatPanelStats.damage_reduce`（0x175） | 未同步 `-` |
| 火/冰/电/毒/神圣抗性 | 无 | `-` |

## 详情

| UI 标签 | 数据源 |
| --- | --- |
| 物理攻击力 | `EquipStat::WATK` |
| 魔攻/物防/魔防/命中/回避/手技/移速/跳跃 | EquipStat 合计 |
| 伤害% / 额外伤害 / 最终% / 首领% / 无视防御率 | 0x175 `dam_r` / `normal_dam_r` / `final_damage_percent` / `boss_dam_r` / `ignore_pdr` |
| 暴击几率/伤害 | 0x175 `crit_rate` / `crit_dam` |
| 状态异常抗性 / Buff 持续 / 稳如泰山 / 掉落 / 金币 | 0x175 `asr_r` / `buff_time_r` / `stance_prop` / `item_drop_prop` / `meso_drop_prop` |
| 攻速/施法/格挡/HP·MP恢复/避免*/诅咒/荣誉/元素抗 | 无 → `-` |

## 协议

- Opcode：`SET_ITEM_FINAL_DAMAGE = 0x175`
- Handler：`SetItemFinalDamageHandler`（仅显示，不改伤害公式）
- 包体对齐 `PacketCreator.setItemFinalDamageBonus`（含可选尾 6B asrR/buff/stance）

## 备份

- Live 备份：`E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow.img.bak-charstats-v4-*`
- Staging 产物：`tools/_out_UIWindow_stat_v4.img`
