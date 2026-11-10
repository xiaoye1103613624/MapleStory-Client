# round76 · UIGuild 公会浮窗按 Angel 083 只读取证复核（2026-11-11）

## 0. 授权与边界
- 目标 goal-11388c42 Round 76/90：以 Angel(13339) 083/v83 带符号 IDB 为权威逐功能 REA→IDA→IMG 只读取证。
- m04292 授权改源码/自测/提交；用户 m09859 硬指令「ui 布局/动画非按 Angel 修过的可重新修复」。
- 本轮结论是**架构级**问题（删窗/换根会动 UIElement::Type、GuildModel、底栏入口、s9 handler、dimension），按门禁交决策点 D-G，零实施；只落纯注释口径纠正（0 行为字面量）。

## 1. 选面
全仓扫「截图 SAD / 自造 / 凭空 / 兜底」剩余 7 面，`IO/UITypes/UIGuild.cpp:52` 自认「属于凭空写出来的逻辑」信号最尖锐；083 `*Guild*` 54 符号密度全场最高；round68 已校准 `CUIUserList` 共用窗证据链可复用。

## 2. 当前端逐字（IO/UITypes/UIGuild.cpp 381 行 / .h 94 行）
- `:39` `UIElement(Point<int16_t>(300, 40), Point<int16_t>(300, 380))` —— dimension 硬编码。
- `:44` **`nl::node guild = nl::nx::UI["GuildUI.img"];`** + `:45-46` `if (guild["backgrnd"]) sprites.emplace_back(...)`。
- `:48-52` 旧注释「本包 UI.nx 根下没有 GuildUI.img ⇒ 四颗钮无原生贴图，只能显式预留…属于凭空写出来的逻辑」。
- `:58-103 draw()` 全文字自绘（title/notice_label/bbs_label/成员行），`update()` 空。
- `:53-55` 只有 `BtClose = UiAssetResolve::close_btn()` 单点守卫；BtCreate/Invite/Notice/Alliance 无任何贴图。
- `.h:65-72` `enum Buttons { BtCreate, BtInvite, BtNotice, BtClose, BtAlliance };` / `ROW_H = 18`。

## 3. 083 活 IDB（禁凭记忆，tools/_guild_ida*.py）
- 全库 `*Guild*` **54 符号**，`*CUIGuild*` / `*UIGuild*` 全是 `CUIGuildBBS` —— **083 无独立 CUIGuild 浮窗类**。
- 公会主体 = `?Draw@CTabGuild@@QAEJ... 0x90f17c (size 0x1a12)` 挂 `CUIUserList::OnCreate @0x919b73` 共用窗第 3 页签；独立看板 = `CUIGuildBBS::OnCreate @0x8133a9 (0x6c2)`。
- `CTabGuild_Draw.dec.txt` 1469 行抽 4 SP_（`SP_3348__GUILD_NOTICE` / `SP_350_YOU_ARE_NOT_IN_THE_GUILD` / `SP_5641_NONE` / `SP_2082__D_D_`）；`CUIGuildBBS_OnCreate.dec.txt` 3 个 vtable+32 控件立即数 `a3:2019 (1,3)` / `2021 (40,450)` / `2013 (680,501)` + `SP_3724_UI_GUILDBBSIMG_GUILDBBS_BTQUIT`。
- `search_text`：`"GuildUI"` 0 / `"Guild.img"` 0 / `"GuildBBS.img"` 0（动态 UOL） / `"UserList"` 30 / `"UIWindow"` 30。
- 当前端自己已知道：`IO/UIStateGame.cpp:361-363`「v83 G(17)=GUILD『家族』…原生是共用窗第 3 页签，不是独立 UIGuild 浮窗（浮窗仍由底栏社区钮保留入口）」→ **主路径已按 083 修好**。

## 4. orange-wz AGENTS.md 门禁 + .nx 回退
- `tools/_guild_probe.py`：`KEY b64_len=87384 raw_len=65536 sha256_16=268a8407a47ee76c iv=TSPHKw==`，`load_files exclusive:false`。
- `GuildUI.img` → **IMG_MISSING**（物理 `Data/UI` 只 20 个 .img，无此文件）；`GuildBBS.img` / `UIWindow.img` 结构层 WORKS、内部节点 `IMG 解析失败`（GBK 原文留 `tools/_guild_probe_stdout.txt`）；`unload_all rootCount=0`。
- `.nx`（旧缓存 2026-09-21）：`GuildUI.img` **NOT FOUND**；`UIWindow.img/UserList/Guild` 8 子含 `backgrnd bitmap 233x304`；`GuildBBS.img/GuildBBS` 14 子含 `backgrnd 734x526`。

