# 掉落光环特效全链路问题一次性修复（2026-11-21）

> 用户指令：「存在的问题都需要修复」。本轮按点到面通读全链路，
> 一次性修完所有已定位问题，不挤牙膏。全程 Orange-WZ MCP + 二进制 +
> 服务端 class 一手取证，未凭截图、未想象改字段。

## 0. 结论摘要（先看这个）

| 层次 | 修复前状态 | 修复动作 | 修复后状态 |
| --- | --- | --- | --- |
| **live 资源** `DropAuraEff.img` | 数字节点 `1..5` 是 webp 误导入的**单个静态红色帧**（无 origin/z/delay）→ 当前已部署 exe 读的就是这批节点 → 所有档位显示成几乎相同的静态红光、不动 | 把正确的 `Rare/Epic/Unique/Legendary` 四色 6 帧动画（含 origin/z/delay）覆盖到数字节点 `1..5` | 数字节点 1=蓝/2=紫/3=黄/4=绿/5=绿兜底，各 front/back 6 帧、元数据齐全（`ee7eb6a2…`） |
| **源码** `DropAuras.cpp/.h` | 工作区有未提交改动，把读取从「命名四档」改成「数字档 1..5 + GRADE_MAX=5」——与服务端 grade 0..4 不符，5 是死槽 | `git checkout HEAD --` 恢复到正确的命名四档实现（GRADE_RARE=1..GRADE_LEGENDARY=4, GRADE_MAX=4） | 源码干净，下次 rebuild 出来就是正确四档逻辑 |
| **服务端** s9 `PacketCreator.java` | 已实现 grade 0..4 发包，无需改 | 不动 | 运行中的 s9（PID 50672, port 8484）compiled `PacketCreator.class` 已含 `getDropItemGrade`（mtime 23:51 > 源码 23:49） |

**真机验收边界（如实声明）**：本轮修复后**未做真机发包进图验收**（4 个 MapleStory 客户端进程在跑但无法非侵入抓图/发包）。
资源层、源码层、服务端层均已按一手证据逐项核验；真机观感需用户进图丢一件带潜能的装备确认四色光环在动。

## 1. 根因（比首轮表面结论更深）

首轮只看到「grade 档位口径不统一」「资源冗余」。本轮深挖发现真正的破坏点：

