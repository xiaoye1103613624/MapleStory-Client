# 背景闪烁问题调查与同类资源扫描（2026-11-16）

## 1. 用户问题

1. 录屏 `XiaoYe_2026-11-16_22-35-57.mp4` 里画面背景"一闪一闪"，是不是间隔太短？
2. 顺带检查是否还有别的地图资源存在类似问题。

## 2. 结论先行

**视频确实在闪，但不是"整体调光间隔太短"，而是画面被以约 9.4 Hz（106 ms）的周期反复"提亮一档再逐帧衰减"，形成亮度锯齿波。** 该频率正落在人眼最敏感的 8–12 Hz 闪烁带，所以格外刺眼。把间隔拉长不会消除闪烁，只会让它变慢——真正的抓手是"那次突然提亮从哪来"。

资源层面通过 orange-wz MCP 全量扫描 `Data/Map/Back/` 的 631 个背景 IMG，发现 **94 个含 alpha 渐变帧（a0/a1）的资源，其中 12 个资源（49 帧）符合"大幅 ramp + 短 delay"的闪烁特征**，列于第 5 节。**但经官方原版逐帧对照（第 5.1 节），这 49 帧全部是设计特效、无一被改坏——扫描是"找现象"，对照是"定性"，两者结论必须合起来看。**

第二轮追查（第 6.1 节）把视频那次闪烁高置信度锁定到 **`shineWood` 的 `back/16` 帧**（a0=235, blend=1, 100ms ramp，被地图 101000000/100050000 各 4 层引用）：源码级机制链与视频 106ms 波形逐项吻合，Effect 层已用排除法否定。**这同样是官方原版设计特效。**

## 3. 视频逐帧证据（PyAV 解码全部 211 帧）

| 实测项 | 数据 | 判读 |
| --- | --- | --- |
| 视频规格 | 3.53 s / 60 fps / 211 帧 / 1280×720 | 采集正常 |
| 闪烁周期 | 6.4 帧 = 106 ms ≈ 9.4 Hz，3.5 s 内 30+ 次 | 固定周期，非随机 |
| 全屏平均亮度波形 | 93.3 ↔ 95.3，骤升 +1.6~1.9 后每帧 -0.3 线性衰减 | 亮度锯齿波 |
| 单次提亮像素 | 变化像素中位 +20、均值 +27~37，87.5% 变亮 | 台阶式提亮 |
| 暗部/亮部差异 | 原亮度 0–60 的像素 +64，180–240 的 +20；corr(Δ,bg)=-0.263 | 非均匀加法，符合 alpha 混合特征 |
| 静态占比 | 每帧 92–95% 像素与前后帧完全相同 | 画面基本静止，闪的是叠层 |
| 编码伪影排除 | 210 对相邻帧零完全重复 | 真实渲染，非插帧 |

分区亮度跳变差异极大（左下 +5.41、小地图 +4.26、中央 +3.06，而右上 +0.03、底部 UI 栏 +0.00），说明**不是全屏统一调光，而是特定渲染层的周期变化**。

## 4. 根因机制（源码闭环，RCA 完成）

问题在渲染侧的背景动画透明度插值，不是资源"间隔太短"这么简单：

- `Gameplay/MapleMap/MapBackgrounds.cpp:48`：`animation = sheet[animated ? "ani" : "back"][src["no"]]`——背景/前景层按资源里的 `ani` 标志决定读动画帧分支。
- `Gameplay/MapleMap/MapBackgrounds.cpp:164`：`animation.draw(..., opacity / 255)`，其中 `opacity = src["a"]`（层常量）。
- `Graphics/Animation.cpp:36-56` `read_frame_timing`：帧节点带 `a0`/`a1` 时生成透明度渐变区间——
  - 仅 `a0`：`{a0, 255-a0}`
  - 仅 `a1`：`{255-a1, a1}`
  - 两者都有：`{a0, a1}`
- `Graphics/Animation.cpp:281`：`opacity += framedata.opcstep(timestep)` 逐帧推进透明度。
- `Graphics/Animation.cpp:341`：每切换一帧 `opacity.set(frames[nextframe].start_opacity())` 重置到起始透明度。
- `Graphics/Animation.cpp:33-34`：`delay==0` 时默认 `delay=100ms`。

