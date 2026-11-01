# r810 — 拆分提交与推送台账（J8 收尾）

> 时间：2026-11-01 14:55–15:14（本轮）
> 触发指令：`按一功能一提交拆并推送`
> 分支：`feature/dev_0.0.1` → `origin/feature/dev_0.0.1`
> 结果：`169ea09..358193d`，共 **27** 个提交，推送后 `HEAD == origin`，工作树 0 条脏项。
> 本台账自身的提交（含 `job-families/README.md` 功能文档）为**第 28 个**，随后推为 `358193d..97954a2`。

## 1. 拆分前的规模与口径

| 项 | 数量 |
| --- | --- |
| 拆分前 `git status --porcelain` 条目 | 1272（目录折叠后） |
| 真实未跟踪文件（`--exclude-standard`） | 2806 → 屏蔽副产物后 2651 |
| 已跟踪且被修改 | 224 |
| 进入源码类提交的path | 430 |
| 进入文档类提交的文件 | 2427（含 2 篇 README 更新） |

> **注意口径**：`git ls-files --others` 不带 `--exclude-standard` 时**不查忽略规则**，
> 会把 `build_check/`、`.codegraph/`、`__pycache__/` 全列出来。第一次统计因此虚高，
> 复核用 `git check-ignore -v` 才确认 `.gitignore` 早已生效。

## 2. 推送前的两道门禁

### 2.1 副产物屏蔽（`c986a5d`）

约 **120 MB** 草稿从未跟踪状态直接入库的风险：`.codegraph/` 66 M、`build_check/` 52 M
（内含 `MapleStory.exe` 4.3 M + `.pdb` 47.8 M）、`build/` 1.7 M，以及根目录散落的
`$log`、`build_log_t12b.txt`、`_pw_hits.txt`、`_w8_*.png`。先补 `.gitignore` 再拆，
未跟踪文件从 2806 降到 2651，全部 >2 MB 的非字体文件都被挡在门外。

### 2.2 凭据扫描发现明文口令（已脱敏，未入库）

扫描命令命中：

```
docs/features/port-095/evidence/world_props.txt:74:net.sf.odinms.database.password=sa!@#456
```

这是 095 参考端 `world.properties` 的取证快照，含**数据库账号 + 口令成对明文**。
远端是 GitHub，提交即公开，所以按「evidence 须脱敏」处理：

1. 该文件当时已随 `docs(port-095)` 提交（`0e301a3`，其后还有 `7ed1030`）；
2. 用 `git reset --mixed HEAD~2` 撤回**这两个本地未推送提交**（不动工作树，文件全部留在原处）；
3. 字节级替换为 `<REDACTED-DB-USER>` / `<REDACTED-DB-PASSWORD>`，保留键名与「存在该项」这一取证事实；
4. 按原范围重新提交为 `13a54a0` / `22d5e16`，提交信息内声明已脱敏；
5. 复扫：对 `origin..HEAD` 全部 27 个提交树逐个 `git grep 'sa!@#456'`，命中 **0**。

## 3. 提交清单

### 3.1 源码（21 个，合计 430 path）

| sha | 分组 | path 数 | 对应轮次 |
| --- | --- | --- | --- |
| `502c682` | startup（片头 + i18n + 字体 + 配置） | 12 | boot-splash / i18n |
| `043ada0` | render（CJK 按需字形 + UTF-8 度量） | 4 | T12 / T12b |
| `f69a1b3` | transport（GBK 编解码 + opcode 门禁） | 16 | T5/T6/T10/T11/T3 |
| `ced32af` | asset（命名空间解析 + 后台预热 + 诊断） | 9 | W0 / 登录卡顿 |
| `5051252` | physics（履带力 + 装载时序） | 13 | R23-A |
| `54acd5b` | map（怪物/NPC/召唤/宠物） | 23 | J1 |
| `06ee330` | combat（技能命名空间/攻击位/连击/buff 掩码） | 25 | J2/J6/J7 |
| `d5ecd8c` | quest（updateQuest + QuestData + 任务窗） | 5 | J5 / W10 / R23-C |
| `ea11fba` | creation（三家族创建路由） | 6 | J3 |
| `d37f0e6` | login（登录/选角/世界/频道版式） | 20 | login-flow-ui-ida-align |
| `60f557d` | social（好友/公会/联盟/组队/聊天/私聊） | 33 | R14-9/10/11 |
| `df31fb1` | char-panels（能力/装备/背包/技能/图鉴窗） | 12 | W6–W9 / R14-1/2/3 |
| `e7efc8d` | commerce（仓库/交易/制造/个人商店） | 15 | merchant / maker |
| `042ead2` | cashshop（商城/每日礼物/彩票/大锤/MTS） | 19 | r17 外壳 |
| `81e51ea` | npc（对话按钮 extend_id + 商店/事件框） | 8 | R23-B |
| `f5af924` | systems（20 个缺失系统窗外壳 + handler + 出站包） | 54 | T14 |
| `416201f` | tooltip（四类提示 + ColorBox opacity） | 13 | W5 / R14-6 |
| `0cd8303` | hud（底栏/快捷格/软键/小地图/大地图） | 18 | W1–W4 / W11 / R14-7/8 |
| `cf62050` | widgets（按钮四态/滚动条/图标/名牌 + UIElement） | 15 | W0 / R14-5 / W12 |
| `3071de6` | register（PacketSwitch + vcxproj + 出站聚合头） | 9 | 全部新增注册 |
| `c5c1af1` | tools（版式测量与取证一次性脚本存档） | 101 | W/R 轮辅助 |

