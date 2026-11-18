# 创建角色第二页全链路修复 · 交付与交接报告

- 日期：2026-11-17
- 需求：全面深度修复创建角色第二页（UI / 名字设置与校验 / 确认后 UI 切换 / 皮肤肤色 / 初始装备切换 / 发色 / 性别），对照 angel、MinMS、img、服务端。
- 追加追问（2026-11-17）：第一页职业团选择页龙神/双刀是否有专属卡图 → 已查证：无专属图，复用战神/冒险家卡面 + 文字标签区分（详见 §4.2）。
- 结论：三处实质缺陷已修复并完成双端编译验证；其余链路经四端核对确认自洽、无需改动；真机端到端发包验收因环境阻塞未完成，本文末附证据化交接。

## 一、四端取证基准

| 端 | 取证对象 | 关键结论 |
| --- | --- | --- |
| 服务端 | `E:/project/BeiDou-Server_s9`（分支 feature/integration_client，活跃开发态） | CreateCharHandler / MakeCharInfoValidator / ItemConstants / Character.canCreateChar |
| MinMS | `E:/project/MinMS` | 单页两态建角，cjob=1 硬编码；仅名字 codepoint 计数可借鉴 |
| Angel | 13339 带符号 IDB + 13337 无符号 notes | 原生仅 3 套命名页 ctor（Aran/Normal/Cygnus） |
| img | `Etc.nx MakeCharInfo.img`（orange-wz MCP 不可用，回退 `tools/nx_inspect.py`） | 每池 node0脸/1发/2发色/3肤色/4上衣/5裤/6鞋/7武器 |

> orange-wz MCP 在本会话不可用（`ops_discover_tools` 查无该能力），按 AGENTS.md §2 回退 `.nx` dump + `tools/nx_inspect.py`。

## 二、三处实际修复（均已入链）

### 2.1 服务端 P0：龙神（EVAN/job=3）建角必失败
- 文件：`E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/client/creator/MakeCharInfoValidator.java:33`
- 根因：`getMakeCharInfo` switch 无 EVAN 分支 → `default -> null` → `isNewCharacterValid` 返回 false → CharacterFactory 返回 -2 → CreateCharHandler 发 `deleteCharResponse(0,9)`。
- 修复：`case EVAN -> character.isMale() ? charMale : charFemale;`（EVAN 脸/发/肤/装白名单与冒险家一致；verifyCharacter 对非 BEGINNER/NOBLESSE/LEGEND 不校验装备）。

### 2.2 服务端 P1：EVAN/双刀初始武器白名单与客户端精简包错配
- 文件：`E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/constants/inventory/ItemConstants.java:319-333`
- 根因：`isNewCharDefaultWeapon` 的 job3={1372005,1382000,1302000}、job4={1332000,1332001,1302000}；客户端精简 WZ 池为 {1302000,1322005,1312004}，交集仅 1302000。双刀出生 Job.BEGINNER 还存在"validator 走 charMale 池 / CreateCharHandler 走 job4 池"双重校验死锁。
- 修复：job3/job4 各并入 `|| weaponId == 1322005 || weaponId == 1312004`。纯扩展不收缩，同时解开双刀死锁。

### 2.3 客户端 P1：名字长度校验口径失真
- 文件：`IO/UITypes/UICommonCreation.cpp`
  - `:69` 新增 `size_t utf8_codepoint_count(const std::string& s)`（按 UTF-8 码点计数，非法首字节按 1）。
  - `:1419` `size_t nameLen = utf8_codepoint_count(name);`
  - `:1424/:1428` 由原字节数 `name.size()>=4` 改为 `nameLen>=2 && nameLen<=12`。
- 对齐服务端 `Character.java:1025` 正则 `[a-zA-Z0-9\u4e00-\u9fa5]{2,12}`。原实现会误拒 2~3 字符 ASCII 名、放宽中文名门槛。名字框 Textfield 上限已是 12 码点（Textfield.cpp:462）。

## 三、经核对确认无需改动的项

