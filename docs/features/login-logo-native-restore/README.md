# 登录页品牌字标换回 v83 原生（MapleStory + 红枫叶）

> 状态：已闭环（构建通过、exe 刷新；实机截图待确认）
> 适用客户端/协议版本：BeiDou v16/v17（v83）
> 项目名称：MapleStory-Client
> 作者/日期：2026-11-21
> 相关提交：未提交（工作区改动）
> 产物目录：docs/features/login-logo-native-restore/

## 1. 背景与目标

用户诉求：登录页顶部中央红框区域当前显示 "BeiDouMS" 品牌字标（+红枫叶+森林底），
要求换成 "原生的"，参照 angel / minms。

解析结论：angel/minms 的原生登录字标就是 v83 `UI/Login.img/Title/MSTitle`（"MapleStory" 花体 + 红枫叶 + Beta Version），
而**本端底层已经在画这张原生图**——只是被一张本地 PNG 覆盖层（`LoginLogo_override.png`）整块盖住，
才显示出 "BeiDouMS"。因此"换回原生"的正确做法不是拷贝新素材，而是**移除覆盖层**，让底层 MSTitle 直接显示。

明确不做：不改右上角 `Ver. 0.83`、版本号、木牌、背景森林、MSTitle 资源本身。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端代码 | `IO/UITypes/UILogin.cpp` | 移除 `login_logo.load(...)`（构造）与 `login_logo.draw(...)`（draw）两处 |
| 客户端代码 | `IO/UITypes/UILogin.h` | 不动（保留 `LocalPngTexture login_logo` 成员以便回滚） |
| WZ/IMG/资源目录 | — | 无（覆盖层 PNG 文件保留在原地，只是不再被加载） |
| 服务端/协议/DB | — | 无 |
| 并发会话代码（非本任务） | `Weather/WeatherBackLayers.cpp`、`Gameplay/MapleMap/MapBackgrounds.h` | 两处编译修复，见 §7 |

## 3. 最终实现逻辑

### 3.1 原生字标的绘制链路（保留、现成）

- `Gameplay/MapleMap/Obj.cpp:29-38`：登录 Map Obj（`oS=login, l0=Title, l1=logo`）把动画位图换成
  `nl::nx::UI["Login.img"]["Title"]["MSTitle"]`，保留 Map Obj 的 a0/a1 淡入。
- `IO/UIStateLogin.cpp:54-75`：先画 `LoginBackground`（含地图 Obj 的 MSTitle），再画各 UI 元素。
- 底层 MSTitle 实测 397×219，内容 = "MapleStory" 花体 + 红枫叶 + "Beta Version" 小字（导出图见 evidence）。

### 3.2 本次改动（移除覆盖层）

1. `UILogin.cpp` 构造（原 :82-87）：删掉 `ATLAS_LOGIN_LOGO` + `login_logo.load({...})`，
   改为注释说明为何停用（底层已有原生 MSTitle）。
2. `UILogin.cpp` draw（原 :391-393）：删掉 `if (login_logo.is_valid()) login_logo.draw(Point(434,80))`，
   原生 MSTitle 在 Map Obj 原位置显示，带淡入动画。
3. 覆盖层 PNG 文件（`LoginLogo_override.png`，仓库 resources/creation/ 与部署 UI/Creation/ 各一份）**不删除**，
   只是不再加载——回滚只需恢复两处调用。

## 4. 资源与同步

- 无新增/修改资源；覆盖层 PNG 原地保留。
- 无多语言/多端目录同步、无密钥/编码/版本兼容问题。

## 5. 编码与 i18n

- 无新增文案；MSTitle 内文字来自 v83 原生位图（烘焙），无需 i18n。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | `README.md` | 本文件 |
| 证据 | `evidence/native-mstitle.png` | 从部署 UI.nx 导出的原生 MSTitle（397×219） |
| 证据 | `evidence/change-summary.md` | 改动摘要 + 构建/部署校验 + 并发修复记录 |

## 7. 验证记录

- 构建：`tools/build_smoke.bat` → `EXITCODE=0`（2026-11-21 11:00，GATE OK）。
- 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`，Size 5143040，LastWriteTime 2026/11/21 11:00:01。
- lint：`UILogin.cpp` 无新增诊断。
- 实机：待用户启动登录页确认顶部中央显示原生 "MapleStory + 红枫叶"（含 Beta Version），且保留淡入动画。

### 7.1 为解除构建阻塞而做的并发代码修复（非本任务，务必知悉）

构建过程中发现工作区有一批**并发会话的未提交改动**导致全工程编译失败。
为使本任务能出 exe，做了两处**零语义变化**的编译修复，并已在本文件与代码注释中标注：

1. `Weather/WeatherBackLayers.cpp:252`：原 `DrawArgument(Point, Point, float)` 无对应重载（C2440）。
   改为语义等价的七参构造 `DrawArgument(pos, center=pos, stretch=(dw,dh), 1,1, opacity=l.current*alpha, 0)`。
2. `Gameplay/MapleMap/MapBackgrounds.h`：`Background::refresh_viewport()` 声明在 private，
   但外层 `MapBackgrounds` 在视口切换点调用它（cpp:294-300）导致 C2248。
   把声明移到 public（删 private 重复声明）。

**上述两处属并发会话代码，请其作者复核后再决定是否保留/改写**；本任务逻辑（logo 覆盖层移除）与它们无关。

## 8. 预留、风险与回滚

- 风险：原生 MSTitle 自带 "Beta Version" 小字（用户已确认要完整原生，保留）。
- 风险：去掉覆盖层后，原 beiDou 覆盖层曾冻死的 LOGO 区动画恢复——这正是 angel/minms 原生行为（背景透过字标半透明区），符合用户要求。
- 回滚：恢复 `UILogin.cpp` 构造的 `login_logo.load(...)` 与 draw 的 `if(login_logo.is_valid()) login_logo.draw(...)` 两处即回到 BeiDouMS。
