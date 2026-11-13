# 商城同观感复刻 · s9 协议 + 双交易闭环 + 周边深度

> 2026-11-12 · 决策：协议跟 s9；Angel 只校布局/交互/状态机；含普通面交与现金交易房；不含 MTS。

## 交付摘要

| 波次 | 内容 | 状态 |
|---|---|---|
| W0 | IDA TradingRoom/CashTrading/PersonalShop；orange-wz IMG 解析失败→.nx 回退 | ✅ `notes/r01-w0-evidence.md` |
| W1 | `UICashShop` 去 stub，顶栏接真实职业/角色名；七面板映射注释 | ✅ |
| W2 | `CashShopOperationHandler` tip → locale；`0x5C` 走 NoticeFailReason 文案 | ✅ |
| W3 | `UITrade` Angel 按钮坐标；席位相对 `my_number`；`UITradingRoom`/`UICashTradingRoom` 委托 `UITrade` | ✅ |
| W4 | Personal/Entrusted/Member `O` 拉起 `UIMerchant` 货架 | ✅ |
| W5 | build_smoke + 联机清单 | ✅ 曾 `EXITCODE=0`；续作见 r02 |
| W6 | orange-wz 节点回填 + 按 IMG 接线 | ✅ `notes/r02-img-node-parity.md`（完整 128B key） |

## 联机清单（待实机）

1. 进商城：顶栏职业/角色名非 stub；余额三币种；买/赠/心愿/取出存入
2. 双角色普通面交：邀请→放物/钱→聊天→双 CONFIRM→成功；再测取消
3. 打开 `UITradingRoom` / `UICashTradingRoom` 快捷入口应落到 `UITrade`
4. FM：PersonalShop CREATE + `O` 开 `UIMerchant`；雇佣店同理

## 红线

- 未改任何 c2s/s2c opcode 数值
- MTS 未做
- orange-wz 对 live CashShop/UIWindow.img 本机解析失败，布局以 Angel OnCreate + `.nx` 为准
