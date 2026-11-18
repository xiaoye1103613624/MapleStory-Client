# 客户端侧取证（available() >= N 系统性 bool 误用）

## 定义层铁证
- `Net/InPacket.h:37` `bool available() const;` —— 返回类型是 bool。
- `Net/InPacket.cpp:30-33` `bool InPacket::available() const { return length() > 0; }` —— 只回答"有无剩余字节"。
- `Net/InPacket.h:39` / `Net/InPacket.cpp:35-38` `size_t length() const { return top - pos; }` —— 真正的剩余字节数。
- C++ 整数提升：bool→int 为 0/1，`1 >= N`（N≥2）恒假。

## 范式判定
| 写法 | 求值 | 结论 |
| --- | --- | --- |
| `available() >= N`，N≥2 | 1>=N / 0>=N 均假 | 🔴 恒假，bug |
| `available() == 1` / `>0` / 单独 / `>=1` | 等价 available() | 🟢 无害 |
| 三元 `available()>=N ? read_x() : default` | 恒取 default | 🔴 字段恒被吞成默认值 |

## 精确清单（排除注释/备份/>=1）
- 有效 bug 点 **329 处**，12 个源文件；CSV 见 `evidence/client-sites.csv`。
- 分布：WaveAShellHandlers.cpp **200**、MapObjectHandlers.cpp **88**（两者占 87.5%）；PlayerInteractionHandlers 13、MessagingHandlers 8、TradeHandlers 8、AllianceHandlers 3、InventoryHandlers 3、QuestHandlers 2、CommonHandlers/FredrickHandlers/PlayerHandlers/TestingHandlers 各 1。
- `Helpers/ItemParser.cpp` 2 处已在上一轮（s9-protocol-align）修复为 `length()>=10/>=5`，本轮不再命中。
- 活跃度：279/329 处落在 `Net/PacketSwitch.cpp` 已 `emplace` 注册的 handler（注册总数 304），即绝大多数是活跃协议路径，非 stub。

## 改动安全性的机制分析
- 所有 329 处均为 `if(available()>=N)` 门控或 `available()>=N ? read : default` 三元，handler 尾部普遍有 `while(recv.available()) skip_byte()` 清尾。
- 改成 `length()>=N` 后：length()<N 仍走 default/跳过，**不会越界**（length() 是真实剩余字节）。
- 唯一风险：若服务端对某 opcode 根本不发字段 X，但后续字段凑够 N 字节，`length()>=N` 会误判并读错位——故必须服务端发包证据（子任务 server-sender）逐 opcode 核对。

## 已修复先例（证明模式会咬人）
- 商城购物车永远空：CashShopHandlers.cpp 约 75 处 available>=N 恒假 → cash_inventory 读成 0 → UICashShop 过滤；改 length()>=N 后 3 件到货项立即显示。该文件的注释明确记录了"`available() >= 10` 恒为 false"导致 SET_FIELD 错位。
- 装备潜能/强化段：ItemParser.cpp:129-150 注释坐实 available>=10/>=5 恒假，改 length() 后修复。