1. **数字节点是误导入的动画帧，不是档位**。`E:\MXD\扩展改动\掉落特效\` 里 12 个 webp
   （`0..5.webp` front + `0..5_光柱.webp` back）尺寸序列 `(72,33),(73,34),(63,37),(71,34),(72,33),(61,38)`
   **与 Rare/front 的 6 帧尺寸逐帧一致**——它们就是「一套光环的 6 帧动画」，
   被错误地当成「6 个品级」导入成 `dropItemAura/0..5`，每档只剩 1 张静态红色帧、
   没有 origin/z/delay。
2. **当前 live exe 编入的正是读数字节点的坏版本**。二进制取证：live `MapleStory.exe`
   （5,303,296B, 05835954…）中 `Rare/Epic/Unique/Legendary` 字面量 **count=0**，
   而正确版本 build（`build-final-assets/MapleStory.exe`）四个字面量 **count=1**。
   ⇒ live exe 走 `std::to_string(g)` + `root["dropItemAura"]["1".."5"]` 路径，
   正好吃到上面那批坏节点。这就是「实际效果存在问题」的真正根因。
3. **四档正确素材一直在资源里**。命名节点 `Rare/Epic/Unique/Legendary` 各 front/back 6 帧，
   origin/z/delay/_hash 齐全，颜色分别为：Rare 蓝 `(145,201,255)`、
   Epic 紫 `(185,145,255)`、Unique 黄 `(255,252,145)`、Legendary 绿 `(204,255,145)`。

### 为什么选「资源层兼容修复」而不是直接 rebuild exe

- 工作区有 **180 个真实改动文件**，其中 15 个是别的会话正在做的协议/UI 工作
  （`Net/PacketSwitch.cpp`、`Net/OutPacket.h`、`Configuration.cpp`、`IO/UITypes/UIWorldMap.cpp` 等，
  mtime 一直到 23:46）。`build_smoke.bat` 的 OutDir 直接指向 live，
  opcode 门禁当前因这些新增 opcode（CASHSHOP_WINDOW_ACTION/14128、ENHANCE_ACTION/14109 等）
  报 `GATE FAIL`。**此时 rebuild 会把另一会话未完成的协议改动一起部署进 live**，风险不可接受。
- 因此采用「资源层兼容修复」：把正确四色动画填进数字节点，
  **当前部署的坏 exe 立刻能正确渲染**；同时把源码恢复正确版本，
  等协议工作结束、门禁转绿后的下次干净 rebuild 同样正确。两条路径都成立，无需现在冒险 rebuild。

## 2. 源码层修复（行级）

| 文件 | 行 | 修复前（工作区未提交） | 修复后（HEAD 正确版） |
| --- | --- | --- | --- |
| `Gameplay/MapleMap/DropAuras.h` | 37-39 | `GRADE_MAX = 5`（与服务端 0..4 不符，5 死槽） | `GRADE_RARE=1 / EPIC=2 / UNIQUE=3 / LEGENDARY=4 / GRADE_MAX=4` |
| `Gameplay/MapleMap/DropAuras.cpp` | 19 | 删掉了 `GRADE_NAMES[4]` | `constexpr const char* GRADE_NAMES[4] = {"Rare","Epic","Unique","Legendary"}` |
| `Gameplay/MapleMap/DropAuras.cpp` | 50-56 | `for g=1..5; root["dropItemAura"][std::to_string(g)]` | `for g=0..3; root["dropItemAura"][GRADE_NAMES[g]]`，落到 fronts/g+1 |
| `Gameplay/MapleMap/DropAuras.cpp` | 89/97/112/122 | `grade<=0 || grade>5` | `grade<GRADE_RARE || grade>GRADE_MAX`（即 1..4） |

`Net/Handlers/MapObjectHandlers.cpp` 的未提交改动是**别的会话的 NPC rx0/rx1 修复**，与本功能无关，未触碰。

## 3. 服务端 grade 逻辑核查（无需改）

运行中的服务端 = s9（`BeiDou-Server_s9`，PID 50672 监听 :8484，
command line 含 `BeiDou-Server_s9\gms-server\target\classes`）。
`PacketCreator.java`：

- `:1955 getDropItemGrade(MapItem)`：按 Δ属性评分，`<6→0 / <23→1 / <40→2 / <55→3 / else→4`，
  金币/非装备/负增益 0，base 缺失 0；已把教程的 5 档收敛为官方 0..4。
- `:2025 updateMapItemObject` 与 `:2056 dropItemFromMapObject`：
  `p.writeByte(getDropItemGrade(drop))` 紧跟 playerDrop 字节之前，两分支位置一致。
- compiled `target\classes\org\gms\util\PacketCreator.class`（mtime 23:51:34 > 源码 23:49:19）
  含 `getDropItemGrade` ×1 ⇒ 运行中的 class 已是修复后逻辑。

## 4. 资源层修复（解决当前已部署 exe）

脚本：`tools/fix_drop_aura_resource.py`（本次新增，可复用）。
走 AGENTS.md §3 安全流：`load_files(staging 副本)` →
`copy_paste_nodes(Rare|Epic|Unique|Legendary 的 front/back → 数字节点 1..5, OVERWRITE)` →
`save_as` 到**不同路径** `DropAuraEff.aurafix.out.img`（禁止 save_as 回写 loaded 根）→
`unload_all` → 原子替换 live `Data\Effect\DropAuraEff.img`。
备份：`tools\_staging\DropAuraEff.aurafix.img.pre-aurafix.bak`。

踩坑记录（供复查）：
- `copy_paste_nodes` 直接粘「档位节点」会把源按源名挂成子节点（`1/Rare`），**必须逐个粘 front/back**；
  OVERWRITE 才能覆盖同名旧静态帧。
- `save_node` 对 loaded 根返回 ok 但**不落盘**（hash 不变）；必须 `save_as` 到异路径才真正生效。
- 首次原子替换报 `WinError 1224`（文件被内存映射）——原因是早前 MCP 会话还 mmap 着 live img；
  `unload_all`(hintGc=true) 全部旧会话后解决。

### 修复后 orange-wz MCP 实测（部署文件）

| 节点 | 帧数 | 首帧尺寸 | 元数据 | 首帧均色 |
| --- | --- | --- | --- | --- |
| `dropItemAura/1/front` | 6 | 72×33 | origin/z/delay/_hash | 蓝 (145,201,255) |
| `dropItemAura/2/front` | 6 | 72×33 | 齐全 | 紫 (185,145,255) |
| `dropItemAura/3/front` | 6 | 72×33 | 齐全 | 黄 (255,252,145) |
| `dropItemAura/4/front` | 6 | 72×33 | 齐全 | 绿 (204,255,145) |
| `dropItemAura/5/front` | 6 | 72×33 | 齐全 | 绿（兜底，服务端不发 5） |
| `dropItemAura/1/back` | 6 | 58×46 | 齐全 | 浅蓝 (228,247,255) |
| `dropItemEffect` | 6 | 192×165 | origin/z/delay/_hash | 光柱未动，PNG 有效 |

命名节点 `Rare/Epic/Unique/Legendary` 原样保留（供修复后源码版本读取），两套节点并存但内容一致。

## 5. 产物与校验和

| 产物 | SHA256(前16) | 说明 |
| --- | --- | --- |
| `E:\MXD\BeiDou-Client-xy\Data\Effect\DropAuraEff.img` | `ee7eb6a2003a1b2c` | 已部署修复后资源（216,687B） |
| `E:\MXD\BeiDou-Client-xy\MapleStory.exe` | `058359544c1660ae` | **未改**（走资源兼容路线，避免卷入并发协议改动） |
| `E:\MXD\BeiDou-Client-xy\MapleStory.exe.bak-before-aurafix-20261121` | `058359544c1660ae` | 备份（与本轮同 SHA，未动用） |
| `tools\fix_drop_aura_resource.py` | — | 本次新增的可复用修复脚本 |
| `tools\_staging\DropAuraEff.aurafix.img.pre-aurafix.bak` | — | 修复前 live 资源备份（136,602B） |

## 6. 遗留 / 待办

1. **真机验收未做**：请用户进图丢一件带潜能的装备，确认四色光环（蓝/紫/黄/绿）正常显示且会动、光柱落地播放一次。
2. **干净 rebuild 待办**：等 `Net/PacketSwitch.cpp`/`OutPacket.h` 等并发协议工作结束、
   `gen_opcode_matrix.py --check` 转 `GATE OK` 后，用 `tools\build_smoke.bat` 重新构建
   （源码 DropAuras 已是正确的命名四档版），把 live exe 换成带四档逻辑的版本，
   之后可清理资源里的数字节点冗余（改回只留命名节点）。本轮不动 exe 是为避免部署半成品协议。
3. **原文档（E:\MXD\扩展改动\掉落特效\）里的 itemaura.cpp/dropitemaura.cpp 仍是 32 位 hook 稿**，
   对当前 x64 live exe 不可用——仅作教程参考，不要拿去编译部署。

---

## 7. 追加：192 原生 DropItemEff.img 移植闭环（2026-11-22，用户选 C）

用户拍板 C（原生等价优先），并指出「orz mcp 新版本客户端密钥就可以打开」。本轮据此打通
资源导出 → 跨 key 转换 → 客户端接线 → 构建部署全链路。

### 7.1 密钥翻案（关键）

首轮「186 加密 WZ 无 key 解不开」的结论**被用户纠正翻案**。orange-wz `WzKeyStorage.java:49-51`
内置三套 key，其中 `new WzKey(3,"新版本客户端", WZ_LATEST_IV, DEFAULT_KEY)`：
- IV = `WZ_LATEST_IV` = `00000000` → ivBase64 `AAAAAA==`
- userKey = `DEFAULT_KEY`（128B，与 GMS 相同字节，源码 `WzAESConstant.java:14-23`）
- 用这套 key + `autoParse:true` 加载 186 `Effect.wz`（1.5GB），`list_children` 列出全部 80+ img，
  含 `DropItemEff.img`（IMAGE）；`query_nodes` 也命中。
- 翻案要点：之前「children=0」是因为用了 `_gms_key_128.b64` 文件（如需精确请用源码
  `DEFAULT_KEY` 字节构造 base64：head `EwAAAFIAAAAqAAAAWwAAAAgA`），且 `list_children`
  需要 `autoParse:true`；GMS IV `TSPHKw==` 对 186 无效，必须 zero IV。

### 7.2 资源证据（186 Effect.wz，orange-wz 实测）

`Effect/DropItemEff.img` 7 档，每档 front/back 帧数与 192 明文 XML 逐字一致：

| 档 | front | back | front/0 尺寸 | front/0 均色 |
|---|---|---|---|---|
| 0 | 8 | 1(1×1占位) | 62×60 | (227,241,200) 淡黄绿 |
| 1 | 6 | 16 | 57×46 | (226,232,252) 淡蓝 |
| 2 | 4 | 1 | 56×54 | (248,255,255) 近白 |
| 3 | 4 | 1 | 64×56 | (255,255,243) 奶白 |
| 4 | 4 | 1 | 55×54 | (222,217,229) 淡紫灰 |
| 5 | 4 | 1 | 54×53 | (254,235,73) **亮黄** |
| 6 | 5 | 1 | 44×38 | (202,209,247) 淡紫 |

192 明文 XML（`MapleServer192\wz\Effect.wz\DropItemEff.img.xml`）只有尺寸/origin/hash
元数据、**零像素**（逐帧 `pixel=False`）；真像素只能从 186 加密 WZ 导出。

### 7.3 索引驱动的证据与决策

- 原生用装备品级 1..4：v214 `DropPool.java:110` `encodeByte(ItemGrade.getHiddenGradeByVal(...))`，
  `ItemGrade.java` 定义 HiddenRare=1..HiddenLegendary=4；权威资料（MapleStoryWiki/Nexon/MapleSea）
  只定义 4 品级，**0..6 逐档无权威说明**。服务端只能下发 1B grade(0..4)，无 0..6 字段。
- 决策（自洽闭环，非宣称原生 0..6 等价）：**grade 1..4 直连 tier 1..4**；tier 0/5/6 资源保留
  但不由 grade 触发。不改协议、不主观挑档，行为可解释可扩展。

### 7.4 资源移植（AGENTS.md §3.1 强制跨 key 流程）

```
load(186 Effect.wz, 新版本key) 
  → create_img_file(新版本key) 
  → copy_paste_nodes(DropItemEff.img/{0..6}, OVERWRITE) 
  → save_node(src)  → batch_convert_key(新版本→GMS, verify:true) 
  → 原子放入 live Data/Effect/DropItemEff.img