### 3.2 文档与收尾（6 个）

| sha | 内容 |
| --- | --- |
| `c986a5d` | chore(gitignore)：屏蔽副产物 |
| `26667ce` | docs(beidou-v17-parity)：r001–r809 笔记 824 篇 + evidence 434 项（共 1260 文件） |
| `2f833e6` | docs(ui-v83-window-restore)：PLAN/CHECKLIST + 逐窗证据（1008 文件） |
| `13a54a0` | docs(port-095)：095 线笔记与 opcode-v95 取证（脱敏后重建） |
| `22d5e16` | docs(s9-protocol-align)：协议对照与探针证据（脱敏后重建） |
| `358193d` | docs(readme)：启动动画 + opcode 门禁两篇功能文档更新 |

## 4. 必须如实声明的三条限制

1. **拆分粒度是「文件」，不是「代码块」**。同一文件里叠加的多条轮次无法再分——例如
   `IO/UITypes/UIQuestLog.cpp` 同时含 W10 与 R23-C 与 J5 的改动，`IO/UITypes/UIChatBar.cpp`
   同时含 W4、R14-9 与 R15。因此每个提交的边界是「这个文件属于哪条功能线」，不是「这一个 hunk 属于哪条」。
2. **共享注册件只能落一次**：`IO/UIElement.{h,cpp}`（Type 枚举）、`Net/PacketSwitch.cpp`（dispatch 表）、
   `MapleStory.vcxproj{,.filters}`（工程文件）横跨多条功能线，统一归入 `3071de6`（register）与 `cf62050`（widgets）。
3. **中间提交不保证各自可构建**。只有 `358193d`（tip）的树等于本轮真机验证过的那份构建：
   md5 `9636f77b4030ab4d9a85379cb0694911`、size `4,419,584 B`、mtime `2026-11-01 14:16:01`
   （见 r807）。拆分过程只用 `git add`/`git commit`，未改动任何源文件；唯一被改动的内容是 §2.2 那个 evidence 文本。

## 5. 遗留（不在本轮处理）

- `fonts/SimHei/simhei.ttf`（9,745,792 B）随 `502c682` 入库：它是 `Configuration.h` 里
  `font.path.normal` / `font.path.bold` 的默认值，缺它中文即空白，且仓库已有 `fonts/Arial/*.ttf` 先例。
  但 **SimHei 是微软随系统分发的字体，公开再分发存在许可争议**，若本仓库要对外，建议改为「首次运行自动定位系统字体」或换成可自由再分发的 CJK 字体（如思源黑体）。
- `[DBG-*]` 标记共 **7 个文件、25 处**，逐个查过落地方式，只有 1 处真的会在 Windows Release 落文件：
  - **仍在写文件**：`Graphics/GraphicsGL.cpp:33 dbgatlas()` → `atlas_dbg.log`（限 200 条），调用点 `:581`（stale-offset）与 `:794`（null-offset-draw）。
    `GraphicsGL.cpp` 确在 `MapleStory.vcxproj:289` 的编译清单内，无开关拦截。属登录页竖条纹轮遗留，待 #44 关闭后整段删除。
  - **已中和为 no-op**（保留桩便于复现）：`Char.cpp` `DBG-CHAR`、`BodyDrawInfo.cpp` `DBG-BDI`、`GraphicsGL.cpp:562` `DBG-GO`、`MapleStory.cpp` `DBG-INIT`（均为空 lambda / 注释）。
  - **非本平台或需显式触发**：`Configuration.cpp:281` `DBG-CFG` 在 `#ifdef __ANDROID__` 内；`Net/SocketPosix.cpp` `DBG-SOCK` 不在 vcxproj 清单（Windows 不编译）；
    `Util/Resource/ResourceProbe.cpp` 的 17 处只在 `MapleStory.cpp:376/379` 经命令行 `--probe` / selftest 进入，正常游玩不触发。
  - 结论：本轮不删（最小改动，且 #44 真机验证轮仍未闭环），但 `atlas_dbg.log` 这条要在验证轮结束时随调试日志清理一并移除。
- `docs/features/*/evidence/` 内还有若干 `127.0.0.1:<port>` 的本机 MCP 端点，属本地调试地址、不含凭据，按现状保留。