**链条**：资源帧的 `a0/a1` 定义一个大幅透明度爬升 → 客户端每帧线性插值、到点重置 → 形成"暗→亮→骤暗"的呼吸/闪烁。视频实测 106 ms 周期与默认 100 ms delay 高度吻合。`a0/a1` 幅度越大、delay 越短，闪烁越强、越接近人眼敏感频段。

> 诚实边界（第一轮，保留原始记录）：录屏所在地图的背景层（已核对地图 104010000 的 `vicportTown.img` 20 层均为单帧 canvas、无 a0/a1）本身不构成候选，因此第一轮结束时视频里那个具体闪烁源尚未锁定。第二轮（第 6.1 节）通过地图引用链 + shineWood back/16 源码级机制闭环，把它高置信度锁定到 shineWood（官方设计特效）。"资源侧带大幅 a0/a1 + 短 delay 就会触发同类闪烁"的机制已由源码+实验双向确认，第 5 节清单即基于此判据扫描得出。

## 5. 同类资源扫描结果

工具：`orange-wz MCP`（HTTP `127.0.0.1:10012/mcp`，128B DEFAULT_KEY，符合 AGENTS.md 2026-11-09 门禁；全程未直接解析 .wz/.img 二进制）。
判据：Back IMG 的动画帧带 `a0`/`a1`，透明度跨度 `span>=64`，且 `delay<=300ms`（≈≥1.7Hz，落入可感知闪烁区）。
完整机读证据：`evidence/scan_result.json`；聚合清单：`evidence/risk_summary.md`。

| 资源 | 候选帧 | 最大 span | 最小 delay(ms) | 最高频(Hz) | 风险分 |
| --- | --- | --- | --- | --- | --- |
| ancientforest2.img | 1 | 255 | 150 | 3.3 | 255.0 |
| moltenRock.img | 2 | 255 | 80 | 6.2 | 255.0 |
| moltenrocka.img | 2 | 255 | 80 | 6.2 | 255.0 |
| moltenrockabc.img | 2 | 255 | 80 | 6.2 | 255.0 |
| shineWood.img | 1 | 215 | 100 | 5.0 | 215.0 |
| shinewoodolaola.img | 1 | 215 | 100 | 5.0 | 215.0 |
| dragonRoad.img | 7 | 195 | 100 | 5.0 | 195.0 |
| pl_abrup_kaftafell_night.img | 8 | 165 | 150 | 3.3 | 165.0 |
| starplanetpvp.img | 2 | 255 | 300 | 1.7 | 127.5 |
| globalJP2.img | 1 | 150 | 300 | 1.7 | 75.0 |
| dragondream.img | 18 | 128 | 300 | 1.7 | 64.0 |
| toyCastleDungeon2.img | 4 | 65 | 250 | 2.0 | 32.5 |

风险分 = opacity_span × 人眼敏感频段权重（3–12 Hz ×1.0，1.5–3 Hz ×0.5，其余 ×0.3）。

**高频项**：`ancientforest2`、`moltenRock` 系列、`shineWood` 系列、`dragonRoad`、`pl_abrup_kaftafell_night`——大跨度 + 落在 3–6 Hz 敏感带。

## 5.1 官方原版对照（决定性证据，推翻"整改资源"口径）

工具：`tools/_bg_xml_compare.py`；基线：`E:/project/BeiDou-Server/wz-zh-CN/Map.wz/Back/*.img.xml`（GMS 官方原版 XML dump）。机读证据：`evidence/xml_compare.json`。

**不能按 `a0/a1` 字段字面比对**——那会把渲染等价的资源误判为"被改坏"。按引擎 `Graphics/Animation.cpp:36-56 read_frame_timing` 的真实语义归一化后比较 opacity ramp：

- 两者都有 → `{a0,a1}`；仅 a0 → `{a0,255-a0}`；仅 a1 → `{255-a1,a1}`；都无 → `{255,255}`
- `delay==0`/缺失 → 引擎默认 100ms（`Animation.cpp:33-34`）

**结论：49 个候选帧按渲染语义归一化后 100% 与官方原版一致，0 个被本地改坏。**

- 唯一字面"不一致"的 `starplanetpvp ani/5/0,1`：live 写 `a1=255`、原版省略 `a1`；引擎对仅 `a0=0` 自动补 `{0,255}`，ramp 完全相同 → **假阳性**。
- 三份 mtime 落在 Nov12 的 live 文件（`ancientforest2`/`dragondream`/`starplanetpvp`）经逐帧比对，内容与官方原版渲染等价 → mtime 变化不等于改坏。

