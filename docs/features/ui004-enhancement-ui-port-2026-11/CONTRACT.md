# UI_004 强化族移植 — 客户端/服务端协议契约（实现基线）

> 192 二进制加壳（ida-192.md），原生 opcode/概率不可静态取证。本契约是**自造协议**（用户 D3=A 接受：公开 Nexon 概率做可配置表），两端必须严格对齐。任何字段顺序改动须双端同步。

## opcode

| 方向 | 名称 | 十进制 | hex | 备注 |
|---|---|---|---|---|
| C→S | `ENHANCE_ACTION` | 14109 | 0x371D | 通用强化请求 |
| S→C | `ENHANCE_RESULT` | 14110 | 0x371E | 通用强化结果快照 |

延续 0x3719(魔女森林)/0x371B(突破石) 的 s9 扩展通道号段，未占用。

## C→S 请求包 `EnhanceActionPacket`

```
byte  action       // 族 id，见下表
short cashSlot     // CASH 页签消耗道具槽（0=无）
int   cashId       // 消耗道具 itemid（魔方/锤/印章/材料）
short equipSlot    // EQUIP 页签目标装备槽（0=无）
short matSlot      // 第二材料槽（合成铁砧用，0=无）
byte  mode         // 子模式：0=执行, 1=预览(before/after), 2=应用预览
```

## 族 id 表（action）

| id | 常量名 | UI 节点 | 道具号段 | 交互 |
|---|---|---|---|---|
| 1 | CUBE_RED | Cube_Red | 5062xxx | 选装备→重洗→BtOnemore |
| 2 | CUBE_BLACK | Cube_Black | 5062010 | 重洗+Before/After选择 |
| 3 | CUBE_PLATINUM | MiracleCube_Platinum | 5062006 | 白金魔方，最高潜能 |
| 4 | CUBE_ADDITIONAL | MiracleCube_Additional | 5062500 | 附加潜能 |
| 5 | CUBE_WHITE | AddtionalCube_White | 5062503 | 白附加+Before/After |
| 6 | CUBE_MIRACLE | MiracleCube / Amazing/Master/8th/9th/Memorial/NewYear | 5062xxx | 通用神奇魔方 |
| 7 | CUBE_HYPER | HyperMiracleCube | 5062xxx | 超级魔方 |
| 8 | CUBE_HEXA | NewHexaCube | 5062xxx | 六角魔方+BtConfirm |
| 9 | CUBE_10TH | CN_10thHugeCube | 5062xxx | 10周年巨大魔方 |
| 10 | HAMMER_GOLD | GoldHammer | 5570000 | 选装备→BtStart→Gauge→Effect |
| 11 | HAMMER_WHITE | WhiteGoldHammer | 5570001 | 白金锤 |
| 12 | POTENTIAL_STAMP | PotentialStamp | 5530111/5530124 | 加1条潜能 |
| 13 | ADDITIONAL_STAMP | AdditionalPotentialStamp | 5530268/9 | 加1条附加潜能 |
| 14 | STONE_PURIFY | stonePurify | 5060023 | 石净化+BtPurify |
| 15 | JEWEL_CRAFT | jewelCraft | 50624xx | 宝石炼金 |
| 16 | SYNERGY | SynergyMachine/2/3 | — | 协同机 |
| 17 | SCROLL_SYNERGY | ScrollSynergyMachine | — | 卷轴协同 |
| 18 | ITEM_POT | itemPot | — | 物品罐 |
| 19 | MASTERPIECE | MasterPiece | 5062400 | 大师作品/铁砧 |
| 20 | SOUL_COLLECTOR | SoulCollector | — | 灵魂收集 |

> 卷轴强化（EnchantSkill/204）走**已有** `SCROLL_EQUIP` 链路（`UIEnchantSkill`），不在本契约内；维加/保护（561/559）已有链路，不重复。

## S→C 回包 `EnhanceResultPacket`

```
byte  action      // 对应请求 action
byte  result      // 0=失败, 1=成功, 2=破坏
short equipSlot   // 目标装备槽
// 装备全量增强字段（与 PacketCreator.addItemInfo / ItemParser.add_equip 严格对齐）：
short potential1
short potential2
short potential3
short bonusPotential1
short bonusPotential2
short spark
short soul
byte  starForce
short vicious
short slots
```

客户端收到后：更新本地 Equip 缓存 + 刷新 `UIEquipInventory` + 刷新 `EquipTooltip`（如开着）。

## 服务端概率配置（D3=A，公开 Nexon 公示，非 192 实测）

- 魔方 tier-up：red R→E 6%/E→U 1.8%/U→L 0.3%；black 15%/3.5%/1.4%；additional 4.7619%/1.9608%/0.4975%。
- 选项位置：第1行=当前等级；第2/3行低一级，按"逃逸选项"概率（rare 10%/epic 20%/unique 5%/legendary 3%）可能同级。
- 白金/惊人魔方：全部潜能拉满当前等级上限（B..SS）。
- 金锤：70% 成功率，+1 升级槽，最多 2 次；白金锤 100%。
- 星之力：0→25，按公开成本/成功率表；高星失败掉星/破坏。
- 全部集中在 `EnhanceConfig.java`，可单点改；注明"参考 Nexon 公示，非 192 私服实测"。

## 资源节点

- `Data/UI/UIWindow2Enh.img`（GMS，43 节点，SHA `7ca40557…`）
- `Data/UI/UIToolTip.img`（GMS，SHA `7733678e…`）
- 解析入口：`UiAssetResolve::window_enh(name)` / `UiAssetResolve::tooltip()`
