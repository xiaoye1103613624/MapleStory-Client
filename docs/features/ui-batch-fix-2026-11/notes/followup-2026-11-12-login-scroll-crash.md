# 续作取证 · 2026-11-12（登录白字 / 卷轴锚点 / 选角闪退）

## 1. 登录字色

用户明确：账号/密码改回 **白色**（覆盖本批次 JAMBALAYA）。无待决。

## 2. 频道卷轴 + 羊皮纸偏左上

### Angel / IDA（已证）

| 项 | EA | 常数 |
|----|-----|------|
| `CUIWorldSelect` ctor | `0x6246CE` | `CreateDlg(-140,-804,400,100)` |
| `CUIChannelSelect` ctor | `0x628501` | `SetOption(-176,-688)`；`CreateFadeWnd(448,253)`；背景 SP_1358 |
| 卷轴展开 | `0x629348` | 子层局部 **`(-38,-141)`**；SP_1364；音效 1239 |
| 卷轴收起 | `0x62955D` | 同锚 `(-38,-141)`；SP_1365 |

原生画布绝对锚：`(-176,-688)+(-38,-141)=(-214,-829)`（再加相机 viewpos）。
REA `catalog/WorldSelect.json` n=0；逻辑以 IDA 为准。

### 本端现状

- 架构合并进 `UIWorldSelect`，用 `scroll_open` + `channelsrc_pos`（相对 MapLogin `signboard/2` + 截图补丁）。
- `WS_SCROLL_MAP_POS(61,-771)` / `WS_POLE_MAP_POS(35,-896)` 相对 MapLogin 真值 `(42,-753)/(51,-888)` 有截图 Δ。
- `WS_SCROLL_ANIM_OFF(-302,-274)` 相对面板中心；相对原生面板中心估 `(-262,-257)` 约 **偏左上 40×17**，与用户观感一致。

### orange-wz

`Login.img` / `Map/Obj/login.img` 本轮 `IMG 解析失败`（记入回退）；地图真值沿用既有 `MapLogin.img_runtime.json`。

### 推荐

按用户「在背景卷轴位置开始动画」：去掉截图 Δ，**卷轴动画直接锚 `signboard/2` top-left**，`channelsrc_pos` 由卷筒底边推导；用 Angel `(-38,-141)` / 窗 `(-176,-688)` 做校验，不另开 FadeWnd。

## 3. 选角后闪退

| 时间 | 异常 | 模块偏移 |
|------|------|----------|
| 2026-11-12 09:00 / 07:39 / 01:16 | `0xc0000005` | `VCRUNTIME140.dll+0x1cca7` |

dump：`MapleStory.exe.6168.dmp` 等。与进图路径同类；需建库符号栈确认是否本批次 `UICharSelect` 改动或进图既有崩。

## 落地（同日 Plan 执行）

- 权威切换：`login-flow-angel-calibration-2026-11/notes/authority-switch-2026-11-12.md`
- 矩阵：`login-chain-matrix-2026-11-12.md`
- 登录字色 WHITE；频道/世界改 Angel `(-176,-688)` / `(-38,-141)` / `(-140,-804)`
- 选角闪退真因：进图后 `CharStats::get_jobname` → `Job::get_name()` **按值返回悬空引用**（`UIStatusBar::draw_classic:2415`）。已改为 `const std::string&` 返回成员 `name`。
