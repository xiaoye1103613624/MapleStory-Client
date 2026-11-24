# 自由市场雇佣商店 · 原生还原（范围 A）

> 日期：2026-11-20 ｜ 状态：代码已落地，待 FM 实机验收

## 1. 目标与决策

- **范围 A**：FM CREATE + 上架 + 访客购买 + `UIWindow.img/PersonalShop` 原生 UI
- **输入**：窗内 Textfield + 物理键盘；SoftKey 仅 PIC（见 [softkey-hotkey-native-restore](../softkey-hotkey-native-restore/README.md)）
- **二期预留**：黑名单 / 已售 / VisitList / 完整拖拽 / 场标点击进店 / owner 包体分支

阶段 0 差距见 [notes/phase0-gap.md](notes/phase0-gap.md)。

## 2. 实现逻辑

```text
现金栏 514* → UIPersonalShop（店名 Textfield + BtStart）
现金栏 503*/545* → UIEntrustedShop（CREATE type5 / OPEN_STORE）
  → dispatch_shop_create_or_open（FM 校验 + HiredMerchantCreatePacket）
  → s2c PLAYER_INTERACTION ROOM sub=4|5 / 0x19
  → UIMerchant（PersonalShop 底板 + BtBuy/BtStart/BtExit…）
  → BtBuy → HiredMerchantBuyPacket；BtItem → SET_ITEMS（背包选中 + EnterNumber）
地图 SPAWN_HIRED_MERCHANT → FieldMarker::CHARBOX（点击进店二期）
```

### 关键文件

| 文件 | 改动 |
|---|---|
| `IO/UITypes/UIMerchant.{h,cpp}` | 迁 `PersonalShop` 原生底板/钮；购买/上架/开业 |
| `IO/UITypes/UIMiscShells.{h,cpp}` | Personal/Member/Entrusted 开店壳 + Textfield CREATE |
| `Net/Handlers/HiredMerchantBoxHandlers.*` | 场标 Kind=CHARBOX |
| `Net/Handlers/TradeHandlers.cpp` | owner 块二期注释 |
| `locale/zh-CN.ini` / `en-US.ini` | merchant.* / create_hint |

## 3. SoftKey 结论（摘要）

不是软键盘实现坏了，而是历史 parity 误把 SoftKey 当万能输入。Angel 聊天/店名走 `CCtrlEdit`；本端商店 SoftKey 掏空后未接 Textfield 导致 CREATE 断线。本版已接回正途。

## 4. 验证

- [x] `tools/build_smoke.bat` GATE OK + EXITCODE=0（2026-11-20）
- [ ] 非 FM：红字「仅自由市场」、不发包（待实机）
- [ ] FM + 514：店名 → CREATE type4 → ROOM sub=4 → UIMerchant（待实机）
- [ ] FM + 503：CREATE type5 → ROOM sub=5（待实机）
- [ ] 背包选中 + BtItem 上架 → 0x19 刷新（待实机）
- [ ] 访客选格 + BtBuy（待实机）

清单同步：[pending-live-tests §4](../pending-live-tests/README.md)

## 5. 二期

- `DeliverBlackList` / `CSoldItemDlg` / VisitList
- 拖拽 PutItem（Angel `CDraggableItem`）
- 场标点击进店
- 店主 ROOM 包体与访客区分
