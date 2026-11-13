# r03 · orange-wz(IMG) 门禁阻塞与替代取证链小结

> 第七面 CashShop 校准 · 证据分级说明 · 2026-11-09
> 性质：只读取证说明，非实现提交。全程未改任何源码。

## 0. 结论
三联门禁（REA / IDA / orange-wz(IMG)）中，前两路已闭环，第三路 orange-wz MCP 本会话不可用。
因此 CashShop 的 **IMG 资源层**（位图、图标、试衣 Sprite、标签纹理、MVP gauge 贴图、窗口九宫格、Sound/BMS）无法取得一手解析；凡依赖 IMG 实机的校准点在本面 README §3 一律判 `⏳`，不臆测、不编造资源帧号与贴图内容。

不受影响的部分（一手证据充分）：EA 字节级比对、反编译语义、c2s/s2c opcode、handler 字段读序、UI 类/状态结构、包构造字段。

## 1. 三路门禁状态
| 门禁 | 端点 / 工具 | 状态 | 产物 |
| --- | --- | --- | --- |
| REA | shipped-artifact 既有解析 | ✅ | r01 §1 |
| IDA Angel | 127.0.0.1:13339（pid 29620, imagebase 0x400000, strings 1281） | ✅ | evidence/angel-cashshop-ea.json 等 6 件 |
| IDA BeiDou | 127.0.0.1:13337（pid 31332, strings 3889） | ✅ | evidence/angel-beidou-cashshop-ea-bytelevel-audit.json |
| orange-wz(IMG) | MCP | ⚠️ 阻塞 | 无 |

## 2. 替代取证链（本面实际采用）
1. `.nx` 资源包 + `nx_inspect.py`：仅在需要节点名 / 路径时用于交叉印证，**不据此断言像素级样式**。
2. 正确客户端截图库 `C:\Users\11036\Desktop\正确客户端截图\`：作为 `F 样式` 维的**人工体感输入**，明确标注为输入而非证据。
3. 源码层 `IO/UITypes/UICashShop.h` 枚举与常量（`MAX_ITEMS = 7u*2u+1u = 15`、20 个 `Buttons`、22 个 `Item::Label`、`preview_sprites[3]` / `menu_tabs[9]` / `mvp_sprites[7]`）：作为控件**结构**证据；sprite 具体帧与贴图内容仍待 IMG。

## 3. 降级清单（README 判 `⏳` 的类别）
- 试衣间 3 个 preview slot 的实际外观帧
- `Item::Label` 22 种标签位图（ACTION / BOMB_SALE / BONUS / EVENT / HOT / LIMITED* / LUNA_CRYSTAL / MASTER / MUST / NEW / SALE / SPEICAL / SPECIAL_PRICE / TIME / TODAY / WEEKLY / WONDER_BERRY / WORLD_SALE / NONE）
- MVP gauge 与 `mvp_sprites[7]` 分级贴图
- 9 个 `menu_tabs` 的选中 / 未选中态位图
- 窗口九宫格拉伸与像素对齐、坐标绝对精度
- `CashShop.img` / `Commodity.img` 节点树与 BMS / Sound 资源

## 4. 处置
- 上述项进入 README §7 的 **P2（需 IMG 实机 + build_smoke.bat）**，不在本面 P0 文档闭环里虚报完成。
- orange-wz 恢复后，以本文作清单逐项回填；回填前保持 `⏳`。
- 明确排除：`docs/features/port-095/evidence/opcode-v95/**` 属 095 线，本面 083 基准禁引。`Net/Packets/CashShopPackets.h:182` `CubeRerollPacket` 的「095 魔方重洗请求」仅为**注释污染**，不作功能依据。

## 5. 复现与核对
- IDA 端点事实不可本地复跑，凭 `evidence/*.json` + `notes/r01-angel-native-cashshop-facts.md` 复核。
- opcode 门禁可本地跑：`python tools/gen_opcode_matrix.py --check`（期望 `GATE OK: opcode matrix matches both sources ...`）。
- 采集脚本：`tools/ida_angel_cashshop_{ea,decompile,normalize,crosscheck,facts}.py`。