**定性翻转**：这些 a0/a1 大幅 ramp 不是本地 bug，是**官方设计特效**（岩浆发光、树林光影呼吸、Night 场景明暗交替）。因此"改资源收窄 ramp"这条路被否决——改了就是偏离原版 + 误杀正常特效。`Constants.h:29 TIMESTEP=8ms` 与 ramp 步进自洽，引擎未算错。

## 6. 最终决策（按 owner 意识给出，未擅自改任何资源）

1. **不改资源**：49 帧全部是官方原版设计特效，改 a0/a1 或 delay 属于偏离原版 + 误杀，否决。这是本次"对照"最核心的产出——避免了把正常特效当 bug 修。
2. ~~不轻易改引擎：全局加透明度钳制会误伤正常淡入淡出~~ → **2026-11-17 用户明确要求修复后已翻转**：不做全局钳制（仍会误伤），改为**外科手术式的单帧静态修复**，见 §6.2。
3. 视频闪烁定性：官方设计特效 + 引擎把"单帧静态 a0"误当互补 ramp 的**边界 bug** 共同作用——数据没错，是引擎对单帧的处理有缺陷。

## 6.2 引擎侧修复（2026-11-17 实装 + 真机闭环）

**根因收敛**：`read_frame_timing:43-46` 的 `{a0, 255-a0}` 互补 ramp 是为**多帧交叉淡入淡出**设计的（frame0 a0=200→55，frame1 a0=55→200 接力）。但 shineWood `back/16` 是**单帧静态背景**（a0=235，无 a1），没有配对帧接手，于是每轮 delay 到点 opacity 从 20 骤回 235，形成 ~100ms/9.4Hz 呼吸锯齿。**数据正确，是引擎对"单帧 + 仅 a0"这一边界的处理缺陷。**

**修复（2 文件）**：
- `Graphics/Animation.h`：Frame 新增 `has_explicit_a0` / `has_explicit_a1` 标志 + `freeze_opacity_if_static()` 声明。
- `Graphics/Animation.cpp`：两个 Frame 构造函数记录 a0/a1 是否存在；新增 `freeze_opacity_if_static()`；两个 Animation 构造函数在 `frames.size()==1` 时调用。
- **收紧后的判定（关键）**：仅当 **`frames.size()==1` + 显式 a0 + 无显式 a1 + `delay <= 150ms`**（约 ≥6.7Hz，人眼敏感频闪带）时，才把 ramp 端点压平成 `{a0, a0}`。

**为什么不误伤（收紧的由来）**：第一版只判"单帧 + 仅 a0"，用户指出"呼吸明暗是不是没了"后复查 XML，发现这会连带压平 moltenRock 的 `ani/1/10`(a0=0, delay=1900ms)、`ani/3/10`(a0=0, delay=3700ms)——那是 **0.5Hz/0.27Hz 的岩浆缓慢发光呼吸设计特效**，被压成恒定透明就是误杀。收紧加入 `delay<=150ms` 后：
- 高频频闪（shineWood `back/16` delay=100ms）→ 压平，消除 9.4Hz 刺眼闪烁；
- 缓慢设计呼吸（moltenRock 1900/3700ms、shineWood3 树林光影 50↔170 delay=2500ms、shineWood3 ani/9/2 delay=1100ms）→ **全部原样保留**。

影响面实测（原版 XML）：1205 个 alpha 帧中 1177 个有 a1（多帧交叉淡化，从不命中 freeze）；28 个"仅 a0"里只有 2 个（shineWood / shinewoodolaola 的 back/16 delay=100ms）被压平，其余 26 个低频帧全部保留。

离线确定性脚本 `tools/_flicker_fix_verify.py` **6/6 ALL PASS**：shineWood back/16 振幅 218→0、骤升 9→0；moltenRock 1900/3700ms、shineWood3 树林光影、ani/9/2 的呼吸序列修复前后逐点一致（ramp 完好）。

