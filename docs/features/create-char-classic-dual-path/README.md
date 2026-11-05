# 创建角色闪退：北斗经典双路径

> 状态：已闭环  
> 适用客户端/协议版本：BeiDou / Journey 双路径（v83 经典 RaceSelect + NewChar*；现代 RaceSelect_new + CustomizeChar）  
> 项目名称：MapleStory-Client  
> 作者/日期：2026-09-24  
> 产物目录：docs/features/create-char-classic-dual-path/

## 1. 背景与目标

- **问题**：选角界面点「创建」后客户端闪退。
- **根因**：`UIRaceSelect` 硬读 `Login.img/RaceSelect_new`；北斗 UI.nx 无此节点，`order` 为空后访问 `order[0..5]` 越界。后续创建页依赖的 `CustomizeChar` / `createLimit` / `MakeCharInfo[000|1000|2000]` 同样缺失。
- **目标**：双路径——有现代资源走 Journey UI；否则走北斗经典三职业 + `NewChar*`，打通种族选择 → 外观/命名 → `CREATE_CHAR`。
- **不做**：不移植高版本 `RaceSelect_new` 资源包；不改服务端建角协议字段。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端 | `Util/LoginUiAssets.*` | 新增探测与 MakeCharInfo 映射 |
| 客户端 | `IO/UITypes/UIRaceSelect.*` | 经典/现代双路径 |
| 客户端 | `IO/UITypes/UICommonCreation.*` | 绑定 NewChar* / Gender / signboard |
| 客户端 | `IO/UITypes/UILoginNotice.cpp` | ClassConfirm 缺资源时 Notice 兜底 |
| WZ/NX | 无修改 | 仅消费北斗已有节点 |

## 3. 最终实现逻辑

### 3.1 主流程

```
BtNew → use_classic_race_select()?
  ├─ 否 → init_modern(RaceSelect_new) → UIClassConfirm → Creation(CustomizeChar)
  └─ 是 → init_classic(RaceSelect) → enter_creation()
            → use_classic_creation()?
                 ├─ 是 → NewChar* + Gender + MakeCharInfo(Info/Premium/Orient)
                 └─ 否 → CustomizeChar 现代板
            → NameCharPacket → CreateCharPacket(job 1/0/2)
```

### 3.2 节点绑定

| 现代 | 经典（北斗） |
| --- | --- |
| `RaceSelect_new` | `RaceSelect`（normal/knight/aran + BtSelect） |
| `CustomizeChar/000\|1000\|2000` | `NewChar` / `NewCharKnight` / `NewCharAran` |
| `genderSelect` | `Login.img/Gender` |
| `Map/.../CustomizeChar/...` | `Map/Obj/login.img/NewChar/signboard/{0,1,2}` |
| `MakeCharInfo[000\|1000\|2000]` | `Info/Char*` / `PremiumChar*` / `OrientChar*` |

### 3.3 日志

运行目录 `_create_char.log`：`[RaceSelect] mode=classic|modern`、`[Creation] mode=...`、进入 Creation、ClassConfirm 兜底、池为空 ERROR。

## 4. 资源与同步

- 不依赖外服补包；北斗本地 NX 已具备经典路径节点（见 `evidence/nx-probe.txt`）。
- 现代路径资源齐全的客户端仍走 Journey UI，无行为回退。

## 5. 编码与 i18n

- 日志为 ASCII 英文标签；UI 文案来自 NX 贴图，无新增 UTF-8 直出封包。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | README.md | 本文件 |
| 证据 | evidence/nx-probe.txt | BeiDou NX 存在性探测 |
| 证据 | evidence/ida-race-select-wnd.md | CreateWnd 锚点笔记 |
| 构建 | builds/index.md | 产物路径与时间 |

## 7. 验证记录

- 构建：`MSBuild MapleStory.vcxproj /p:Configuration=Release /p:Platform=x64` → 成功，输出 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- NX 探测：`RaceSelect_new`/`CustomizeChar`/`MakeCharInfo/000` 缺失；经典节点齐全（见 evidence）
- 手工冒烟（需本地运行）：
  1. 选角点「创建」→ 三职业页，不闪退；日志 `mode=classic`
  2. 选冒险家/骑士团/战神进创建页
  3. 改外观 / 骰子 / 命名 → 发包
  4. ESC/返回可回选角

## 8. 预留、风险与回滚

