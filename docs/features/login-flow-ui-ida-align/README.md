# 登录链路 UI 对齐（对照 IDA：BeiDou gms083）

## 目标

以参考客户端（北斗 gms083）的 IDA 反编译产物为权威依据，修正 `MapleStory-Client` 登录链路
（登录 → 选世界 → 选频道 → 选角色 → 创建角色）各界面控件坐标，使其在 **1280x720** 下与原版版式一致。

## 结论（本轮）

**登录 → 选世界 → 选频道 → 选角色 全链路已打通并逐屏对齐复验通过。**

- 选世界 / 选频道：木牌锚 `signboard/0` 绳扣；频道面板在 `signboard/2` 底边下并下滑出现
- 选角色：三按钮锚 `CharSelect/signboard/0`；槽位上移到桥面；选中光脚底→身光
- 所有坐标均改为 `viewpos + 地图物体/常量偏移` 的锚点式，不再写死屏幕绝对像素

| 修正项 | 改前 | 改后 | 依据 |
|---|---|---|---|
| 世界槽位条纵向位置 | y=40（悬在横杆上方 70px） | y=110 | IDA 槽位 Y=0 相对条幅 + 实测横杆顶 y=111 |
| 频道面板 chBackgrn | 顶 y=74 | 顶 y=139 | 随条幅同步下移，保持既有相对间距 34 |
| 世界公告文本 | (58,-50)，压住频道按钮 | (58,-90)，落面板表头 | 实测与 CH.1 同行重叠 |
| 「到选择的世界去」判定区 | 落在频道面板矩形之外，点击会被当成“点面板外”→先去选世界→按钮被停用，**永远点不动** | 把该按钮判定区并入，不再误触发去选 | 实测点击后世界被取消、无法进入角色列表 |

世界槽位排布（步进 27、30 槽、提示图标 +5）经核对与 IDA 完全一致，未改动。

### 定位方式：改为**锚点相对偏移**（不再写死屏幕绝对像素）

登录地图层由 `LoginBackground` 以 `viewpos = center - (28, step_height(step))` 绘制，
因此所有登录界面控件都必须**相对地图相机**摆放，这已经是 `UILogin` 的既有做法
（面板锚在告示牌：`signboard_map_pos + viewpos - sign_origin`）。
本轮把 `UIWorldSelect` 的条幅 Y 从写死 `110` 改为 `viewpos.y + (-857)`，
并用 **1280x720 / 1024x768** 双分辨率实测验证：预测 111/135，实测横杆 111/135、槽位挂钩 112/136（±1px）。
若沿用写死值，1024x768 下会错位 25px。

> 参考客户端（IDA）同样是锚点式：`CWnd::CreateWnd / CDialog::CreateDlg / CFadeWnd::CreateFadeWnd`
> 末参均为 `W4UIOrigin@CWnd`（窗口原点枚举），`CWndMan::GetOrgWindow(UIOrigin)` 提供参照窗口，
> 而步进位移恒为 600（`GetStepHeight = -8-600*step`，与分辨率无关）。
> 换言之原版的「动态」是**锚点式 + 固定尺寸地图平移**，不是按分辨率缩放。

### 背景改用地图自带 `back` 层（修掉选角色页空白 + 天然消除锚点漂移）

**问题**：`LoginBackground` 原先把手搓的 `Map001/Back/login.img/back/11..13` 当固定尺寸图层，
用 `posy = (viewh - dim.y()) - scroll`（**底对齐，随 viewh 1:1**）绘制；
而地图物件层走 `viewpos`（**居中，随 viewh/2**）。两者锚法不同 → 换分辨率互相漂移；
且 step=2 时森林滚到 `viewh-8-1200 = -488`、物件相机到 `viewh/2+1208`，**两块都出图，只剩书页外框**。

**修复**：改用工程既有的 `MapBackgrounds`（与主地图同一套 `Stage` 用法
`backgrounds = MapBackgrounds(src["back"])`），数据源为登录地图自己的
`MapLogin.img/back`。它内含 `rx/ry` 视差、tiling、ani，视差基准是 `VWIDTH/2 / VHEIGHT/2`，
**与地图物件共用同一相机**，因此 step 与分辨率都自洽。手搓森林层保留为 `has_backs` 缺失时的兜底。

**复验**：
- step=2（选角色）：背景由空白变为正常的树屋/绳桥/木牌场景 → `evidence/charselect_bg_fixed.png`
- step=1（选世界）：UI 槽位挂钩实测仍为 `x 235-259, y 112-202`（与改前一致，**无回归**）；
  背景色数 48598 → 127583（back 层内容明显变多）→ `evidence/verified_worldselect.png`
- step=0（登录）：告示牌面板锚点不变

> 待确认：改用地图自带 back 层后，step=0/1 的**背景观感**与之前不同（step=1 由木牌签变为卷轴）。
> 由于本机没有可运行的原版 gms083 客户端做对照，请以原版截图确认哪种正确。