**验证闭环**：
- 构建：`build_flicker_fix.bat` 全新 IntDir 全量重编，error C=0，最终 exe SHA `6178eaf4`（收紧版）。
- 部署：canon `E:/MXD/BeiDou-Client-xy/MapleStory.exe` 原子替换为收紧版，40s 守护复查未回滚（登录器回滚老坑已防）。
- **真机（关键铁证，第一版即验证过压平有效）**：mapid=100050000（shineWood，与视频同图）录屏，最静止 60 帧连续段 **峰峰值 0.00、标准差 0.000、8–12Hz 峰值 0.000**——呼吸频闪完全消失（对照原视频 9.4Hz/峰峰值2/30+次骤升）。收紧版仅放宽了对低频呼吸帧的限制，对 shineWood delay=100ms 的压平行为不变，故该真机结论仍成立。采集脚本 `tools/_flicker_capture.py` + 分析 `tools/_flicker_compare.py`。

**诚实边界**：收紧版（SHA 6178eaf4）未再单独真机录屏（真机 A/B 采集连续 4 次因进图重建窗口失败，不刷无限验证）；其正确性由"离线 6/6 用例 + 逻辑上对 shineWood 100ms 帧行为与第一版完全一致 + 第一版真机铁证"三者共同保证。用户可直接双击 canon 亲眼确认呼吸明暗仍在。

**名词澄清（2026-11-17 用户追问"树林光影是单帧还是多帧、是不是明暗过渡"）**：必须区分两套完全不同的资源，此前表述含混，特此钉死——
- **魔法密林主图 101000000 / 南郊 100050000（视频同图）**的"一闪一闪背景"：`bS=shineWood` 的 **`back/16` 单帧**（back 容器第 16 号是一张 canvas，a0=235、无 a1、delay 默认 100ms、blend=1）。主图 44 个 back 层中第 8–11 层共 4 层引用 no=16。**它不是设计的明暗过渡**，而是引擎给"单帧+仅a0"硬套 `{235,20}` 互补 ramp 造成的 235↔20 骤亮骤暗锯齿（~9.4Hz 刺眼频闪）——**已压平，这是应该压的**。
- **真正的"设计明暗过渡呼吸"**是另一套资源 `shineWood3` 的 `ani/*`：**多帧交叉淡化**（每帧 a0=50↔170 / 30↔170，delay 2300–3400ms），这才是名副其实的"明暗渐变呼吸"。它只出现在**魔法密林码头 101000300、候船室 101000301 等 9 张图**，**不在主图 101000000 / 南郊 100050000**，且从来没被我的修复触碰过（有 a1，不命中 freeze）。
- 一句话：视频里魔法密林闪的是**单帧锯齿、非过渡、已修**；设计好的**多帧明暗过渡在码头等图、未动**。此前"树林光影多帧呼吸一直都在"指的是后者，用在这里属于指代不清，已澄清。

**"back 容器有 16/17/18 多张图" ≠ "动画多帧"（2026-11-17 用户现场观察后的二次澄清）**：
- `shineWood.img` 的 `back` 容器确实有 **38 张并列 canvas**（编号 0–37，含 16/17/18）——但它们是**38 张独立的背景图**，不是同一元素的 38 个动画帧。证据：`back/16` origin=(134,134)、`back/17`/`18` origin=(116,116)、`back/19`=(121,121)，位置各不相同，是画面上不同位置的独立背景元素。
- 引擎引用方式（MapBackgrounds.cpp:48）：`animation = sheet[animated ? "ani" : "back"][src["no"]]`——地图**每一层**用一个 `no` 值引用 back 容器里的**一张**图，各自当独立静态背景绘制。主图 101000000 的 44 个 back 层各自 no 不同（0/1/2/5/7/16/21/22…），其中 no=16 有 4 层。
- 所谓"单帧"，指**每层引用的那一张 canvas 在引擎里构成一个 frames.size()==1 的 Animation**（src 是 bitmap → Animation.cpp:178-183 `frames.push_back(src)`）。16/17/18 彼此**不是**动画帧序列，不存在"16→17→18 播放"这回事。
- 38 张里**只有 back/16 带 a0=235+blend=1**（无 a1/delay）→ 被引擎套 {235,20} ramp 才频闪；back/17/18 虽有 blend=1 但**无 a0** → read_frame_timing 给 {255,255}，不产生 ramp、不闪。修复只压平 back/16，没有误伤 17/18 或其它的任何一张。
- **没改错也没用错**：live `shineWood.img` 是 2024-08-04 原始未动；`shinewoodolaola.img` 虽 2026-11-12 改过，但其 back/16 字段（a0=235/blend=1）与原版 XML 一致。频闪是"官方资源 a0=235 + 引擎互补 ramp 机制"的交互结果，非本地改坏。

