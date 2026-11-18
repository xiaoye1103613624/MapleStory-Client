# 加深轮（2026-11-16 晚）

## 证据

| 项 | 源 | 结论 |
| --- | --- | --- |
| Enchant Effect | orange-wz `EnchantSkill/Effect` 0..25 | 多帧序列；`Animation(Effect)` 播一次 |
| RefreshToolTip | Angel `0x7f675d` | 成功/失败/毁物/白卷文案；`EnchantSuccess`/`EnchantFailure`；毁物关窗；MakeClone |
| 商城确认 | MinMS `UICashBuyConfirm` + `CashShop.img/CSNotice/4` | 桌面 scale=1；支付勾选 NX/Maple；映射 useNX 1/2/4 |
| CD 秒数 | MinMS 行条 + 本端 IconCover | 图标下叠剩余秒文本 |

## 改动摘要

- `UIEnchantSkill`：Effect 全序列；`on_scroll_result(success,destroyed)`；音效+中文结果键；毁物关窗
- `UICashBuyConfirm` 新窗替换 Buy 的 `UIYesNo`
- `UICooldownList`：剩余秒显示
- 礼物仍 YesNo+SoftKey（无原生礼物确认条时保留）

## 真机

- Release 构建 EXITCODE=0（含 `UIStateCashShop` 叠层修复后再编）。
- **根因翻案**：`UICashBuyConfirm`/`UISoftKey` 的 Type（NOTICE/SOFTKEYBOARD）枚举值小于 `CASHSHOP`，`UIStateCashShop::draw` 升序绘制时商城底板盖住确认窗 → 光标可点、窗体不可见。已改为 focused 后绘，并补 `send_key`/`has_active_focused`。
- orange-wz：`CSNotice/4` 0..4、`EnchantSkill/Effect` 0..25、`UtilDlgEx` BtYes/BtNo 齐。
- 进图复验一度被幽灵 pid 47560「账户正在连接」挡住；叠层修复后需再进商城点「购买」目视 CSNotice。

## 续完（预览 / 礼物 / PartyHP）

| 项 | 结论 |
| --- | --- |
| 预览跟帧 | `avatar_look.update(TIMESTEP)` + `draw(DrawArgument, inter)`，不再每帧锁 STAND1/frame0 |
| 试穿 | 货板左图标区点击 toggle；脸/发/1xxxxxx 装备；同槽互斥；`BtDefaultAvatar`/`BtTakeoffAvatar` 回进店快照。无宠物/特效沙盒（显式预留） |
| 礼物确认 | orange-wz：`CSNotice` 仅 0..4，无 Gift 节点 → **保留 YesNo+SoftKey** |
| PartyHP | 计划本就内嵌 UserList；独立 CUIPartyHP 窗 **显式预留**（当前 `GAUGE_FILL_W=63` + BtHP 开关已够用） |

## 商城预览沙盒闭环（2026-11-16）

| 项 | 结论 |
| --- | --- |
| 物理 | `Foothold` 程序化构造；`FootholdTree`/`Physics::install_floor`；井内单段地板 + walls/borders |
| Ladder / Player | `Ladder(x,y1,y2,bool)`；`Player::use_sandbox_fht`；沙盒时跳过 `MovePlayer`/`MovePet` |
| UICashShop | `init_preview_sandbox` + `apply_preview_controls`；脚底跟 `preview_actor`；方向/跳进 `send_action`；`UIStateCashShop::send_key` 转发 key-up |
| 井坐标 | 相对 CSChar：`(24,40)` 宽 212；地板 y=134；梯 x=150；出生 x=86（Angel/MinMS） |
| REA | MCP 不可用；IDA 无可用 CCSWnd 符号（字符串层仅 CashShop）→ 以 MinMS + IMG 为准 |

### 本轮预留（非目标）

- 预览触控十字键 UI（MinMS 已撤，仅键盘）
- 预览宠物 / 现金特效 Animation
- 改 Stage 真身外观或发包试穿