| 项 | 证据 | 结论 |
| --- | --- | --- |
| 发色语义 | 客户端 `hair=(hairStyle/10)*10+color=绝对值-hair`，服务端 hair+color 还原并按 base/color 拆验 | 自洽 |
| 性别切换索引 | randomize_look 重算全部 index，next_or_zero 防空池 | 自洽 |
| 肤色池不分性别 | Etc.nx 实测各池 node[3] 男女均为 {0,1,2,3} | 结果正确，仅健壮性问题 |
| 披风不进包 | 客户端仅预览、服务端不读 | 双方一致 |
| 字段顺序/宽度/job 映射 | 客户端 10×write_int+1×write_byte 与服务端逐位对齐；adventurer=1/aran=2/cygnus=0/evan=3/dual=4 与 s9 switch 一致 | 对齐 |
| 命名查重→外观页→点确定发包串行 | appearance-branch-decision.md §2（来自用户缺陷+截图） | 本端产品口径，非 bug |

## 四、P2 视觉模板决策：不改（资源约束）

### 4.1 第二页外观模板

`class_map` 有 5 项（000/1000/2000/2001/0004），但锚点/场景/label_pos/getXxxPos 只有 adventurer / cygnusKnight / else(=aran) 三套，evan/dualBlade 落战神模板。Angel 原生仅 3 套命名页 ctor、运行时 NewChar/NewCharKnight/NewCharAran 同构；坐标 (323,150) 为低置信度推导值。属体验一致性而非硬 bug，强行改需 40+ 处且无专属资源，风险大于收益，**保留现状**。

### 4.2 第一页职业团选择卡图：龙神/双刀无专属图，复用战神/冒险家（已坐实）

用户追问第一页职业团选择页龙神/双刀是否有专属卡图。结论：**没有专属卡图**，当前实际渲染的就是战神（aran）/冒险家（normal）的职业卡位图，仅靠文字标签区分。证据链：

- 代码回落逻辑 `IO/UITypes/UIRaceSelect.cpp:114-121`：卡节点 `nodes[] = {knight, normal, aran, RaceSelect["evan"]||["Evan"]||aran, RaceSelect["dual"]||["dualBlade"]||normal}`；按钮图 `btn_names[] = {BtKnight, BtNormal, BtAran, BtAran, BtNormal}`（龙神取 BtAran、双刀取 BtNormal）。
- 资源实证（2026-11-17，live `E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx`，`tools/nx_inspect.py list ... 1 Login.img/RaceSelect`）：`RaceSelect` 直接子节点只有 `BtSelect / aran / aran1 / knight / normal / textGL` 六个，**无 evan / Evan / dual / dualBlade 任何节点**。故 `:118-119` 的所有专属分支均落空，必定走 `:139` 回落。
- 三张卡图尺寸各异、确为不同美术：`aran/BtAran/normal/0` 181×222、`normal/BtNormal/normal/0` 223×221、`knight/BtKnight/normal/0` 209×219。已导出存档 `evidence/card_aran.png`、`evidence/card_normal.png`、`evidence/card_knight.png`。
- 原生基线印证：`docs/features/create-char-classic-dual-path/evidence/ida-race-select-wnd.md:21` 原生仅骑士团/冒险家/战神三卡，顺序 BtKnight(104,79)/BtNormal(293,79)/BtAran(511,80)；`evidence/nx-probe.txt`、`evidence/img-node-layout.txt:10` 同样只记录三职业卡节点。龙神/双刀系本端向后追加的职业（`:56`），资源包未随附专属卡图。

**若要补专属卡图**：属于新增美术资产，不在本次"修复既有 bug"范围。需先由美术/Photoshop 出两张职业卡（尺寸对齐 181×222 量级、含 normal/mouseOver/OnAnimation/text 四槽），落入 `Login.img/RaceSelect/evan` 与 `.../dual` 节点（走 orange-wz MCP 改写，客户端关闭→staging→save_as→原子替换），代码 `:114-121` 的专属分支会自动生效、无需改 C++。此项为可选增强，非缺陷，需用户确认后另立需求。