## 6.1 视频闪烁源的追查（第二轮，RCA 命中但保留诚实边界）

用户追加"可以"后，把视频那次 ~9.4Hz 闪烁往具体资源钉死。路径全靠**纯文本证据**（视觉后端本轮全挂：Vision Router 4 模型超时 + 本地 tesseract 未装 + modlens 无 provider），逐字追踪如下：

**① 地图引用链**（源：`wz-zh-CN/Map.wz/Map/Map1/*.img.xml`，ElementTree 精确解析 back 容器）：

| 地图 | bS(背景) | back 层数 | no=16 引用层 | ani |
| --- | --- | --- | --- | --- |
| 101000000 | `shineWood` | 44 | 4 层（name 8/9/10/11） | 0 |
| 100050000 | `shineWood` | 17 | 4 层（name 9/10/11/12） | 0 |
| 104010000 | `vicportTown` | 20 | 1 层 | — |

`setfield_trace.log` 末尾时段在 104010000 / 101000000 / 914000000 / 100050000 间反复切换；`101000000` 与 `100050000` 的背景都是 `shineWood`。

**② shineWood 的闪烁帧**（源：`wz-zh-CN/Map.wz/Back/shineWood.img.xml`，逐字原文）：

```
<canvas name="16" format="1" scale="0">
  <vector name="origin" x="134" y="134"/>
  <int name="z" value="0"/>
  <int name="a0" value="235"/>
  <int name="blend" value="1"/>
</canvas>
```

`a0=235`、无 `delay`、无 `a1` → 按 `read_frame_timing`：`{235, 255-235=20}`、delay 默认 100ms。地图层 `ani=0` 走 `sheet["back"][no]`（`MapBackgrounds.cpp:48`）。

**③ 引擎机制逐字闭环**（`Graphics/Animation.cpp`）：

- `:76` `Frame::Frame(src)` → `read_frame_timing(src, delay, opacities, scales)` —— **back canvas 的 a0 照样进 opacity ramp**（并非只有 ani 帧才生效）；ramp = `{235, 20}`，span=215。
- `:251` `reset()`：`opacity.set(frames[0].start_opacity()) = 235`。
- `:281` `update()` 每 8ms：`opacity += framedata.opcstep(timestep)`，`opcstep = timestep*(opacities.second-opacities.first)/delay`（`:142`），即从 235 向 20 线性爬升。
- `:320-323` 单帧动画 `frame==lastframe` → `nextframe=0`、`ended=true`；`:341` `opacity.set(frames[nextframe].start_opacity())=235` —— **每 100ms 从 20 骤升回 235，再逐帧衰减**，形成锯齿波。

这与视频实测的 **106ms 周期、骤升后每帧 -0.3 衰减、9.4Hz** 高度吻合（100ms 理论值 + 帧相位对齐误差 → 实测 106ms/6.4 帧）。`blend=1` 叠加模式解释了"暗部 +64、亮部 +20 的非均匀提亮"。

**④ Effect 层对照（蓝军排除法）**：扫 `wz-zh-CN/Map.wz/Effect.img.xml`（927KB，159 个 a0/a1 节点），命中 28 个 span≥64 & delay≤300 节点，其中 100ms 级（10Hz）8 个，完整路径：`crossHunter/blackout/1,4`、`xenon/blackout/1,4`、`orbis_mysticGate/bomb/15`、`cygnusReturns/demian/0`、`CrossBrigade/CherylAction/4`、`CrossBrigade/CrowAction/4`。**全部是 BOSS 战/剧情/传送特效**，只在特定事件触发，不会在静止城镇画面持续播放 3.5 秒 → Effect 层基本排除。

**结论（诚实边界）**：综合 ①②③④，** shineWood `back/16`（a0=235, blend=1, 100ms ramp）是视频闪烁的最合理源**，它在候选地图 101000000/100050000 里被 4 层引用、属于常驻背景、周期与视频吻合，Effect 层已排除。但因本轮无法用视觉工具确认视频画面到底是哪张地图（日志无时间戳、看图链路全挂），**不能 100% 断言就是这一处**——是"高置信度锁定 + 可复现机制链"，不是"眼见为实"。这也是官方原版设计特效，不是本地 bug。

## 7. 复现命令

```bash
python tools/_bg_alpha_scan.py
# 产物：docs/features/bg-flicker-scan/evidence/scan_result.json
#       docs/features/bg-flicker-scan/evidence/risk_summary.md
```