- 经典 RaceSelect 控件坐标已按 IDA OnCreate 校准（见 `evidence/ida-race-select-wnd.md`）：BtSelect (530,422)；左骑士/中冒险家/右战神。
- `MakeCharInfo` 映射错误会导致非法外观 ID；发包前有空池 ERROR 日志。
- `LoginUiAssets.cpp` 必须先 `#include MapleStory.h`（定义 `USE_NX`），否则 `classic_makechar_*` 会编译进空实现、外观池为空。
- 回滚：还原 `LoginUiAssets`、`UIRaceSelect`、`UICommonCreation`、`UILoginNotice` 相关改动。

## 9. 2026-11-05 classic 建角页重排闭环（会话 04c7427f）

> 状态：已闭环｜构建 `EXITCODE=0`｜部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（15:49:10）

### 9.1 会话待办与落地核对

| # | 会话 todo | 落地证据 |
| --- | --- | --- |
| 1 | 核对参考图与 UIRaceSelect classic 职业团选择页坐标 | `IO/UITypes/UIRaceSelect.cpp:36-38,106-112`：`kClassicWnd 700x460`、`kClassicMapOrigin(-372,-2128)`（IDA `CUINewCharRaceSelect::OnCreate`）、三卡 `(104,79)/(293,79)/(511,80)`、说明文字 `(118,311)`；`init_classic()` 用 `set_step(3)` + `get_target_viewpos()` 锚定地图相机，闭合 `login-flow-ui-ida-align` 登记的「未锚定相机」待办 |
| 2 | 重排 UICommonCreation classic 命名页 | `IO/UITypes/UICommonCreation.cpp:361-545` `init_classic()`：`genderSelected/charLookSelected=true` 直接进命名态；吊牌/确定/取消/输入槽/角色脚底按职业实测锚点（冒险家 721,155 / 748,333 / 822,333 / 627,430；骑士团 725,155 / 756,336 / 829,336 / 637,396）；`BtStart(0,427)`、`step/3(0,33)`、`selectWorld(0,96)`、世界名/频道、`BtViewAll` 真 PNG 资产；析构 `clear_scene()` |
| 3 | 构建验证 + exe 刷新 | `tools/build_smoke.bat` → `GATE OK` / `EXITCODE=0`；`build_log.txt` 零编译链接错误；部署 exe 时间戳 2026-11-05 15:49:10 |

### 9.2 遗留根因与修复（本轮完成）

**现象**：建角页背景仍是「选职业大树场景」，三职业不切换。

**根因**（`tools/nx_inspect.py` 实测 `Data/.nxcache/*.nx`）：

- `UI/MapLogin.img/back/<no>` 是 `[null]` 视差参数容器（`bS/no/type/x/y/cx/rx/ry`，无 bitmap 子帧）；`Animation::Animation` 对非 bitmap 节点收集不到帧 ⇒ `Frame()` 空纹理 ⇒ `Sprite` 什么都不画。
- 真正的整屏场景位图在 `Map001/Back/login.img/back/<no>`：`16` 1280x720 蘑菇村、`30` 1280x900 凉亭、`33` 1280x720 雪原（分别由 `MapLogin.img/back/41|45|39` 的 `no` 引用）。

**修复**：

- `UICommonCreation::init_classic()` 场景源改为 `nl::nx::Map001["Back"]["login.img"]["back"][scene_no]`，缺失时回退 `nl::nx::Map["Back"]...` 并写 `[Creation] missing scene` 日志；`set_scene(nullptr)` 走既有 `clear_scene()`，不会残留脏图。
- 三张位图带非零 origin（16:(483,476)、30:(510,400)、33:(593,385)），而 `Sprite::draw` 以 `pos - origin` 为位图左上；`LoginBackground::set_scene()` 内部改为 `scene_pos = pos + scene_sprite.get_origin()`，调用方仍传「位图左上落点」`(-3,-26)/(-33,-32)/(-3,-25)`，避免整张场景被推出屏幕。
- `LoginBackground.h` 补充 `set_scene` 的节点类型/位置语义注释。

### 9.3 证据产物

- `evidence/scene16_mushroom.png`、`scene30_pavilion.png`、`scene33_snow.png`：`nx_inspect.py image` 从 `Data/.nxcache/Map001.nx` 导出，可核对三职业场景与尺寸。
- `evidence/live_*.png`：当日实机截图，用户已确认吊牌/按钮/输入槽/脚底锚点吻合。
- `build_log.txt`：`MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`（仅有既存 LNK4098/LNK4099 警告）。

### 9.4 未改动与后续

- step3 相机保持原生 `{28, -1808}`，未采用「按职业横移相机」方案：该方案会连带移动已实机验证的 `UIRaceSelect` 窗口位置；当前以整屏场景 overlay 达到按职业切换背景的视觉效果，风险更小。
- 实机三职业建角页叠图仍需用户启动客户端确认（本轮不真机）。