## 证据

- 坐标提取与实测依据：[`notes/ida-coords.md`](notes/ida-coords.md)
- IDA MCP 逐函数导出（已归档）：[`evidence/`](evidence/)（`CLogin::ChangeStep`、
  `CUIWorldSelect`/`CUIChannelSelect`/`CUICharSelect` 构造、`DrawWorldItems`、`GetStepHeight`、
  `CUITitle::OnCreate`、登录地图内嵌元素尺寸等）
- IDA MCP 端点：`http://127.0.0.1:13338/mcp`
- 复验截图：`E:\project\_cap_world3.png`、`_cap_channel4.png`（1280x720 实测）

## 新提取到的关键事实（本轮经 IDA MCP 补出）

- `CLogin::ChangeStep`(0x5F53C0)：共 **6 步进**（0 登录 / 1 选世界 / 2 频道·角色 / 3 新建角色·种族 /
  4 角色预览 / 5 其他），步进切换走 `GetStepHeight(cur)→GetStepHeight(next)` 补间 + `SCROLLUP`。
- 各窗口锚点：`CUIWorldSelect`(-140,-804,400,100) · `CUIChannelSelect`(136,51,540,167) ·
  `CUICharSelect`(200,-1359,**101x131**) · `sub_60E157`(0,125,125,352) ·
  `sub_592992`(275,100,250,314) · `sub_593A8D`(275,100,250,343)。
- 容器规模：15 个世界 / 60 个频道。

## 选角色界面（本轮补齐）

### 1. 打通 CHARLIST 解析（前置阻塞）

实测流程：登录 → 选世界 → 选频道 → 点「到选择的世界去」→ `CHARLIST_REQUEST(0x05)` 已发出，
服务端 `CHARLIST(0x0B)` 也已到达且包体完整，但客户端 `LoginParser::parse_charentry` 抛 `PacketError`，
界面停在「正在连接到服务器。」。

**根因**：本客户端 `parse_stats` 的字段宽度与服务端 `addCharStats` 不一致 ——
服务端 `hp/maxHp/mp/maxMp` 为 **int32**、`exp` 为 **int64**，而客户端按 **int16**×4 与 **int32** 读取，
整段错位后 `parse_look` 的等值槽位循环读不到 `0xFF` 终止符 → 越界抛错。
已按服务端定义修正字段宽度，解析通过后 `UICharSelect` 正常创建。

### 2. 结构差异与重映射策略

参考版 `CUICharSelect` 只是 **101x131 的小窗口**（承载三个按钮），整屏由多个对话框拼合；
本实现 `UICharSelect` 是 800x600 的单一 `UIElement` 且大量坐标取自 NX 节点，**结构不同**。
故**不照搬锚点**，而采用「**原点归一 + 锚点式偏移**」：

- 记录 NX 节点原始坐标 → 减去一个基准原点（`CS_FRAME_ORIGIN`），使帧内坐标与参考版同一尺度
- 渲染时 `position = viewpos + CS_FRAME_ORIGIN`，从而与地图相机同源、跨分辨率自洽

### 3. 槽位与按钮（IDA 权威字面量）

| 项 | IDA 来源 | 落地值 |
|---|---|---|
| 每行槽位数 / 横向步长 | `CUICharDetail ctor` `x = 130*(i%3) - 220` | 3 个 / **130** |
| 槽位纵向（占用 / 空） | 同 ctor `v8.y = 1384 / 1348`，`bScreenCoord=0`（**登录画布坐标系**） | 帧内 `CS_LIST_T + 176 / +140` |
| 槽位窗口尺寸 | 同 ctor `CreateFadeWnd(183, 占用151 / 空115, …)` | 行高取 **151** |
| 三按钮 | `sub_60148D` `id=1000/1001/1002, x=4, y=-2/36/85`（**101x131 窗口内**） | 帧内 `(204, -1361 / -1323 / -1274)` |

> 关键换算：`GetStepHeight(2) = -8-600*2 = -1208`，`1384-1208 = 176` → 屏幕 y = 482，
> **正落在实测桥面（≈470–505）上**。

复验截图：`evidence/charselect_slots_on_bridge.png`（槽位立于桥面）、
`evidence/charselect_buttons_visible.png`（三按钮可见）。

## 第二轮：收尾与加固

详见 [`notes/round2-hardening.md`](notes/round2-hardening.md)。摘要：