### 4.3 「为什么用替代图」根因澄清（2026-11-17 追问）

用户问：是否因为高版本端没有可同步的资源，才用当前客户端资源替代？**因果需掰正**：

- 本端存在两套互斥 UI 体系（README.md:4/12/44）：经典 `RaceSelect`（v83 三卡 normal/knight/aran）+ `NewChar*`；现代 Journey `RaceSelect_new` + `CustomizeChar`（支持龙神/双刀等后加职业）。
- 本端走经典路径是**被迫且历史的**：北斗 `UI.nx` 缺 `RaceSelect_new`/`CustomizeChar`（`evidence/nx-probe.txt:2,4`），这正是 README.md:12 记录的"点创建闪退"根因；README.md:14 明确决策"**不移植高版本 RaceSelect_new 资源包；不改服务端建角协议字段**"。
- 经典 `RaceSelect` 资源是 v83 时代产物，原生只有三职业卡（`ida-race-select-wnd.md:21`）；龙神/双刀系本端向后追加（`UIRaceSelect.cpp:56,60-61`）。**不是高版本端没图，而是本端选中的这套经典资源从设计上就没为它们做卡图**——专属图只存在于另一套（未被 adopt 的）现代 Journey 体系。
- 故"用战神/冒险家替代"是「把高版本职业塞进经典三卡页」的必然妥协，不是缺陷。真正"同步高版本资源"= 移植整套 `RaceSelect_new` + `CustomizeChar` + 改建角协议字段，成本/风险远超收益，历史已否。
- 诚实披露：高版本端资源为 `UI.wz` 格式（如 `E:/MXD/【2021最新】095PLUS2_10.1国庆版/.../UI.wz`），本会话 `tools/nx_inspect.py` 只认 `.nx` 且 orange-wz MCP 工具不可用，未能逐像素实证其中龙神/双刀选角图；"专属图在现代体系"为依据职业体系与双路径设计的合理推断。但即便存在，同步路径仍须连整套现代 UI + 协议一起搬，即历史否掉的方案。
- 轻量补图（§4.2 出的方案 A）之所以可行的前提，正是**绕开整套移植、只补两张卡图到经典节点**，让 `:114-121` 已有专属分支生效。

### 4.4 「高版本资源拆分移植，只拿需要的」可行性评估（2026-11-17 追问）

用户问：能否把高版本资源拆分、只移植龙神/双刀卡图。**概念上可行，但当前前置被卡住**，分层结论：

- **架构层可行**：卡图是独立位图资源（normal/mouseOver/OnAnimation/text 四槽），与整套 `RaceSelect_new` 逻辑无代码耦合；经典 `UIRaceSelect.cpp:114-121` 已预留 `RaceSelect["evan"]/["dual"]` 专属分支，理论上只塞两张图即可生效，无需动 C++、无需改服务端协议。这是"拆分移植"相对整套移植的核心价值。
- **取图前置被卡（实证）**：本会话通过 HTTP 直调 orange-wz MCP（`http://127.0.0.1:10012/mcp`，握手 HTTP 200，server v1.157.49），尝试从两处高版本端取图：
  - `E:/MXD/BeiDou-ClientV17.7/Data/UI/Login.img`（7.2 MB，高版本 img）
  - `E:/MXD/【2021最新】095PLUS2_10.1国庆版/V095冒险岛PLUS2客户端/UI.wz`
  - 结果：`load_files` 均返回 `rootCount:1` 看似加载成功，但 `list_children` 恒为 `{"children":[]}`、`search_node` 返回"找不到该节点"。穷举 orange-wz 内置全部 key/IV 组合（`WzAESConstant.java`：GMS `4D23C72B` / CMS `B97D63E9` / LATEST `00000000` × 128B `DEFAULT_KEY` / 65536B `key_gms`）共 6 种，全部无法展开节点。
  - 定性：**v83 的 orange-wz key 打不开高版本资源**，高版本 img/wz 用了不同的 AES key/加密方案。要取图须先解决高版本解密（找到对应版本 key 或换支持该版本的工具如 WzComparerR2 / 专用导出器）。此项为硬前置，未解决前"拆分移植"停留在概念层。
