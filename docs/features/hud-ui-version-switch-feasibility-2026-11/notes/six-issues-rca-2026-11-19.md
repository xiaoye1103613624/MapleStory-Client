# 用户 6 点 HUD/主题需求 RCA 与修复状态（2026-11-19）

> 口径：逐条给行号级结论；真机未验收的一律标「真机待验」，不拿参考端截图当本端验收。
> 对照源：IDA `MapleStoryT.i64`（用户口头称 192，实为 StatusBar2 契约）+ orange-wz 192 分卷 UI_002/004/005.wz。

## ① 小地图：视口上限 / 跟随 / 未显示区域图标溢出

| 项 | 结论 | 行号证据 |
|---|---|---|
| 视口上限（禁止整图 overview） | 已修（前轮） | `UIMiniMap.h:138-139` MAX_PANE_W=200/MAX_PANE_H=150；`UIMiniMap.cpp:871-940` update_canvas 钳制 |
| 随角色移动滚动 | 已修（前轮） | `UIMiniMap.cpp:971-1008` `refresh_map_viewport()`，192 CalculateScr@0x14316B0 同款：玩家像素−视口半宽+clamp |
| **未显示区域传送门/NPC 图标溢出** | **本轮修复** | 新 `UIMiniMap.cpp:977-988` `canvas_pos_in_view()`，语义对齐 192 `IsPtInViewRange@0x1431DD0`（`0<=cx<=pane_w`）；NPC/角色标记 `:1024-1045`、静态传送门 `:199-210 / :254-265` 全部加裁剪；`static_marker_info` 由 pair 改 tuple（`:124`）同时存 pane 坐标(命中)与画布坐标(裁剪)，命中判定改 `std::get<1>`(`:481`)、`std::get<0>`(`:483`) |

编译：BUILD_EXITCODE=0（hud_theme_build.log）。真机待验：裁掉视口外标记后小地图观感需用户进图确认。

## ② 地图 NPC 全部消失

**根因（RCA 坐实，见 evidence/npc-disappear-rca-2026-11-19.md）**：工作区里前轮未提交改动把非授控/不可动 NPC 设为 FIXATED 并在 update 跳过 `physics.move_object` → `update_fh()` 不执行 → `phobj.fhlayer` 锁死默认 0 → MapObjects 按 layer 登记到 ZERO → Stage 先画层0、被上层 tile 覆盖 → 全场 NPC「消失」。IDA 参考端 `CNpc::Update@0xdc85a0` 无条件调 `UpdatePassive`，无"静态跳过物理"分支，改动注释缺乏依据。

**修复**：撤销 `Npc.cpp` 三处 FIXATED 改动（`:150-153` spawn、`:240-242` update、`:349-361` set_control），`git diff -- Gameplay/MapleMap/Npc.cpp` 现为空，回到原生物理链路。编译 BUILD_EXITCODE=0。

**边界（如实声明）**：该改动原本想治的「MapleTV 从天而降」不在本轮闭环——参考端靠 fh 正确定层（非 FIXATED），若真机仍见掉落，另案按 fh 对齐修，禁止再用 FIXATED 覆盖症状。

## ③ 左侧竖栏灰底 + 收起展开

用户裁决：裸图标常驻，无灰底、无折叠钮（wz 取证显示原生 ShortCutUI 确有 top/middle/bottom 底板，去掉属对原生的**有意改造**，以用户裁决为准）。

| 项 | 修复 | 行号 |
|---|---|---|
| 灰色底板 | 删 ColorBox | `UISideIconBar.cpp:297-303`（原 302-311） |
| 折叠钮创建 | 删 BT_FOLD 创建块 | `UISideIconBar.cpp:170-245` |
| 折叠/展开布局 | 改为常驻排全部槽 | `UISideIconBar.cpp:247-283` |
| 折叠点击逻辑 | 删 button_pressed BT_FOLD 分支 | `UISideIconBar.cpp:378-395` |

配置 `collapsed` 字段保留解析兼容（不再影响 UI），避免动 config 结构。编译 BUILD_EXITCODE=0。

## ④/⑤ 192 主题底部聊天残缺 / 主功能键 / 快捷键