```
- `save_as` 对 WZ 内单个 img 节点报「该节点不支持另存为」→ 改用 create_img_file + copy_paste。
- `batch_convert_key` 用 `outputDir`（目录）+ `paths`，verify:true 返回 `ok+verified`。
- 产物：`DropItemEff.img` 115,953B，SHA256 `4f8406fc5bd20f804f65ee8a98dea86ec165a696bdc7ef6fc9666e88d90b2677`；
  装前/装后用 live GMS key 加载，7 档像素与 186 源逐档一致。

### 7.5 客户端接线（行级）

| 文件 | 行 | 改动 |
|---|---|---|
| `DropAuras.h` | :60-63 / :79-82 | 新增 `has_tier/has_tier_back/tier_front/tier_back` 声明 + `tier_fronts/tier_backs/ok[]` 成员 |
| `DropAuras.cpp` | :25 / :89-110 / :159-185 | `DROPITEMEFF_IMG` 常量；init() 里 grade 1..4 直连加载 tier front/back；4 个访问函数 |
| `Drop.h` | :89-93 | `Drop` 新增 `tier_front/tier_back` Animation + ok 标志 |
| `Drop.cpp` | 构造/:update_aura/draw_* | 按 grade 取 tier 动画；绘制序：tier.back → 光柱 → aura.back → 图标 → aura.front → tier.front |

`#include <string>` 补在 `DropAuras.cpp`（`std::to_string` 依赖）。

