# #14 装备染色（ColorPrism 全装备）· Phase0 取证与范围决策（2026-11-18）

## 结论：渲染引擎级移植，需用户拍板范围，非可自动一次性闭环目标

参考包 `E:\MXD\扩展改动\装备染色\coloring-prism\`（用户已选此包，满足"支持所有普通+现金装备"）：

### 规模（实测 wc -l）
- 客户端 `coloringprism.cpp` 2575 行 + `weapontint.cpp` 3270 行 = **6196 行**，全是 Kaentake Detours 注入。
- 服务端 3 文件（2 java + 1 xml schema，18 列，无新表无 FK）。

### 客户端硬骨头（原生无法照搬）
`INTEGRATION.md` + README §How the recolour works：
- `weapontint.cpp` Detours `CAvatar::PrepareActionLayer @0x00453AD1`（本体）、`0x00453696`（脸/Eyes 页）、`0x0093BEB9`/`0x0093C218`（现金特效）——**4 个 v83 exe 固定地址 hook**。
- 改色 = 在 PrepareActionLayer 调用期内把 HSV 变换后的 canvas **clone 换进 WZ 树**再 RAII 还原；含 UOL alias 处理（记录 raw slot 非 resolved canvas，避免破坏间接引用）。
- 皮肤/头发：swap `Character/000020NN.img`+`000120NN.img`；披风 aura：`Effect/ItemEff.img`；现金特效：两 Detour。
- **原生客户端（MapleStory-Client 源码）没有这套 WZ canvas 间接层 hook 机制**——需重写为原生渲染管线（Canvas/Texture/Animation 在 Char.look 层按 HSL 生成 tinted texture 缓存），是引擎级改造。

### 服务端（可移植，但有 DDL 前置）
- 18 列：`inventoryequipment` tint*/tintfx*(6)、`characters` hair/eyes/skin tint(9)、`inventoryitems` efftint*(3)。
- ⚠️ 列名永久（Liquibase checksum）；`efftint*` 命名避 Duplicate column（ItemFactory SELECT * join）。
- ⚠️ 触碰 `ItemFactory` INSERT 两处（saveItemsCommon + saveItemsMerchant）——改错 merchant 保存会 `Column count doesn't match`。
- ⚠️ 用户此前选"暂不碰库只落 SQL"——但染色**必须**有这 18 列才存色，同 #8 需专项 DB 通道决策。
- opcode 0x372E/0x372F（s9 需确认空闲后重排）。

## 决策点（阻塞，需用户拍板才能继续）
1. **范围**：
   - A 全量：装备+皮肤+头发+眼+技能+现金特效全染（对齐包能力），引擎级重写，工程量最大。
   - B 首期只装备本体（tint*，砍 tintfx/skill/effect/look），UI+服务端先行，验证改色管线后再扩。
   - C 先只做服务端数据层 + UI 窗（滑块+快照+存库），客户端"改色生效"留后续专项（不改变像素，仅数据闭环）。
2. **改色管线**：原生无现成 hook——是否接受在 `Character/Look/EquipLook`/`BodyLook` 层新增"per-item tinted texture 缓存"作为新渲染路径？（动核心 look 渲染，爆炸半径需评估）
3. **DB 通道**：18 列 DDL——独立扩展 SQL 只落盘待用户执行？还是这次授权建列？

## 建议
选 **B+C 折中**：先做服务端 18 列(只落SQL)+UI 窗(滑块/预览/存色/发包)，客户端改色先实现"装备本体 tint 的 texture 缓存路径"最小可用版，跑通端到端再扩 fx/look/skill。避免一次性吞 6196 行引擎改造导致长期不可验证。

## 本轮动作
只读取证，零代码改动。等用户拍板范围后再进 Plan/实现。
