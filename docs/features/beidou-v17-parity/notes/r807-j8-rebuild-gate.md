# r807 · J8：全量重建门禁 + 登录崩溃回归 + 构建期陷阱复记

对应任务 #75（J8）。日期：2026-11-01。

## 1. 为什么必须全量重建

J7 给 `AttackResult` 加了一个成员：

```cpp
// Gameplay/Combat/Attack.h
bool toleft = false;
Point<int16_t> origin;   // NEW：毒炸弹的起爆点，出站封包要用
```

`Attack.h` 被战斗/角色一系编译单元广泛 include ⇒ **类布局变了**。
MSBuild 增量构建不跟踪这种改动，实测脏 obj：

```
find x64/obj-Release -name '*.obj' ! -newer Gameplay/Combat/Attack.h | wc -l
→ 200 / 274
```

即 200 个 `.obj` 仍按旧布局编译，链接出的 exe 会 ODR 错乱 —— 和 R26 那次
「登录进图 100% 崩」同一条坑（见项目记忆 `msbuild-header-layout-stale-objs`）。
构建门禁当时是绿的（0 error），完全发现不了。

## 2. 做法

```
python tools/gen_opcode_matrix.py                       # 先发号，否则 GATE STALE
MSBuild.exe MapleStory.sln -t:Rebuild \
  -p:Configuration=Release -p:Platform=x64 -p:PlatformToolset=v142 -m
```

⚠ **Git Bash 下必须用 `-t:` 而不是 `/t:`**：MSYS 会把 `/t:Rebuild` 当路径转换，
MSBuild 收到两个"项目" ⇒ `MSB1008: 只能指定一个项目`，构建根本没跑（我第一次就踩了这个）。
`/p:` 形式没事，因为带 `=` 的参数不参与路径转换。

## 3. 结果

| 项 | 值 |
| --- | --- |
| error 计数 | **0**（仅既有 `LNK4098` / `LNK4099`） |
| 脏 obj | **0 / 274** |
| 产物 | `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（`MapleStory.vcxproj` 的 OutDir 就是客户端目录，构建即部署） |
| md5 / size / mtime | `9636f77b4030ab4d9a85379cb0694911` / 4,419,584 B / 2026-11-01 14:16:01 |
| 协议门禁 | `GATE OK: opcode matrix matches both sources (s9 44d0d5d9ca)` + `EXITCODE=0` |
| 封包版式 | `tools/attack_layout_check.py` ⇒ `[修复后] 0/13 用例异常`；`--legacy` ⇒ `9/13` |

## 4. 真机回归（R26 / #67 就此闭环）

按项目记忆的规矩，部署后**第一关只验能不能登录进图**，且判据不能再用 `_boot.py` 的底部亮度
（NEXON splash 白底假阳性，open-issues **A13**）。

1. `python _drive.py where` → `NO WINDOW`（确认没有旧进程占着 exe）。
2. `python _boot.py wait` → 10s 后 `HUD bright px=66560 -> in game`。
3. 抓帧 `_v_j8boot.png`：选择频道页正常，世界/频道信息进得来（服务端在跑）。
4. 再抓帧 `_v_j8full.png`：**已在图内**（彩虹岛：蘑菇森林），底栏血蓝/经验条、小地图顶带、
   聊天四页签、`0级 / 0 / 789 / 0 / 0` 全部渲染；5 颗 蓝蜗牛/香菇怪 带 `100/100` 血条刷出。
5. 全程未杀进程。

⇒ 修前那份增量构建的 exe（`0688b7c1…`）在同一台机、同一份存档上**必崩**，现在一次进图。
`Foothold`/`PhysicsObject`/`AttackResult` 三个公共头的布局改动都已被全量重建覆盖。

## 5. 调试日志处置

- J 段本口的文件（`Attack.h`、`Combat.cpp`、`AttackAndSkillPackets.h`、`UIComboCounter.cpp`、
  buff 掩码相关）**零** `[DBG-*]`、零 `LOG_DEBUG` ⇒ 无需清理。
- 全仓改动文件里仍带 `[DBG-*]` 的只有 5 个：`Character/Char.cpp`、`Configuration.cpp`、
  `Graphics/GraphicsGL.cpp`、`MapleStory.cpp`、`Util/Resource/ResourceProbe.cpp`。
  逐条核过：全部属于**仍未闭环**的 #44（真机验证轮）/ #67（R26 崩溃取证）诊断桩，
  且多数已是「闭环后不再落文件（no-op）」的形态；`ResourceProbe.cpp:675` 那条
  `[DBG-PROBE] 临时注释版本检查` 实际在诊断报告路径内（写 report 文件），
  不是游戏功能开关 ⇒ 不影响玩法。
- 结论：**本轮不删**，等 #44/#67 真机项收完再统一清理，记在 CHECKLIST §K。

## 6. 提交状态

J1–J8 全段截至本文仍未提交（`git status` 1269 条目，最近提交是 maker/alliance/monsterbook）。
按门禁 12「单一功能一次提交」，建议拆成：J1 怪物、J2 技能数据、J3+J4 三家族创建、
J5 转职/任务、J6 buff+战神连击、J7 出站封包、J8 文档与工具 —— 等用户授权后执行。