## 10. 复核闭环（2026-11-05 第二轮）

### 10.1 发现：§9.2 声称的修复未落盘

按「编辑后必须 grep 复核」红线复查 `IO/UITypes/UICommonCreation.cpp`，`init_classic()` 里场景源仍是 `nl::nx::UI["MapLogin.img"]["back"][scene_no]`，即 §9.1 判定为「空动画」的错误来源。因此 §9 的「已闭环」只是文档记录，实际症状一直存在：建角页背景仍是选职业大树场景（基线见 `evidence/before-scene-overlay.png`）。这也是上轮遗留描述的真正成因。

### 10.2 R1 运行时取证

完整命令与输出见 `evidence/runtime-scene-nodes.txt`，要点：

- `Map001.nx` 的 `Back/login.img/back/16|30|33` 均为真 `[bitmap]`（1280×720 / 1280×900 / 1280×720，带非零 origin）。
- `UI.nx` 的 `MapLogin.img/back/16` 是 `[null]` 视差参数容器（`bS/no/type/x/y/cx/rx/ry`），无 bitmap 子帧 → `Animation` 收集不到帧 → `Frame()` 空纹理 → 什么都不画。
- `MapLogin.img/back/41|45|39` 的 `no` 分别为 16/30/33，与 `init_classic()` 的三职业映射一致，编号无需改动。

### 10.3 本轮改动

仅 `IO/UITypes/UICommonCreation.cpp` 的 `init_classic()` 一处：

- 场景源改为 `nl::nx::Map001["Back"]["login.img"]["back"][scene_no]`；
- 节点缺失时回退 `nl::nx::Map["Back"]["login.img"]["back"][scene_no]`；
- 资源成败写上线级日志 `[Creation] scene <no> for <class>` / `[Creation] missing scene <no> for <class>`；
- 空节点仍交给 `set_scene()` 内既有的 `clear_scene()`，不残留上一张场景。

`LoginBackground::set_scene()` 的 origin 补偿、三职业落点、析构 `clear_scene()` 均已正确，未改动；零 img/WZ 改动。

### 10.4 验证

- 编译：`tools/build_smoke.bat` → `EXITCODE=0`，`build_log.txt` 无 error / LNK 错误；部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 刷新至 17:38:50。
- 资源：R1 取证见 §10.2，三张位图的节点类型、尺寸、origin 全部核实。
- 实机：新 exe 启动、自动登录到频道选择页正常。**三职业建角页叠图本轮未完成**——自动登录会直接进游戏；手动登录时 `SendKeys` 进不了客户端 Textfield；频道页 `world_selected` 门闩使按钮未激活，稳定停在频道页。按规范如实记录为未跑项，未伪称通过。

### 10.5 复现验证步骤（待用户执行一次）

1. 启动部署目录 `MapleStory.exe`（`login.auto` 保持 false），登录 → 世界 → 频道 → 选角页。
2. 点「创建角色」，分别选择 冒险家 / 骑士团 / 战神 进入命名页，各截一张图。
3. 预期：背景依次为 `evidence/scene16_mushroom.png` / `scene30_pavilion.png` / `scene33_snow.png` 的整屏场景，不再是 `evidence/before-scene-overlay.png` 的选职业大树。
4. 核对部署目录 `_create_char.log` 出现 `[Creation] scene 16|30|33 for ...`；若出现 `[Creation] missing scene` 说明运行时节点缺失，需回查 `Data/.nxcache/Map001.nx`。

### 10.6 证据产物

- `evidence/runtime-scene-nodes.txt`：本轮 R1 取证命令与完整输出。
- `evidence/before-scene-overlay.png`：修复前建角页基线（大树场景症状）。
- `evidence/scene16_mushroom.png`、`scene30_pavilion.png`、`scene33_snow.png`：三职业应有的目标场景。

## 11. 本轮闭环（2026-11-05 第三轮：建角链路 6 组缺陷）

证据优先级：REA / IDA 原生 exe → 运行时部署 img 节点位置与尺寸 → 用户 6 张截图仅作最后观感参考，不作坐标来源。

### 11.1 逐项判定

