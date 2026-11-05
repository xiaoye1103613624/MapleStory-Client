# 登录流程五症状 · 运行时 IMG 属性与原生证据核对

日期：2026-11-04
口径：唯一真值 = 运行时部署 `E:\MXD\BeiDou-Client-xy\Data\UI\*.img`，结构 dump 为
`E:\MXD\BeiDou-Client-xy\REA反编译\wz_dump\*_runtime.json`（单行紧凑 JSON，用 python `json.load` 解析）。
打包 `Login_img.json` / `MapLogin1_img.json` 仅作版本差异对照，**不作准**（R1/R2）。
orange-wz MCP 对这批私服改版 img 内部节点有已知解析缺陷，未使用。
原生逻辑依据：`docs/features/login-flow-ui-ida-align/notes/`（r04、r08、ida-coords.md、
charselect-node-schema.md）与 `evidence/login-panel-offset.md`。

## 1. 运行时 IMG 节点属性（python 精确解析 `Login.img_runtime.json`）

| 节点 | 属性 |
|---|---|
| `WorldSelect/chBackgrn` | bitmap **449×233**，origin **(224,116)** |
| `WorldSelect/scroll/0` | 4 帧，**513×152** delay250 → **513×416** delay50 → **513×408** delay50 → **513×401**（末帧无 delay）；全部 origin (0,0) |
| `WorldSelect/scroll/1` | 反序收卷：513×401 delay150 → 416 d50 → 408 d50 → 152 |
| `WorldSelect/channel/chSelect` | 4 帧 **23×11** origin(12,-6) delay100 → **38×21** o(27,4) d50 → **42×33** o(29,16) d50 → **59×33** o(29,16) |
| `WorldNotice/BalloonForLoginProcess` | 九宫拼框：`Title` 170×18、`n` 12×40 o(0,40)、`ne` 22×40 o(0,40)、`e` 22×9、`c` 12×9 等；**无 184×74 单张 canvas** |
| `CharSelect/charInfo1` | bitmap **183×151**，origin **(45,57)** |
| `CharSelect/charInfo` | bitmap **183×115**，origin **(45,57)** |
| `CharSelect/scroll/2` | 4 帧 **217×30** d250 → **217×196** d50 → **217×195** d50 → **217×193**；origin (0,0) |
| `Login.img/Title` | 仅 `MSTitle` 397×219 + 7 个 `Bt*` + `check` + `effect`；**无 mapleID / PW / ID / backgrd 贴图** ⇒ `bedou=true`，输入框走 IDA `CUITitle::OnCreate` 兜底矩形 |
| `Obj/login.img/Title/signboard/0/0` | 木牌 **368×236** origin **(184,118)**（归档 `MapLogin_logo_metrics.txt`） |

版本漂移对照（**不采信**）：打包版 `Login_img.json` 的 `WorldSelect/scroll/0` 只有 1 帧 371×222、
`chSelect` 64×27 origin(0,0)、`scroll/2` 末帧 217×158、`charInfo1` 180×154 —— 与运行时全部不同。

## 2. 原生逻辑证据（已归档，未补跑 REA/IDA）

- `r04-step-scroll-tween.md`：`CLogin::GetStepHeight = -8-600*step`；
  `CLogin::ChangeStep@0x5F53C0` 时长 `v65 = 300*abs(new-old)+500` ms；
  补间由 `sub_43E7C7` 向 `IWzGr2D` 中心写命令；`sub_5F4C16` **滚动结束后**才 `OnStepChanged`；
  每次切步播放 `SP_1236_SCROLLUP`。**未记载任何 S 曲线缓动**。
- `r08-step-window-defer.md`：构造期用 `get_viewpos()` 会取到插值起点 ⇒ 端点 `get_target_viewpos()`；
  `:67-70` 明记「参考端会把旧窗口一起滚出，本实现直接隐藏旧窗口，属已知差异」。
- `evidence/login-panel-offset.md`：7 个按钮 NCC 互相关 → `TITLE_WINDOW_OFFSET=(58,16)`；
  `CUITitle::OnCreate` 输入框窗口内坐标 账号 **(67,12) 132×15** / 密码 **(67,40) 120×15**；
  木牌自带深色雕刻槽约 x105..255 / y30..48；`sign_topleft = map_pos + viewpos - origin`。

## 3. 五症状根因与最小修复点

