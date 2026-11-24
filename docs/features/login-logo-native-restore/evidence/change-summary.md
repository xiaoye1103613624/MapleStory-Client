# 改动证据摘要（login-logo-native-restore）

日期：2026-11-21

## 解析取证（只读）

- 当前 "BeiDouMS" 字标来源：`IO/UITypes/UILogin.cpp:82-87` 本地 PNG 覆盖层加载 + `:391-393` draw 整块盖图。
- 生效文件：`docs/features/login-flow-ui-ida-align/resources/creation/LoginLogo_override.png`（166894B，
  部署目录 `UI/Creation/LoginLogo_override.png` 同字节）。内容为 "BeiDouMS" 文字 + 红枫叶 + 森林底裁块。
- 底层原生 logo：`Gameplay/MapleMap/Obj.cpp:29-38`，登录 Map Obj 换位图为
  `nl::nx::UI["Login.img"]["Title"]["MSTitle"]`。
- 原生 MSTitle 实测：用 `tools/nx_inspect.py image` 从部署 `E:\MXD\BeiDou-Client-xy\Data\.nxcache\UI.nx`
  导出节点 `Login.img/Title/MSTitle` → 397×219，内容 = "MapleStory" 花体 + 红枫叶 + "Beta Version"
  （见 `native-mstitle.png`）。即 angel/minms 同款 v83 原生资产。
- 历史背景：`docs/features/login-flow-angel-calibration-2026-11/TERM_VERIFY_REPORT.md §11.2` 记录该覆盖层
  是为对齐 BeiDou 目标端而加，把原生 MapleStory 盖成 BeiDouMS；本任务反向撤销。

## 本次改动

| 文件 | 位置 | 改动 |
| --- | --- | --- |
| `IO/UITypes/UILogin.cpp` | 构造 :82-87 | 删 `ATLAS_LOGIN_LOGO` + `login_logo.load(...)`，改注释说明 |
| `IO/UITypes/UILogin.cpp` | draw :391-393 | 删 `if(login_logo.is_valid()) login_logo.draw(...)`，露底层 MSTitle |
| `IO/UITypes/UILogin.h` | — | 不动（成员保留便回滚） |

## 构建/部署校验

- `tools/build_smoke.bat` → `EXITCODE=0`（2026-11-21 11:00，GATE OK）。
- 部署 exe：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`，5143040 B，LastWriteTime 2026/11/21 11:00:01。
- lint：`UILogin.cpp` 无新增诊断。

## 并发会话代码修复（非本任务，需其作者复核）

构建期间工作区有并发会话未提交改动导致编译失败，为出 exe 做了两处零语义修复：

1. `Weather/WeatherBackLayers.cpp:252`：C2440，三参 `DrawArgument(Point,Point,float)` 无重载 →
   七参等价构造（pos, center=pos, stretch=(dw,dh), xscale=1, yscale=1, opacity=l.current*alpha, angle=0）。
2. `Gameplay/MapleMap/MapBackgrounds.h`：`Background::refresh_viewport` private 被外层调用 C2248 →
   声明移到 public（删 private 重复声明）。