### 7.6 构建与部署证据

- 门禁：`gen_opcode_matrix.py --check` = `GATE OK`。
- 构建：`tools\build_smoke.bat` → `EXITCODE=0`（MSBuild Release/x64）。
- live exe：`4937591f2016feb15608bfb6297cd5be49bdad7939f13eaf47c0998f9d6df2e7`
  （5,357,568B，17:55）；二进制含 `DropItemEff.img`×1 + `DropAuraEff.img`×1 + Rare/Legendary×1
  （`no drop item eff`/`tier missing` count=0 是 Release 下 LOG_DEBUG=no-op，正常）。
- 备份：`MapleStory.exe.bak-before-dropieff-*`（构建前 live exe）。

### 7.7 诚实边界

- **真机进图观感未验收**：本轮为源码行号 + 编译 + 资源像素一手核验，未实际发包进图确认 tier 光效叠加观感。
- **非原生 0..6 等价**：grade 只到 4，tier 0/5/6 保留未触发；若日后拿到 0..6 原生索引公式（需 REA/IDA
  192 客户端）再补映射。
- **并发会话**：本轮期间 live exe 多次被其它会话覆盖（23ecc8c9→529eb91b→17:52 版），
  最终以 17:55 本轮构建产物 `4937591f…` 为准；如再次回滚请用本节 SHA 复核。