- **即便取到图的移植代价（可控）**：取到后按 §4.2 方案落地——Photoshop 裁剪对齐 181×222 量级四槽 → orange-wz `copy_paste_nodes`/`mutate_nodes` 写入 live/UI.nx 的 `Login.img/RaceSelect/evan`、`/dual` → 客户端关闭→staging→`save_as` 异路径→原子替换回 live（AGENTS.md §3 安全流）→ `unload_all` 收口 → 构建验证 → 真机截图验收（本会话无图像判读能力，需换环境）。
- **决策建议**：拆分移植是"补两张图"的最优路径（比整套移植轻得多），但**优先级低于代码修复与真机发包验收**。建议排序：① 先补真机发包验收（已有专项 blocker）→ ② 若确认要补卡图，先解决高版本资源解密/导出（用户可提供 WzComparerR2 导出的 PNG，或指定带图源）→ ③ 再走 §4.2 落地。此项非缺陷，属可选增强，需用户确认资源来源后另立需求。

## 五之二、追加核查：UI 状态联动三问（2026-11-17 用户 m00802）

> ⚠️ **证据定性更正（2026-11-17 用户 m00862 当面纠正）**：用户提供的三职业命名页截图（冒险家 `f19df937`/骑士团 `15120397`/战神 `7eba6ab3`）是**正确客户端（参考端）**的截图，**不是当前客户端（本端修复后）的截图**。故下述截图只能证明"目标效果长这样"，**不能作为本端修复已生效的验收证据**。本节三问的"结论"均为**源码逻辑层推导**，本端真机渲染状态**未经验证**（本会话无图像判读能力 + 环境 blocker §6）。此前措辞"截图证实本端联动正确"属证据错配，作废。

用户要求核查三问。逐条结论（均为源码逻辑层，非本端实测）：

### 5.2.1 二页取消返回一页是否串页 → 不串页，干净重建

- 二页命名态「取消」`UICommonCreation.cpp:1484-1503`（classic_mode 且非 look_stage）：`reset_creation_scene()`（`:59-63` 清场景）→ `remove(CLASSCREATION)` → **全新 `emplace<UIRaceSelect>()`**，一页 UI 实例是新建而非复用，不会带入二页控件状态。
- `reset_creation_scene()` 特意定义为不依赖 this 的自由函数（`:55-63`）：因 `UIStateLogin::remove()` 用 `unique_ptr::release()` 放弃所有权、不触发析构，析构里的清理是死代码；四条离开路径（BtNo / BtPreview / BtStart / 建角成功 onok）统一显式调用该函数。此坑已修。
- 二页外观(step4)态「取消」是另一条：`:1489-1492` → `leave_classic_look_stage()`（`:1180-1196` 复位 classic_look_stage/creation_sent/charName 等 + disable_char_look + set_classic_yes_no(2)），只退回命名态、不回一页，属设计如此。
- 旁证：二页 BtStart「回到首页」`:1339-1361`、BtPreview `:1362-1371` 同样 reset+remove+重建；BtStart 额外 `worldselect->remove_selected()` 复位历史缺陷 m00176（点过世界后回首页误落频道页）。

### 5.2.2 左侧木牌按钮逻辑 → 分派正确，均带状态复位

