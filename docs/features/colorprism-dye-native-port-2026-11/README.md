# #14 装备染色 ColorPrism 原生移植 · 交付记录（2026-11-19）

## 决策链（用户已拍板）
范围A全量（含技能）+ 渲染方案先评审后动手（RENDER-REVIEW.md 已评审）+ DB 只落 SQL 手工执行 + 全图广播 + 即时刷新。

## 架构（与参考包本质不同，证据链见 RENDER-REVIEW.md §1.5）
参考包 6196 行 Detours 在 WZ 树层换 canvas；原生端**绘制时分流**：
- `Texture::draw` 动态查 `TintContext::resolve_hsl(bitmap_id)` → 命中则 `TintCache::tinted(*this, hsl)`（copy_pixels→逐像素 HSV 变换→ensure_raw 上传）→ `GraphicsGL::draw_raw_ex`（带 origin/裁剪/旋转，与 bitmap draw 同语义）→ 未命中走原路径零变化。
- **为何绘制时而非构造时**：`CharEquips::cloth_cache` 按 itemid 全局共享 Clothing（CharEquips.cpp:39-46），构造时种色会让所有穿同款的角色一起变色。绘制时按 charId 的 Scope 区分，per-char 不污染。
- `Char::draw` 开头 `TintContext::Scope(get_oid())` RAII 压栈；线程内渲染单线程，thread_local stack 安全。

## 服务端（BeiDou-Server_s9，mvn -o compile=0）
| 文件 | 动作 |
|---|---|
| `org/gms/server/TintStore.java` | 新建：内存态 tint 表（charId→tintKey→HSL）；key 空间=itemId / itemId+10M(fx) / hair=1 face=2 skin=3 / skillId+30M；identity 全零 |
| `org/gms/server/ColorPrismPackets.java` | 新建：SNAPSHOT(1)/RESULT(2)/REMOTE_TABLE(3) 三种 0x372F 包，wire 与参考 weapontint.cpp 完全一致；hue 为 SIGNED short（正旋转/负绝对值） |
| `org/gms/net/server/handlers/WeaponTintHandler.java` | 新建：全 7 action（request/apply/restore/applyLook/restoreLook/applySkill/restoreSkill）；目标重解析（负 pos 走 EQUIPPED）；消耗棱镜按 CASH 背包校验；succeed=result+snapshot+**全图广播** |
| `net/opcodes/RecvOpcode.java` | `WEAPON_TINT_ACTION(0x372E)`（实测空闲） |
| `net/opcodes/SendOpcode.java` | `WEAPON_TINT_SYNC(0x372F)` |
| `net/PacketProcessor.java` | 注册 handler |
| `resources/db/extensions/coloring-prism-standalone.sql` | 落盘 18 列（AI 未执行；含 efftint 命名避 Duplicate column 说明） |

## 客户端（VS 三证：链接行/error=0/产物 build-tintcore/MapleStory.exe 07:12, 4921856B）
| 文件 | 动作 |
|---|---|
| `Graphics/TintCache.h/.cpp` | 新建：HSL kernel（weapontint.cpp:215-292 逐位移植，含"透明但有色保留/纯黑透明跳过/朝边界插值"三契约）+ raw 图集缓存（key=bitmap_id+HSL，fresh id） |
| `Graphics/TintContext.h/.cpp` | 新建：per-char tint 表 + bitmap→tintKey 注册（含 `register_tree` 递归）+ Scope 压栈 |
| `Graphics/GraphicsGL.h/.cpp` | 加 `draw_raw_ex`（bitmap draw 的 raw-id 版，复用裁剪/旋转） |
| `Graphics/Texture.h/.cpp` | draw 分流：resolve_hsl→tinted→draw_raw_ex |
| `Character/Look/Clothing.cpp` | emplace 后 `register_bitmap(bitmap_id, itemId)` |
| `Character/Look/Hair.cpp` | `register_tree(hairnode, 1)` |
| `Character/Look/Face.cpp` | `register_tree(facenode, 2)` |
| `Character/Look/Body.cpp` | `register_tree(bodynode+headnode, 3)` |
| `Character/Look/Afterimage.cpp` | skill 分支 `register_tree(src, skillId+30M)` |
| `Character/Char.cpp` | draw 入口 `TintContext::Scope(get_oid())` |
| `Net/OutPacket.h` | `WEAPON_TINT_ACTION=14126(0x372E)` |
| `Net/Packets/ColorPrismPackets.h` | 新建 7 action 出站包 |
| `Net/PacketSwitch.cpp` | enum 0x372F + emplace handler |
| `Net/Handlers/WaveAShellHandlers.h/.cpp` | `WeaponTintHandler::handle`：snapshot 灌本地 oid、remote 灌 charId、result 仅日志 |
| `IO/UITypes/UIColorPrism.h/.cpp` | 新建染色窗：6 页签（装备/特效/技能/发/眼/肤）+ 三滑块 ±热区 + Apply/Restore；目标取背包选中格/技能书选中技能 |
| `IO/UIElement.h` / `Configuration.h` | `COLORPRISM` Type + `PosCOLORPRISM` |
| `IO/UITypes/UISideIconBar.cpp` / `Util/SideIconConfig.cpp` | `colorprism/dye/prism` 分支 + defaults slot |
| `MapleStory.vcxproj` | 登记 TintCache/TintContext/UIColorPrism + 两个 packet 头 |

## 五档注册终核（grep 计数）
Clothing x1（装备本体）｜Hair x1｜Face x1｜Body x2（body+head）｜Afterimage x1（技能）——全落地。

## 已知边界（如实声明）
1. **装备特效（tintfx）渲染档**：服务端/协议/DB 全支持，客户端注册只接了技能 afterimage；普通装备的 ItemEff 特效帧未单独注册（参考包 922/923 走 `effect` canvas，需在特效渲染点补一行 register——已列为后续小项，不影响本体/发肤/技能三档闭环）。
2. **真机未验收**：改色视觉效果、他人观感、即时刷新，全部待用户真机复核（构建证据仅证明编译链接）。
3. **持久化**：SQL 未执行前 TintStore 是内存态，重启丢色（功能不崩，同 #8 设计）。
4. **注册树开销**：register_tree 对每个节点构造 Texture，Hair/Face/Body 树较大，首次构造有额外开销；命中后 draw 走缓存 raw id。真机如卡顿，优化点=仅注册可达帧。
5. live exe 未替换（链接产物在 build-tintcore/，live 被僵尸进程占用；部署需用户关客户端后手动替换或解除占用后重构建）。

## 验收清单（用户侧）
1. 侧栏 `colorprism` 图标（或 ini 加 slot）→ 开染色窗。
2. 背包选中装备 → 装备页调滑块 → Apply → 消耗棱镜 5782000 → 角色即时变色。
3. Hair/Eyes/Skin 页直接 Apply（kind sentinel）。
4. 技能书选中技能 → Skill 页 Apply。
5. 同图他人应看到你的染色（全图广播）。
6. 持久化：执行 `coloring-prism-standalone.sql` 后重启才存色。