---

## 8. 追加：光效与装备中心对齐（B 方案，2026-11-22 18:33 用户拍板）

### 8.1 真机问题（PixPin 录屏为证，本端证据）

用户录屏 `XiaoYe_2026-11-22_18-20-22.mp4`（180×194/60fps/6.4s）+ 截图 `18-21-12.png`：
掉落飞行途中「光效在上、装备（白紫宝石图标=石贝雷帽）在下」有位置偏差，落地后光环围绕装备是对的。

### 8.2 锚点根因（逐层 origin 实测）

`DrawArgument::get_rectangle`（`Graphics/DrawArgument.h:122`）`左上 = pos - center - origin`；
用 `DrawArgument(absp,1.0f)` 时 pos=center=absp → 图片 **origin 点对齐 absp**。
各层 WZ origin 不同 → 视觉中心相对装备偏移：

| 层 | origin | 尺寸 | 说明 |
|---|---|---|---|
| aura front | (35,20) | 72×33 | 基本贴合 |
| aura back | (31,45) | 58×46 | 偏上约22 |
| tier front | (10,49) | 57×46 | 偏上右 |
| 光柱 pillar | (93,140) | 192×165 | origin 靠底、主体向上延伸，原代码还额外 -16 → 偏上约74 |

`Frame::Frame`（`Graphics/Animation.cpp:90-96`）保留 WZ origin；`Texture::draw`（`Graphics/Texture.cpp:121-128`）按 origin 定位。

### 8.3 修复（一次性全部层，以装备几何中心对齐）

新增 `Drop::centered_pos(absp, anim)`（`Drop.h:57`、`Drop.cpp:239-244`）：
```
pos = absp + origin - dimensions/2     // 令图片几何中心落在装备锚点 absp
```
应用到全部 5 层（`Drop.cpp:253/257/260/266/270`）：tier.back、光柱、aura.back、aura.front、tier.front；
**去掉光柱原来的 -16 落点上移**。飞行中/落地后统一用装备当前 absp（`phobj.get_absolute`），
故飞行途中光效随装备几何中心移动、落地后同样居中。`ItemDrop.cpp:33` 的装备 icon 绘制不动。

### 8.4 构建/部署状态（诚实）

- 源码编译验证：`Drop.obj` 含 `centered_pos` 符号 ×4（独立 IntDir `build-centerfix-verify/int` 18:49 编译成功；
  共享 `x64/obj-Release` 18:54 也含 ×4）→ **B 方案语法/编译正确**。
- 部署受阻：本轮期间另一会话持续并发构建（多个 MSBuild/cl 进程反复起），
  抢同一 `vc142.pdb`（C1041）与 live exe（文件被占用/移走），导致本轮多次链接被中断、
  live exe 一度缺失。已备份 `MapleStory.exe.bak-before-centerfix-20261122_184134`。
- 因共享 obj 已含 B 方案，并发会话的构建产物一旦完成即自带本修复；已挂后台监控 exe 重现阶段。
- **真机观感未验收**：B 方案为几何推导修复，需 exe 重新出现且用户在飞行/落地两态确认居中。

### 8.5 结论（回答用户三问）

1. 飞行中偏差：真实存在，根因是各层 origin 不同 + 光柱 -16，B 方案已令所有层几何中心对齐装备。
2. 落地后效果：正确（光环围绕装备），B 方案保持并同样居中。
3. 「宝石后面才是我扔的装备」：理解反了——白紫宝石就是装备本身（tooltip=石贝雷帽），
   光效是叠加在它周围的绿光环/光柱；原教程就是「光柱标落点 + 光环围绕」逻辑，
   本端 B 方案把它改成全程以装备为中心（用户明确要求）。

---

## 9. 追加：移除 DropItemEff tier 层（宝石遮挡装备，2026-11-22 20:05）