- 一页左侧四块（`UIRaceSelect.cpp:162-204`）：标题牌 `rs_step`/世界框 `rs_world_box` 为纯装饰 Sprite；两枚按钮——BtPreview「查看所有角色」(局部 y=377)、BtStart「回到首页」(y=427)。
- classic_mode 分派（`UIRaceSelect.cpp:745-758`）：BtPreview → `ViewAllCharPacket().dispatch()`（与 UIWorldSelect 同口径）；BtStart → `show_worldselect()`。
- `show_worldselect()` `:940-956`：`set_step(1)` + `worldselect->remove_selected()` 复位残留态 + remove RACESELECT/CHARSELECT + activate WORLDSELECT；`show_charselect()` `:928-938`：`set_step(2)` + remove RACESELECT + activate CHARSELECT。二者均先复位再切换，无残留态。
- 二页同名按钮（`UICommonCreation.cpp:1333-1361`）：BtViewAll → `ViewAllCharPacket`；BtStart → 世界列表态（含 m00176 复位）。一页/二页口径一致。
- 结论：木牌与截图从上到下（创建角色/选择的世界/查看所有角色/回到首页）一一对应，逻辑无缺陷。

### 5.2.3 场景切换时 UI 是否完整联动 → 源码层配套切换，本端实测未验证

- 一页选职业 `select_classic_class()`（`UIRaceSelect.cpp:727-743`）只改 selected_index/selected_class + 音效 + mouseover，**不切场景**（一页保持「选职业大树」，符合原生）。
- 场景切换发生在二页派生类构造：`UICommonCreation.cpp:569-608` 按 className 选 `scene_no`——adventurer→16(蘑菇村,-3,-26)、cygnusKnight→30(凉亭,-33,-32)、else(aran/evan/dual)→33(雪原,-3,-25)，`set_scene` 加载真位图（`Map001/Back/login.img/back/<no>`），空节点走 `set_scene` 内 clear_scene 不残留。命名牌 UI 随各派生类构造配套加载。
- 参考端三张截图显示冒险家/骑士团/战神三套「场景 + 命名牌 UI」各不相同——这是**目标效果**（非本端成果）。本端能否达到该效果，源码逻辑上一致，但**本端真机未实测**（见本节开头证据定性更正）。
- 已知边界（P2，见 §4.1）：evan/dualBlade 落 else→scene_no=33，复用战神雪原场景 + 战神命名牌模板，属资源约束，非本次缺陷。

### 5.2.4 「二页功能/UI 是否全修完」+ 装扮页 → 逐项定性

- 命名页：源码层三职业 UI 分派已实现，本轮名字 2-12 码点校验覆盖其输入校验 ✓（源码态）；**参考端截图证明目标效果，本端渲染未实测**。
- 装扮页（命名后 step4 外观页）：UI 已实现（外观池/avatarSel/dice/皮肤肤色/发色/性别切换），历史三职业逐页 NCC 验收 2026-11-16 19:28 通过并归档 `evidence/term-verify-r8/`；本轮三处代码修复（EVAN 建角 / 武器白名单 / 名字长度）覆盖其发包关键链路。
- **唯一未闭环**：真机端到端「点确定发包建角」验收（见 §6，环境 blocker 0xC0000374 + 本会话无图像判读能力）。截图 UI 正常 ≠ 功能全链路修完，此项如实披露，不把视觉验收冒充发包验收。

## 五、编译验证证据（闭环）

> ✅ **交付状态更新（2026-11-17 16:22 重新部署完成）**：live 产物 `E:/MXD/BeiDou-Client-xy/MapleStory.exe` 在被回滚为 `095b8e97…`（5235712 B）后，已用 `verify_out/MapleStory.exe`（SHA `0c3bf5db…`）经 Bash `cp`+`mv` 原子替换回 live。**实时核对 live SHA = `0c3bf5db75ac9661cf5ff58b226b01e4913e5784cc8b8dab88441805cb886649`，T+30s 复查未回滚**。被替换的 `095b8e97` 已备份为 `MapleStory_pre_fix_20261117.exe` 留证。当前 live **确为修复版**。但"当前客户端问题全修完"仍不成立——真机发包未验收（见第六节），本次复测客户端仍崩溃（见下方复测记录）。

