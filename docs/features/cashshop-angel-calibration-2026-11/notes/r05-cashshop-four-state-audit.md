# r05 · 商城既有文档四态审计 + OnCashItemResult 48-case 分列

> 第七面 CashShop 校准 · 2026-11-09
> 性质：只读对照 / 审计，非实现提交。全程未改源码。

## 0. 结论
1. 当前端商城 = **独立真窗**：`UICashShop : UIElement`（`TYPE = UIElement::Type::CASHSHOP` / `FOCUSED = true` / `TOGGLED = false`）+ 独立 `UIStateCashShop`（构造 `emplace<UICashShop>()`，`Stage::change_state(CASHSHOP)`）。与第五 / 六面「DIY 合并壳」形态不同；既有 beidou-v17-parity 里按合并壳写的判断标注为**历史口径、适用面不同**，不推翻、不互换。
2. Angel 083 `CCashShop::OnCashItemResult` 内层 **48 case** 已完整分列（正值 31 + 负值 17），其中 **24 个分支目标落在未命名 `sub_47xxxx`**，IDA 未给修饰名、无字符串锚点；本轮不臆测语义，记 `⏳ 待补符号`。
3. 四态中本面**无「推翻」项**：README 98 条以销案 / 新增 / 遗留为主；冲突项一律交用户裁决，不盲改。

## 1. 四态定义（沿用第六面模板 §5）
| 态 | 含义 | 处置 |
| --- | --- | --- |
| 销案 | 既有文档疑虑被一手证据定论 | 引用证据，归档 |
| 推翻 | 既有结论与一手证据相反 | 显式列出冲突与证据链 |
| 新增 | 本面新发现，既有文档未覆盖 | 补入 README §3 / §4 |
| 遗留 | 证据不足（IMG 阻塞 / 符号缺失） | 判 `⏳`，入 P2 / P3 |

## 2. 既有文档审计（docs/features/beidou-v17-parity/notes/）
| 既有笔记（商城相关） | 主题 | 四态 | 依据 |
| --- | --- | --- | --- |
| r103 / r108 | 商城入口与 opcode | 销案 | `Net/OutPacket.h:70` `ENTER_CASHSHOP = 40`、`:270` `CASHSHOP_OPERATION = 229` |
| r178 / r210 | 商城 handler 读序 | 销案 | `Net/Handlers/CashShopHandlers.cpp` `QueryCashResultHandler` 3×`read_int` |
| r218 / r219 / r220 / r221 / r222 | `CashShopOperation` mode 分发 | 销案 | 同上 `0x4B/0x4D/0x4F/0x55/0x57/0x5C/0x5E/0x60/0x62/0x64/0x68/0x6A/0x6C/0x85/0x87/0x89/0x8D/0x9E/0xA0` |
| r235 / r260 / r268 | 229 包构造族 | 销案 | `Net/Packets/CashShopPackets.h` 11 个 packet 全 `OutPacket::CASHSHOP_OPERATION` |
| r274 / r277 / r278 / r280 / r284 | 现金道具 79 / 230 / 256 | 销案 | `Net/Packets/CashItemPackets.h` + `Net/OutPacket.h:131/265/272` |
| r345 / r507 / r518 | 状态、余额、心愿 | 销案 | `IO/UIStateCashShop.h` static 三币种 + `cash_balances_ready` + `cash_wishlist std::array<int32_t,10>` |
| r771 | UI 形态 | 遗留→注明适用面 | 本面独立真窗结论与「合并壳」口径分属不同面，标注即可，不互相推翻 |

> 上表按 `ops_glob` 命中的商城相关既有笔记归组；逐条正文以 README §9 证据索引为准。

## 3. OnCashItemResult 48-case 分列（Angel 083 一手反编译）
**正值 31**：`71, 75, 76, 77, 78, 79, 80, 85, 86, 87, 88, 89, 91, 92, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110`

**负值 17**（`Decode1` unsigned byte 溢出成 signed char，实为 133..162 区间）：
`-123, -122, -121, -120, -119, -118, -117, -116, -115, -114, -111, -110, -102, -101, -98, -96, -95`

外层 `CCashShop::OnPacket` 另 **11 case** = `323, 324, 325, 326, 327, 328, 329, 331, 332, 333`（**无 330**）。内外两级分发，严禁混列。

## 4. 24 未名分支（`sub_47xxxx`）处置
| 项 | 值 |
| --- | --- |
| 数量 | 24 |
| 形态 | 分支目标为 IDA 未命名 `sub_47xxxx`，无修饰名、无字符串锚点 |
| 本轮动作 | 不臆测语义；README §3 C 协议维记 `⏳`；列入 §8 限制与 §7 P3 |
| 后续路径 | IDA 补符号 / 动态跟包（s9 `SendOpcode.CASHSHOP_OPERATION = 325` 回包按 mode 反查）后回填 |
| 排除 | 不引 095 线 `opcode-v95/dec/_OnPacket_CCashShop__UAEXJAAVCInPacket___Z.c`（083 基准禁引） |

占位结构（证据落盘前保持空槽，不以猜测填充）：

| case 值 | sub_ 地址 | 读序字段 | 对应当前端 mode | 判定 |
| --- | --- | --- | --- | --- |
| `<待补>` | `<待补>` | `<待补>` | `<待补>` | ⏳ |

## 5. 双端 c2s opcode 对照（供 README §3 C 维引用）
| 方向 | Angel 083 | 当前端 s9 | 判定 |
| --- | --- | --- | --- |
| Angel c2s 7 | `21 / 38 / 68 / 123 / 229 / 230 / 256` | — | Angel 一手（r01 §5） |
| 当前端 c2s 5(+1) | — | `40 / 79 / 228 / 229 / 230`（`+256` 跨面 MapleLife） | s9 一手（`Net/OutPacket.h`） |
| 重合 | `229 / 230`（`+256`） | `229 / 230`（`+256`） | ✅ |
| 差异 | `21 / 38 / 68 / 123` | 子功能并入 `UICashShop` + `UIStateCashShop` 逻辑，无独立 opcode | ⚠️ s9 优先 |

**红线**：改回原生 opcode 会断 s9 联机；所有冲突交用户裁决（README §6 D1–D14），不得盲改。
`Net/OutPacket.h:235` 注释「STORAGE / PLAYER_INTERACTION / CASHSHOP_OPERATION 三处同类修正一致」为既有协议对齐留痕，属 s9 一手口径。