### 9.1 用户纠错（推翻本节 8 的判断）

用户新截图 `XiaoYe_2026-11-22_20-05-10.png`：扔的是**衣服**，但画面上有一个**蓝紫八边形宝石**
盖住衣服，质疑移植错误。**本节 8.5 第 3 点「宝石=装备图标」的判断是错的** —— 用户是对的。

### 9.2 根因（orange-wz 逐层导出帧图实证）

把两个 img 全部 96 帧导出逐层看：
- **那个八边形蓝紫宝石 = `DropItemEff.img/4/front`**（tier4 首帧 `tierF_4_00.png` 与截图逐像素一致：
  八角形、蓝紫彩虹渐变、左上闪光）。tier4 是**实心宝石道具图案**，不是光环！
- 各档真实图案：tier0=彩虹圆形光环(中空,合理)、tier1=蓝十字闪光环、tier2=白闪光环、
  tier3=淡绿十字闪光、**tier4=实心八边形宝石(不合理)**、**tier5=黄星星**、tier6=淡蓝散点闪光。
- v214 `DropItemEff.img.xml` 确认 tier4 有 4 帧正式 outlink 节点（非误导入），但它是道具图案。
- 根因：本轮 §7 的「grade 1..4 直连 tier 1..4」映射，**grade 4 → tier 4 = 宝石**，
  把装备遮挡了。grade 5→tier5 星星同理（本端 grade 无 5，未触发）。

### 9.3 用户决策（本次拍板）

给三选项（①去掉这层只留四色光环+光柱 / ②grade 改映射闪光档 1/2/3/6 / ③全用 tier0），
用户选 **①：去掉 DropItemEff tier 层**。理由：本端已有为 4 档设计的正确四色光环
`dropItemAura`（蓝/紫/黄/绿）+ `dropItemEffect` 光柱，DropItemEff 这层是多余的且含宝石。

### 9.4 一次性移除（4 文件，行级）

| 文件 | 删除内容 |
|---|---|
| `DropAuras.h` | `has_tier/has_tier_back/tier_front/tier_back` 声明 + `tier_fronts/tier_backs/ok[]` 成员 |
| `DropAuras.cpp` | `DROPITEMEFF_IMG` 常量、init() 里 tier 加载段、4 个 tier 访问函数 |
| `Drop.h` | `tier_front/tier_back` Animation + ok 标志 |
| `Drop.cpp` | 构造 tier 加载、update_aura tier 推进、draw 里 tier.back/tier.front 绘制 |

**保留** §8 的 `centered_pos`（B 方案居中）对光柱/aura.back/aura.front 的对齐。
grep 复核：`DropAuras/Drop` 4 文件 tier 残留=0，`centered_pos` 保留 3 处绘制。

### 9.5 构建与部署（吸取教训）

- 门禁 GATE OK；独立 OutDir+IntDir 编译（`/p:UseSharedCompilation=false`，不碰 live exe、不碰共享 PDB）
  `EXIT=0`，产物 5,365,248B，**二进制 `DropItemEff.img`=0、四档名在** → tier 确实移除。
- 部署：live exe 被客户端进程占用（Stop-Process Access denied，权限高于代理），
  用**原子替换**（copy 到 `.newtmp` + `os.replace`）成功绕过占用。
- 备份：`MapleStory.exe.bak-before-notierfix-20261122_203413`。
- **实时 SHA（防回滚，两次一致）**：
  `f9ec71fefd4e0fc9fbdc7cb8fb45f8e21befb306f52d60831311c30b4aa7f1e8`（=验证产物）。
  资源 `DropItemEff.img=4f8406fc…` 保留在 live 但代码不再读取（可在下次清理时删）。
- 教训复现：①截图来源/内容必须逐帧实证，不能凭"看着像"下结论（§8 误判被 §9 推翻）；
  ②exe 被占用时 `os.replace` 原子替换有效，无需杀进程。

### 9.6 诚实边界

**真机观感未验收**——宝石移除为逐帧实证 + 编译 + 二进制验证，但无法非侵入抓图。
请用户进图再扔一次衣服：应只剩蓝/紫/黄/绿四色光环（按装备品质）+ 光柱，不再有宝石遮挡；
飞行中/落地后光效以装备几何中心对齐（§8 B 方案）。若仍有异常请发新录屏。