- 客户端：MSBuild 2019，强制删除 `UICommonCreation.obj` 后重编，RC=0 / error C=0；`verify_out/MapleStory.exe` SHA `2204312a3640…` → `0c3bf5db75ac9661cf5ff58b226b01e4913e5784cc8b8dab88441805cb886649`；obj mtime > cpp mtime。
- 服务端：`mvn -o -pl gms-server compile -DskipTests -q` RC=0；class 时间戳 > 源码；`javap -c -p MakeCharInfoValidator.class` 的 getMakeCharInfo 为 tableswitch 1-9，case7→charMale/charFemale，确认 EVAN 分支入 class。
- 部署（16:22 重新）：`verify_out` 修复版经 Bash `cp`+`mv` 原子替换回 live（SHA `0c3bf5db`，T+30s 复查未回滚）。PowerShell `Move-Item` 路径两次因新落盘文件句柄锁定失败，已弃用（非任务失败，属工具文件 API 怪问题），改 Bash 路径一次成功。
- 服务端：主进程 PID 15708（13:37:54 启动）晚于 EVAN 修复 class 编译时间（07:15），已加载含修复 classes；8484 正常 LISTENING（PID 15708）。无需重启。

**本次复测记录（16:23，重新部署后）**：服务端 8484 LISTENING 正常；客户端经 explorer 代理启动（`tools/_watch_launch.ps1`），进程 t=2s 出现、线程/句柄逐步涨到 40/900，**t=14s 退出（仍崩）**。当前 live SHA 在复测后仍为 `0c3bf5db`（未被回滚）。结论：重新部署动作成功坐实，但启动崩溃 blocker 依旧存在——与三向对照中"VS2019 主 canon 线当日崩溃、非本次修复引入"结论一致，真机端到端发包仍无法在本环境验收。

## 六、真机端到端发包验收：阻塞与证据化交接（未执行）

本环节**未完成**，如实记录阻塞与已排除项：

1. **环境崩溃（非本次引入）**——A/B/A 三向对照：
   - 0c3bf5db（本次修复版）：约 7s 崩，`0xC0000374` 堆损坏；
   - 2204312a（本次改动前基线）：约 7s 崩，`0xC0000374`；
   - f4a23b5e（VS18 隔离线昨天成功产物）：今日持续存活 >13s。
   - 结论：崩溃是 VS2019 主 canon 构建线在这台机器上的当日问题，与本次三处修复无关。WER 显示故障模块为 MapleStory.exe 自身（ExceptionOffset 0x61b6）。
2. **模型无图像输入能力**——`read_image` 报模型不支持图像输入、`modlens_read_image` 无 vision provider，无法判读截图 UI。
3. **VS18 隔离线无法直接补位**——用其编译主线源码失败：当前主线工作树被他人并发改到中间态（`Gameplay/Stage.cpp` 缺 `Stage::get_char_entry`/`has_char_entry` 定义，`Net/Handlers/SetFieldHandlers.cpp` 仍在调用），属工作树自身不一致，与本次修复无关。

历史参考：三职业第二页 UI 逐页 NCC 验收已于 2026-11-16 19:28 通过并归档于 `docs/features/create-char-classic-dual-path/evidence/term-verify-r8/`（源 exe f4a23b5e），但当时明确"未点确定发包建角"——本次拟补的正是该空白。

## 七、剩余风险与建议

- **待补验**：真机端到端发包建角（重点 EVAN、双刀两职业）。建议在有稳定可运行 exe 且具备图像判读能力的环境，用 `tools/level999/` 与 `C:/Users/11036/AppData/Local/Temp/term_verify/scripts/` 下的 harness 补跑；验收信号为服务端 `deleteCharResponse` 不再出现 + 数据库 `characters` 表新增对应职业角色。
- **测试账号**：admin/admin、admn/admin、xiaoye/xiaoye、admin1/admin、tostest1/admin、admin2/admin（已自查解锁）。
- **注意**：服务端 s9 exp 保持 4 字节 `read_int`，不得改回 8 字节；装备槽枚举值必须等于官方 BodyPart 绝对值；MSBuild 增量陷阱（改源码须 touch + 删 obj）；live 目录可能被登录器回滚，部署后需复查 SHA。