| 项 | 结论 |
|---|---|
| **世界/频道越界缺陷** | `buttons` 是 `std::map`：越界 key 会静默插入空项 → 误改频道按钮状态或**空指针崩溃**；`channel_gauge` 是定长数组，`channel_count > 30` 属**真实越界写**。已统一收敛到带校验的 `get_world_button` / `get_channel_button`，越界限频 WARN + 安全忽略 |
| **容量对齐参考版** | 按钮 id 段改为表达式推导（`BtChannel0 = BtWorld0 + WORLD_BUTTON_COUNT`），容量由 5 世界扩到**参考版的 15 世界 / 30 频道** |
| **槽位横向常量** | `+140` 穷尽 IDA 可达路径后无法反推画布原点，按「3 槽等距反证为刚性平移」固化为具名常量 `CS_CANVAS_TO_FRAME_X` |
| **槽位行距** | **被证伪**：参考版 `y` 与索引无关、`x` 用绝对索引 `%3`、`OnButtonClicked` 无翻页按钮 ⇒ 参考版是「固定 3 槽 + 翻页换角色」。据此 `PAGESIZE` 12→**3**、去掉臆造的行偏移、导航列数 4→3 |
| **仓库卫生** | 清理构建垃圾与重复证据、补 `.gitattributes` 与 `.gitignore`、构建脚本移入 `tools/` 并实跑验证 |

> **一处前提被证伪**：上一轮记录的「`includes/NoLifeNx/**` 纯行尾符噪声」不成立 ——
> 那些 `M` 多数只是 **stat 脏（mtime）**，真差异的 14 个文件是**真实功能开发**（新增 `wz_file` 支持）。
> 因此**未做** `git add --renormalize` 归一化提交。

## 第三轮：补间、索引与请求门闩（2026-10-21）

| 项 | 记录 | 结论 |
|---|---|---|
| 登录 6 步滚动补间 | [`notes/r04-step-scroll-tween.md`](notes/r04-step-scroll-tween.md) | `set_step` 按 `300*|new-old|+500` ms 补间；端点对齐 `GetStepHeight`，step 5 映射 step 1 |
| 选角索引安全 | [`notes/r05-charselect-index-guards.md`](notes/r05-charselect-index-guards.md) | `world` 初始化 + 默认角色夹取 + `BtSelect`/`BtDelete` 越界校验，消除 C4701 |
| 选角请求门闩 | [`notes/r06-charselect-request-latch.md`](notes/r06-charselect-request-latch.md) | `request_pending` 阻止重复选角/删除；失败回包复位 |
| 世界/频道请求门闩 | [`notes/r07-worldselect-request-latch.md`](notes/r07-worldselect-request-latch.md) | 以 `UILoginWait` 作为请求进行中标记，阻止重复发包 |
| 步进窗口延迟激活 | [`notes/r08-step-window-defer.md`](notes/r08-step-window-defer.md) | 新窗口在补间结束后激活；构造期改用 `get_target_viewpos()`，避免半途坐标 |

## 未完成 / 已知偏差（未臆改）

1. **`UIRaceSelect` 未锚定地图相机（潜在缺陷，待实测）**：
   `UIStateLogin::draw` 不施加全局相机偏移，而 `UIRaceSelect` 从不设置 `position`（保持 `(0,0)`），
   进入 step 3 时背景随相机滚动、UI 不动 ⇒ 两者会错开。
   修正量需 step 3 的实测落点，而进入创建角色界面超出本轮范围（避免触及写库流程），故登记为待办。
2. **创建角色界面版式不可照搬**：参考版 `CUINewCharRaceSelect` 为 700x460 @ canvas(−372,−2128)，
   但本实现 `UIRaceSelect` 全部坐标取自 NX 节点 `Login.img/RaceSelect_new`，
   与参考版 node 集/结构都不同，仅记录锚点备查。
3. **`CUICharDetailVAC` 归属未明**：其 `SetOption` 点为 `(148, -710)`、`CreateFadeWnd(170, 210, 0, z=11)`，
   读 `CLogin+316` 的另一角色数组；已排除「第 2 行槽位」的猜想，具体用途待查。
4. **按钮与地图挂链图元轻微重叠**：属 NX 版本资源差异，不影响可点击与可读性。
5. **旧窗口滚动**：参考端 `OnStepChanged` 在补间结束后才销毁/创建窗口，旧窗口会随背景滚出；当前端已在补间结束后才激活新窗口，但旧窗口是直接隐藏，没有 ghost 绘制。见 `r08` §4。
6. **step 0 相机 X**：参考端 `CLogin::Init` 使用 `(28, GetStepHeight(step))`，当前登录页沿用 `(137, -20)` 锚点对齐书本开口；需实机叠图确认是否为系统性偏移。见 `r04` §4.2。
7. **View All / View Choice**：`UIWorldSelect` 两个按钮仍是 stub，点击无行为；频道按钮固定 30 个，未实现参考端的 60 频道翻页。
8. **请求超时**：选角/世界请求门闩依赖服务器回包或 UI 销毁复位，未加超时；断线场景需 `Session` 重连时统一清理。

## 后续建议

- 若允许进入创建角色界面（**注意：完成建角会写库，仅打开界面不写**），实测 step 3 落点以修正第 1 项
- 补导出 `CUINewCharRaceSelect` 的 `OnCreate`、`CUICharDetail::Draw` 逐图元偏移、`dword_BEDA4C/BEDA50` 归属