- 关键证据：live 只有 `StatusBar.img` + `StatusBar3.img`，**无 `StatusBar2.img`**（192 真资源在怀旧岛 192 整合端 ResMan 分卷 UI_002.wz，跨端导入有闪退前科）。故本轮**不搬 192 资源**，用 StatusBar3 承载，把 IDA 钉死的布局/交互逻辑对齐。
- **翻案**：`UIChatBar.cpp:324` 的 `chat/ingame` 在 StatusBar3 **真实存在**（orange-wz 实测证伪了「无此节点」假设）。真因 = ①input 下按钮 `normal/0` 多为 1×1 占位（间距按 4px 算致 5 钮叠一起）②两处节点名写错（`button:itemLink` 应为 `button:Link`、`button:chatEmoticon` 实际在 `chat/chatCommandMenu/`）③modern 分支从未建聊天目标 UI。修复详见 chat-modern-fix-2026-11-19.md + evidence/wz-statusbar3-chat-qs-2026-11-19.md。
- **聊天修复**（`UIChatBar.cpp:324-451/775-820/978/1232/1965/2007`、`UIChatBar.h:270-275`）：占位按钮回退 `chat/common/Bt*_outside` 18×18、修正错名、补建目标框+列表（`target_ui_ready`），复用已验证的 classic 列表机制，未做整段自绘。
- **主功能键压血条修复**（`UIStatusBar.cpp:461-476`）：定量根因=menu 六钮 X∈[410,620]/Y∈[73,110] 与血条 X∈[416,620]/Y∈[40,110] 全重叠；六钮移到血条正上方 Y∈[1,38]。
- **快捷键修复**（`UIStatusBar.cpp:630-641`）：1024 档由 28 槽改 8 槽（旧 28 槽把 557 底图画到 x=1185 溢出屏幕）；IDA 佐证 800/1024 只显示 8 槽。
- IDA 布局证据（ida-statusbar2-layout-2026-11-19.md）：基准 `CLayoutMan::Init(512,577)@0x16BE531`；QuickSlot `s_ptShortKeyPos@0x279C390`=(7,15)/pitch33/第二排 y=48；底部按钮创建顺序（BtNotice→BtCharacter→BtCashShop→BtMenu→BtSystem→BtStat→BtQuest→BtInven→BtEquip→BtSkill→BtKeysetting）；chatTarget 9 标签。

## ⑥ 主题切换架构（策略模式）

见 `whole-ui-theme-map.md`「架构升级」节。新增 `Util/UiThemeLayout.h/.cpp`：`IThemeLayout` 策略接口 + 三实现 + 注册表；`UiTheme` 门面委托；`UiAssetResolve` 资源根委托。新增版本 = 加枚举+注册策略+字符串映射，不动消费端控制流。编译 BUILD_EXITCODE=0。

## 编译/部署/真机验收状态

| 项 | 状态 |
|---|---|
| VS2019/v142 Release x64 全量重编（6 点全合入） | ✅ BUILD_EXITCODE=0，产物 `hud_theme_out\MapleStory.exe` 4928000B，SHA256 `7b392fb97509cbf235e288a9766768c89f3926a161334827cbbb6acc75bddd9b`（2026-11-19 10:27） |
| live exe 部署 + SHA 核对 | ✅ 已部署（用户授权）：3 个占位进程 taskkill 报 Access denied（shell 非管理员、UAC 提权无人确认），但实测 exe 文件未被锁定，直接覆盖成功；覆盖即时 + 延迟 8s 两次 SHA 均 = `7b392fb9…`，未回滚 |
| 真机本端截图验收 | ⏳ 待用户**重启客户端后**提供本端截图复核；当前视觉结论均为证据推断，不宣称「真机已修复」 |

> 旧 SHA `d17df6db…`（天气版）→ 新 SHA `7b392fb97509cbf235e288a9766768c89f3926a161334827cbbb6acc75bddd9b`，live `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 10:32 覆盖。⚠️ 3 个旧进程仍在运行（内存映射旧 exe），**必须重启客户端**才加载新构建，否则看到的是旧表现（stale-exe 陷阱）。