## 5. 运行时 open_merge 探针（决定性，拒绝拿旧 .nx 当运行时权威）
- `_tools/guild_probe/main.cpp` 镜像 `ImgSource::load_standard` 的 `nl::img_file::open_merge(gather(Data/UI))`；首编漏 `<nlnx/wz.hpp>` → `error C2039: "img_file": 不是 "nl" 的成员`，就地补头重编 `BUILD_RC=0`（探针自身校准，不计任务失败）。
- `tools/_guild_rt_stdout.txt`：`GATHER_FILES=20` / `ROOT_CHILDREN=20` / **`GuildUI.img_valid=0`** / `UserList_kids=9` `backgrnd 312x389` / `UserList/Guild_kids=8` `backgrnd 233x304` / `GuildBBS.img/GuildBBS_kids=14` `backgrnd 734x526` / `OPEN_MERGE_OK` / `PROBE_EXIT=0`。

## 6. 对照清单 + 定性
| 维 | 083 权威 | 当前端 UIGuild | 判定 |
| --- | --- | --- | --- |
| 窗口类 | 无独立 CUIGuild；CTabGuild 挂 CUIUserList 第 3 页签 | `Type::GUILD` 独立浮窗 | ❌ 架构偏离（主路径已另修） |
| 资源根 | `UIWindow.img/UserList/Guild` + `GuildBBS.img/GuildBBS` | `GuildUI.img`（五证为 0） | ❌ 虚构根 |
| 背景 | `233x304` / `734x526` 真位图 | 无底图、全文字自绘 | ❌ 自造 |
| dimension | 共用窗 312x389 | 硬编码 300x380 | ⚠️ 自定 |
| 主路径 | G 键 → 共用窗页签 | `UIStateGame.cpp:361-363` 已按 083 路由 | ✅ 已修 |
- **定性 = 第 8 例同族病**（找错节点后自造：虚构根 + 全文字自绘 + 083 无此类），与 round66 UIRecommendWorld / round68 UIBuddyList-UIParty 同构。
- **但**删窗/换真资源属架构级：会动 `UIElement::Type::GUILD` 枚举（`NUM_TYPES`/`EnumMap` 尺寸）、`GuildModel` 共用、底栏社区钮入口、s9 `Guild*Packet` handler、dimension 300x380 与真背景 233x304/312x389 不匹配 → 按「校准≠改回原生」+ 用户裁决门禁交 **D-G**，本轮零实施。

## 7. 决策点 + 一次性 Plan（交用户裁决）
- **D-G**：是否把额外 UIGuild 浮窗退役、底栏入口并入 083 `CUIUserList` 公会页签；或保留旁路但改用真资源（`UserList/Guild/backgrnd 233x304` / `GuildBBS.img/GuildBBS 734x526`）重绘。需五屏实机截图 + 用户裁决后另轮实施；本轮不新建/删除窗口类型、不改 dimension、不动协议。
- 一次性 Plan（裁决后）：① 定去留 → ② 若换资源则同步 dimension/拖拽区/按钮锚点 → ③ harness 静态守卫 + 场景 → ④ GATE + 真重编 → ⑤ 三文档 + 零署名原子提交。

## 8. 本轮补丁与提交 scope
- 2 块纯注释（`UIGuild.cpp:48-52` + `UIGuild.h:31-32`），**0 行为字面量**：`nl::nx::UI["GuildUI.img"]` / `Point<int16_t>(300,40)/(300,380)/(280,8)` / 五按钮行为 / `ROW_H=18` 逐字不动。
- 四文档：`evidence/guild-window-083-round76.json`、`notes/wave2-guild-window-083-review-20261111.md`、dev-plan §4 **2-31**、PLT **§42 round76**。
- 精确 `git add` 7 文件零署名原子提交；一次性脚本 / IDA 反编译 / 探针 stdout / build_log 仅本地证据链留存不进版本库。
- 保留 `IO/LoginBackground.cpp:77` 按 Angel 083 CLogin 线性插值（不重复返工）。

## 9. 证据优先级 + 教训
运行时探针 > orange-wz 结构层 > .nx 旧缓存 > 物理文件 > 活 IDB > 源码注释 > 旧战报。
- 五证同源（search_text / 物理 / orange-wz / .nx / 运行时）才能把「虚构根」钉死，单靠 `.nx` 会被「旧缓存」反驳（round70/71 教训）。
- 主路径已按 083 修好时，旁路自造窗不等于「整体返工」——先分层定性再决定实施边界，颗粒度不能一刀切。
- CRLF 文件（`UIGuild.h` / PLT）补丁只能对新增片段转 `\r\n`；LF 文件（`UIGuild.cpp` / dev-plan）保持 LF，避免 `\r\r\n`。
