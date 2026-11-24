# #14 装备染色 ColorPrism · 原生渲染方案评审（2026-11-18）

> 用户拍板：范围 A 全量、渲染先评审再动手、DB 只落 SQL 手工执行。本件是**评审件**，零代码改动。

## 1. 核心结论：原生端有现成像素通道，全量染色可行，无需 hook WZ 树

参考包 6196 行 Detours 的本质是"在 `PrepareActionLayer` 调用期把 HSV canvas clone 换进 WZ 树再 RAII 还原"，因为 v83 原版只有 WZ canvas 间接层可改。**原生客户端完全不同**：它把每帧解码成 `Texture`（BGRA in memory），改色就是 CPU 变换 BGRA + 新图集 id 缓存，干净可控。

### 证据链（逐行号，已实读）
| 环节 | 位置 | 结论 |
|---|---|---|
| BGRA 读取 | `Graphics/Texture.cpp:137-157 copy_pixels()` | 每帧完整 BGRA 可取，`bitmap.length()=4*w*h`，渲染层 GL_BGRA 契约 |
| BGRA 回灌 | `Graphics/GraphicsGL.h:68 ensure_raw(id,w,h,bgra)` | 新 id 上传图集 |
| 内存纹理缓存 | `Graphics/MemoryTexture.h:24-50` `set/atlas_id/draw` | fresh id 即新版本，注释明确"changed pixel set must use fresh id" |
| 现成改色 kernel | `Graphics/MemoryTexture.h:52-90` `tint_disabled_*`/`build_tinted_regions` | 已有逐像素 HSL 风格的整数改色实现（矩形版），**整帧版可复用同一 kernel** |
| 装备帧落点 | `Character/Look/Clothing.cpp:121-205` | `stances[stance][z].emplace(frame, partnode)`，每帧一个 Texture；draw :218-224 |
| 头发/脸取数 | `Character/Look/Hair.cpp:30`、`Face.cpp:71` | `nl::nx::Character["Hair"/"Face"]`，同款 Texture/Animation 帧 |
| 身体/皮肤取数 | `Character/Look/Body.cpp` + BodyDrawInfo | 默认身体 canvas，层结构与装备一致 |

## 1.5 实现期校准（2026-11-18 21:55，读源码后修正评审假设）
评审 §1 原写"tinted 结果仍是 Texture"——**被源码推翻**：
- `includes/NoLifeNx/nlnx/bitmap.hpp:47-54`：`bitmap` 构造是 private，只 friend node，**外部无法用 raw BGRA 构造 Texture**。
- `Graphics/Texture.cpp:97-104`：`Texture::draw` 带 origin/vertical/horizontal 裁剪 + color + angle，语义比 `MemoryTexture::draw(Point)` 丰富，不能简单替换。
- `Graphics/GraphicsGL.h:68-69`：`ensure_raw(id,w,h,bgra)` + `draw_raw(id,x,y,w,h,alpha)` 已有，raw 与 nlnx bitmap 共用图集纹理，但 draw_raw **缺裁剪/旋转参数**。

**修正后的技术路径（已验证可行）**：
1. `GraphicsGL` 加 `draw_raw_ex(id, rect, vertical, horizontal, color, angle)`——把 bitmap draw(:65) 的裁剪/旋转逻辑抽成对 raw id 的重载。
2. `Texture` 加可选 `size_t tint_raw_id = 0`；`Texture::draw` 若 tint_raw_id 有效则走 draw_raw_ex（沿用同 origin/dimensions/裁剪），否则原路径。
3. `TintCache`（key=源 bitmap id + HSL）用 `Texture::copy_pixels(:137)` 取 BGRA → 整帧 HSL 整数变换（复用 MemoryTexture.cpp 的 kernel 口径）→ `ensure_raw` 上传 → 把 tint_raw_id 写回帧 Texture。
4. 结果：Clothing/Animation/Hair/Face/Body **零结构改动**，只在构造帧时按需种入 tint_raw_id；即时刷新 = 换新 tint id。

## 2. 改色管线设计（The Algorithm：质疑→删除→简化→加速→自动化）

**质疑**：真要在 WZ 树层做 clone+RAII 吗？——不需要，原生有 BGRA 通道。删掉整套 Detours/hook/UOL alias 处理（6196→约 800 行）。
**简化**：统一一个 `TintCache`，key=(图片源 id, 帧号, HSL)，value=MemoryTexture；命中复用，未命中 `copy_pixels→HSL变换→ensure_raw`。
**加速**：缓存按 (itemId, hue, chroma, bright) 粒度，一次染色全帧只算一次；切装/换色不重复算。
**自动化**：HSL 用整数查表（复用 MemoryTexture.cpp 现有 snap/整数口径），避免浮点。

### 数据契约（与服务端 18 列对齐）
- 装备本体：`tintHue/tintChroma/tintBright` → 作用于 `Clothing` 各帧。
- 装备特效：`tintFxHue/tintFxChroma/tintFxBright` → 作用于 Afterimage/ItemEff（`Character/Look/Afterimage.cpp`）。
- 头发/眼/肤：`characters` 9 列 → 作用于 `Hair.cpp`/`Face.cpp`/`Body.cpp` 帧。
- 技能：`skillId+30000000` keyd 的独立表 → 作用于 `SkillUseEffect`/`Combat`（需另定表，参考包是服务端独立表）。

## 3. 四档落点与爆炸半径（点到面）

| 档位 | 渲染落点 | 爆炸半径 | 风险 |
|---|---|---|---|
| 装备本体 | `Clothing.cpp` emplace 前 tint | 仅自己角色外观 | 低——缓存按 id 隔离，失败回退原 Texture |
| 装备特效 | `Afterimage.cpp` 层构造 | 特效绘制 | 中——afterimage 多帧共享，需按 itemId 分 key |
| 头发/眼/肤 | `Hair.cpp`/`Face.cpp`/`Body.cpp` | 所有角色外观 | 中——按 charId 的 HSL 需从服务端快照取，**同屏他人也要染** |
| 技能特效 | `SkillUseEffect`/`Combat` | 技能特效 | 高——技能无 inventory 行，id 映射复杂 |

## 4. 需用户拍板的关键决策（评审结论带推荐）

1. **他人染色同步**：服务端 snapshot 需广播同图所有人的 tint（否则只有自己变色）。参考包有 `broadcastMapTable`。→ 是否本轮就做全图广播？（推荐：做，否则功能观感残缺）
2. **技能染色**：技能 keyd 需服务端独立表 + `skillId` 映射，复杂度最高。→ 全量 A 是否包含技能？（推荐：A 拆两期，1期装备+特效+发肤，2期技能；若坚持一次全量则工期显著拉长）
3. **改色触发刷新**：染色后需让当前角色 look 重建（重新构造 Clothing 读缓存）。→ 是否接受"确定后换图刷新"（参考包原做法，最稳）还是做即时刷新（复杂）？（推荐：先换图刷新）

## 5. 推荐的落地顺序（若采纳）
1. 服务端 18 列 SQL 落盘 + handler + snapshot（含全图广播）。
2. 客户端 `TintCache` + HSL kernel（复用 MemoryTexture）+ 装备本体 tint（最小闭环，构建验证）。
3. 扩装备特效、发肤、技能。
4. 每步 VS 三证 + 真机待用户验收。

## 6. 本轮动作
只评审、零代码改动。等用户对 §4 三点拍板后进实现。