| # | 症状 | 根因（证据） | 修复点 |
|---|---|---|---|
| 1 | 登录输入框内容相对木框错位 | `UILogin.cpp:143` 构造期用 `get_target_viewpos()` 固化所有控件坐标；木框由 `LoginBackground::draw()` 每帧按 `get_viewpos()`（补间插值）绘制。`ServerlistHandler` 收到首个 SERVERLIST 即 `emplace<UIWorldSelect>`（构造中 `set_step(1)` 启动 800ms 补间），而 `remove(LOGIN)` 只在结束包执行 ⇒ 补间期间旧 `UILogin` 仍 active、控件停在 step0 端点，木框/背景已滚走（`r08:67-70` 已知差异）。另 R28-B5 默认字体 SimHei→思源宋体，SimHei 时代 `textfield_pos += (2,1)` 与 `set_text_shift(2,0)` 的槽内微调不再适配。 | `UIStateLogin::post_add()` 补间开始时统一 deactivate 其它 active 步进窗口；`UILogin` 输入框去掉逐像素微调，回到 IDA 原生 (67,12)/(67,40) 槽内 |
| 2 | 登录→频道动画不流畅/慢 | `LoginBackground.cpp:75` 自写 S 曲线 `t²(3-2t)`（两端速度为 0），r04 原生为 `IWzGr2D` 中心线性补间；叠加旧窗口未按 r08 隐藏 → 观感拖沓/脱节。 | `current_offset()` 改线性；`tween_duration` 保持 `300*|Δ|+500` 不动 |
| 3 | 频道卷轴/列表/公告三动画 | 工作区 `UIWorldSelect.cpp:1236` 删除了原 `if (!LoginBackground::get().available())` 守卫，启用 `Login.img/WorldSelect/scroll/0` 全屏卷（513×401，与地图 `signboard/2` 卷筒重复）；且 `Animation::update()` 对循环动画不终止 → `scroll_playing` 永真 → 始终画整屏卷、不转 `chBackgrn`。`:1228` 把 `channel_slide` 直接置 `1.0f` ⇒ 频道列表滑入动画根本不播。公告 `notice_slide`/`panel_expand` 与全屏卷叠加形成双重运动。 | 恢复守卫（地图卷筒存在时不播整屏卷）；`channel_slide` 回到 `0.0f`；按运行时 IMG 帧序/delay（250/50/50、100/50/50）手动帧播放并定格末帧；公告/列表与面板揭示同步 |
| 4 | 选角后属性 UI 错位 | `UICharSelect.cpp:518-520`：`ci_pos = detail_pos + charinfo.get_origin()`（origin (45,57)）用于 `charinfo.draw()` 是**正确**的（Texture 落位 = pos − origin = detail_pos）；但同一 `ci_pos` 传给 **`ColorBox`**（`Geometry.cpp:66` 直接按 args.getpos() 画、无 origin 抵消）⇒ 米色底垫落在 `detail_pos+(45,57)`，与贴图/数值行错开、压住排行榜区。 | 底垫改画在 `detail_pos`；`CS_CHARINFO_OFF`/`CS_DETAIL_*` 与多分辨率锚点 `get_target_viewpos()` 不动 |
| 5 | 进图背景全白 | `GraphicsGL.cpp:1589` `glClearColor(1,1,1,1)` 是**白色**清屏兜底；`SetFieldHandlers.cpp:123` `unload_atlas()` 后首帧贴图在解锁后第一个 draw 才上传，`:173` `apply_game_viewsize()` 写 Constants 触发下一帧 `reinit(size_changed)→clearinternal()`，其延迟清理在 `clearscene()` 清掉已入队 quad/offsets ⇒ 首帧无任何背景 quad 时露出白底。`MapBackgrounds::drawbackgrounds()` 在 `backgrounds` 空且 `bS` 非空时不画底（`MapBackgrounds.cpp:196-203`）。 | 清屏色改中性黑兜底 + `drawbackgrounds()` 在空背景时强制画不透明底；`apply_game_viewsize()`/9 档/resize 守卫全部保留不动 |

## 4. 共享逻辑回归风险（本轮不得触碰）

- `get_target_viewpos()` 被 `UILogin.cpp:143`、`UIWorldSelect.cpp:242`、`UICharSelect.cpp:209,302`、
  `UIRaceSelect.cpp:91` 共用；改回 `get_viewpos()` 会重演 r08 半途坐标。
- `center = vieww/2, viewh/2`（`LoginBackground.h:68-71`）与 `ResolutionTiers.h:29-49`、
  `Window.cpp:1008-1024` 单向驱动、`GraphicsGL.cpp:537-576` 的 `size_changed` 守卫、
  `SetFieldHandlers.cpp:55-87` 只写 Constants —— 均为提交 `b4874e4` 多分辨率能力，**保留**。
- `LoginBackground::available()` 守卫网分布 4 个窗口；`get_origin()` 补偿 3 处（三源 origin 矛盾）必须保留。
- `r08` 延迟激活钩子 `UIStateLogin::post_add/activate_after_transition/pending_activation` 被 5 类步进窗口共用。
- `fonts/SimHei/simhei.ttf` 0 字节是 R28-B5 有意删除（换思源宋体，见 `Configuration.cpp` 的
  `stale_font_path` 兜底），**不是缺陷**，本轮不恢复。

## 5. 构建与验证

- `tools/build_smoke.bat`（MSBuild Release/x64，OutDir = `E:\MXD\BeiDou-Client-xy\`），`EXITCODE=0` 通过；
  构建前确认 exe 未被占用（占用时 LNK1104 但脚本仍报 0）。
- 正确客户端截图（`C:\Users\11036\Desktop\正确客户端截图\`：`登录界面.png`、`选择频道页.png`、
  `选中频道页.png`、`频道变更.png`、`选择角色页.png`、`进入游戏页.png`）仅作**最后**视觉校准。