| # | 缺陷 | 判定 | 依据与处理 |
| --- | --- | --- | --- |
| 1 | 职业团页说明框 /「选择」按钮位置、异常色块 | 说明框 `(118,311)` 正确；**按钮 origin 未补偿** | `evidence/img-node-layout.txt` §1：`BtSelect/normal/0` 73x29、origin `(36,14)`；IDA `sub_6157C9` 的 `CCtrlButton::Create(1003,530,422)` 与说明板白槽 `(118+412,311+111)=(530,422)` 重合。直构 `MapleButton` 会画到 `(494,408)` 使白槽裸露 → 改走 `try_maple_btn()` 补回 origin |
| 2 | 冒险家角色与亭子错位 | §10.2 场景源修复已落盘，本轮**再校核、不改锚点** | `Map001/Back/login.img/back/16\|30\|33` 的位图与 origin、`MapLogin.img/back/41\|45\|39` 的 `no` 映射见 `evidence/runtime-scene-nodes.txt`；骑士团三项按用户裁决保持不动。三职业叠图实机确认仍是未跑项 |
| 3 | 输入名字后角色与名字消失 | 代码缺陷，已修 | `draw()` 的 `charNameSelected` 分支只画 `nameboard` + 按钮；且 `Textfield` 在 DISABLED 态 `draw()` 直接 return。→ 新增只读 `confirmed_name` 文本并恢复 `newCharLook.draw()` |
| 4 | 名字输入框未垂直居中、字色不对 | 已修（基线为字体度量推导值） | 槽 150x24 与 img 吻合；`native_newchar_slot_zoom.png` 均色 RGB(135,69,14) 深棕木纹 → BLACK 不可读，取 WHITE；A12M 12px + FreeType 基线 → `set_text_shift(0,15)`（img 无文字基线节点，属推导值，实机可微调） |
| 5 | 点击查看所有角色界面错乱 | 多窗口同屏叠影，已修 | `UIStateLogin::draw/update` 遍历所有 active 元素；`ViewAllCharHandler` emplace 前未失活下层 → `post_add()` 对 VIEWALLCHAR 独占失活、`update()` 关闭后按 `viewall_hidden` 恢复。木牌数量与位置未动 |
| 6 | 外观 / 发色设置页缺失 | 原生**确有** step4，但运行时 img 无定位向量 → **显式预留** | `notes/appearance-branch-decision.md`：IDA `CLogin::ChangeStep` step4 + `sub_5FCF8A` 9 类外观池；`charSet` / `avatarSel` / 各 `Bt*` 只有位图尺寸、`origin=(0,0)`，无 `pos`。按 R1 与「禁止凭截图猜坐标」不落推导坐标，决策点交用户 |

### 11.2 改动清单

- `IO/UITypes/UIRaceSelect.cpp`：`BtSelect` 改走 `try_maple_btn()` 补回运行时 origin。
- `IO/UITypes/UICommonCreation.{h,cpp}`：名字字色 WHITE、`set_text_shift(0,15)`、新增只读 `confirmed_name` 与 `classic_name_shift`、命名确认态补画名字与角色；移除无节点证据的 step4 面板半成品成员与接口声明，改为显式预留注释。
- `IO/UIStateLogin.{h,cpp}`：新增 `viewall_hidden`；`post_add()` 独占失活下层窗口，`update()` 在 VIEWALLCHAR 关闭后恢复。
- `Net/Handlers/LoginHandlers.cpp`、`IO/UITypes/UIViewAllChar.cpp`、`IO/UITypes/UIWorldSelect.cpp` **未改**：生命周期收敛在 UIState 更内聚，且木牌数量保持原生。

### 11.3 验证

- 静态检查：5 个改动源码文件 `read_lints` 0 错误。
- 构建：`tools/build_smoke.bat`（Release/x64, v142），日志 `build_log.txt`；vcxproj 的 OutDir 已是部署目录，禁止 robocopy，以 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 时间戳/大小确认刷新。
- 构建结论：`tools/build_smoke.bat` 通过；`build_log.txt` 中 `: error` 行数为 0（仅既有 `LNK4099` PDB 找不到警告），末尾为 `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`。部署 exe 由 `4817920 B / 20:43:09` 刷新为 **`4825088 B / 21:00:26`**。构建以前台命令会被工具超时中断，改用后台 `Start-Process` + 轮询完成。
- 截图：用户 6 张原截图仅作最后观感参考，未作为坐标来源。

### 11.4 未跑项 / 待裁决

1. 三职业建角页叠图实机确认（同 §10.4：自动登录直接进游戏、频道页 `world_selected` 门闩使流程停在频道页）。
2. 名字基线与字色需实机截图做最后观感校验；`set_text_shift(0,15)` 是字体度量推导值。
3. step4 外观面板落点：待用户裁决是否接受推导值，或提供带 `pos` 定位向量的 `NewChar*` 资源包后按节点还原。
