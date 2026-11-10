# round66 · UIRecommendWorld 按 Angel 083 布局重修（WIP 收口）

## 0. 性质与授权

- 用户硬指令 (m09859)：「若碰到 ui 布局、动画一定要确认是否修复了，如果不是按照 Angel 布局修复过的，可以重新修复；之前有很多参照 BeiDou 或者自行修复的 ui、动画都有些问题；」
- 本轮定位：不是「把外部程序留下的半截活儿补完提交」，而是**按 Angel 083 权威布局重新校验并重修**。中途交接期间（round56–65）外部程序留下 8 个未收口 WIP，其中「查看推荐世界」弹窗是典型「找错 IMG 节点 → 自造手绘兜底」的自行修复，正是用户点名的病。
- 授权：m04292（改源码 / 自测 / 测试完成提交 / 占用直关 / 不能自测记 backlog）；门禁 18 REA→IDA→IMG 只读取证；门禁 8 WZ/IMG/XML 走 orange-wz MCP。
- 只读结论：**校准 ≠ 改回原生**。数据源沿用当前端 SERVERLIST 世界列表 + `change_world(World)`，不动 s9 协议、不动 `RecommendedWorldMessageHandler`，仅按 083 重修 UI 布局层。

## 1. 缺陷逐字（为什么判它是「未按 Angel 布局」）

外部程序 WIP `IO/UITypes/UIRecommendWorld.cpp`（169 行，从未编进 build_log）：

- 背景取 `UI["Login.img"]["WorldSelect"]["Alert"]["Backgrnd"]`（**大小写 + 拼写双错**），缺失回退 `["WorldSelect"]["chBackgrnd"]`（也拼错），再缺失 `dimension=Point<int16_t>(400,260)`。
- 判定「BeiDou Login.img 经 R6 裁剪，WorldSelect/Alert/* 节点缺失」→ 用手绘 `Text` 画「推荐世界」标题 + `<` / `>` / 「选择」，手算 `Rectangle` 分派点击，`draw()` 自己画背景绕开基类 `draw_sprites/draw_buttons`。
- 注释把这套兜底写进 `IO/UIElement.h:50-53` 与 `IO/UITypes/UIWorldSelect.cpp:1212-1214`，形成「资源缺失」的错误结论链。

**翻案（决定性）**：`.nx` 下 `Login.img/WorldSelect` 直接子节点 9 个 = `BtGoworld / BtViewAll / BtViewChoice / BtWorld / alert / chBackgrn / channel / scroll / world`；`alert` 子树完整 = `BtArrowL / BtArrowR / BtChoice / BtClose / backgrd`。`Alert` / `Backgrd` / `Backgrnd` 0 命中。真节点 `alert/backgrd` 是 **bitmap 540×167**，导出 `tools/_rw_visual/backgrd.png` 可见深棕边框 + 金色 "Recommended World" 标题 + 左右两块浅色内容面板。**资源根本没缺**——WIP 是找错节点后自造，属用户点名要重修的「自行修复 UI」。

另外发现 `IO/UITypes/UICashShop.cpp:98` 有一行与 `:92` 完全相同的 `buttons[Buttons::BtChargeRefresh] = ...(92,554)` 重复赋值残留（R6 守卫批次带入），本轮一并删除。

## 2. IDA 083 取证（`tools/_ida_recommend/*.txt`，9 个函数反编译全落盘）

`entity_query {"kind":"functions","filter":"*RecommendWorld*"}` 命中 12 个 083 真符号，证实功能依据为真（非编造）：

- `CUIRecommendWorld::OnCreate @0x62b451`：背景 `SP_5362 ..._ALERT_BACKGRD` → `CWnd::SetBackgrnd`；**CCtrlScrollBar id=1003 `(1,7,502,58,85,0)` @0x62b546**；`SP_5363 ..._BTARROWL` → id=1000 **(45,70)** @0x62b5e2；`SP_5364 ..._BTARROWR` → id=1001 **(215,70)** @0x62b662；`SP_5365 ..._BTCHOICE` → id=1002 **(113,114)** @0x62b6df；`ResetWorldAndMessage(1)` 收尾。
- `OnButtonClicked @0x62b708`：`IsRequestValid` 守卫后 1000→`MoveWorld(-1)` / 1001→`MoveWorld(1)` / 1002→`SelectWorld()`。
- `MoveWorld @0x62b99d`：`*(this+33) = (nMovement + v3 + *(this+33)) % v3`（v3 为数组长度；**v3==0 时原生 `%0` UB**）。
- `SelectWorld @0x62b96b`：`CUIWorldSelect::SetFocusWorld(...worldID...)` 后关窗。
- `ResetWorldAndMessage @0x62b9da`：**数组长度 >1 才启用两箭头**；`SP_1366 WORLD_D` 取世界描述图；`IWzFont::CalcLongestText(..., 200, ...)` **换行宽 200**；按 `'\r'` (13) 分行；`CCtrlScrollBar::SetScrollRange`。
- `OnMouseButton @0x62bef3`：`v7[0..3] = {78, 61, 212, 110}` 消息区矩形，命中 WM_LBUTTONDOWN 直接 `SelectWorld`。
- `CLogin::OnRecommendWorldMessage @0x5f8340`：包结构 = count(1B) + N×(worldID 4B + msg string)。

## 3. orange-wz 门禁（AGENTS.md 合规流程）

- `mcp__orange_wz__list_loaded_roots` → `会话不存在或已过期`（会话过期，非内容层失败）。
- 改走 `tools/_orange_wz_probe.py`（同 endpoint `http://127.0.0.1:10012/mcp`，key 从 `tools/_gms_key.b64` 读，避免 87K 入上下文）：`initialize` → `load_files exclusive:false` → 三查 `WorldSelect` / `WorldSelect/Alert` / `WorldSelect/Alert/Backgrd` 全部 **`IMG 解析失败: Login.img`** → `unload_all`。
- **原始报错文本已留证**，`IMG 解析失败` 触发条件满足 → 合规 `.nx` 回退。
- key 规格：`b64_len 87384 / raw_len 65536 / sha256_16 268a8407a47ee76c / head lq4/pEj63ZBGdgVhl854aCugRI/BVn4y/OH1sxQUxSL1w2gu`，`iv TSPHKw==`（GMS 4B `[0x4D,0x23,0xC7,0x2B]`）。数据目录 `E:/MXD/BeiDou-Client-xy/Data/UI/Login.img`（7212655B）、`.nxcache/UI.nx`（86919577B）。

## 4. 最小修复（按 083 立即数整文件重写）

`IO/UITypes/UIRecommendWorld.h/.cpp` 重写：

- 真背景 `alert["backgrd"]` 进基类 `sprites`（`UIElement::draw` 自动加 `position`），`dimension = (540,167)`。
- 三颗真位图按钮经 `try_maple_btn(buttons, id, alert[...], (x,y), "UIRecommendWorld")` 落地；`try_maple_btn` 的 `position` 是 sprite 左上角（内部加回 normal 态 origin），故 083 立即数可直落。
- 复用基类 `draw_sprites/draw_buttons`、`update()`、`send_cursor()`（不再空 override、不再手算 rect 分派按钮）；`draw()` 仅在基类之上叠加世界名牌图 / 名称文字 / 消息文字（换行宽 200）。
- `button_pressed` switch 1000/1001/1002 对齐 `OnButtonClicked`。
- `send_cursor` 消息区 `Rectangle((78,61),(212,110))` 相对 `position` 偏移，命中且 clicked 直接 `SelectWorld`，其余委托 `UIElement::send_cursor`。
- `refresh()` 世界数 >1 才把两箭头置 NORMAL（否则 DISABLED 显示真素材 disabled 位图）；只读访问一律 `buttons.find(id)`（`operator[]` 会插空 `unique_ptr`）；名牌图按 `Login.img/WorldSelect/world/<world.id>` 取、缺失回退 `"0"`（对齐 `UIWorldSelect.cpp:995`）。
- `move_world` 加 `wc==0` 守卫 + `int64_t` 环绕（修掉 083 原生 `%0` UB）。
- `select_world()`：`worldselect->change_world(*w)` + `UI::get().remove(TYPE)`；`UIStateLogin::remove` 用 `element.release()` 放弃所有权不析构，返回后基类继续 `set_state` 不会 use-after-free。

配套修正：`IO/UIElement.h:50-53` 与 `IO/UITypes/UIWorldSelect.cpp:1212-1214` 的错误「R6 裁剪 / chBackgrn 兜底」注释换 083 真依据；`IO/UITypes/UICashShop.cpp:98` 删重复赋值。

**保留（确认按 Angel 修复过，用户硬指令要求逐一确认）**：
- `IO/LoginBackground.cpp:77-86` 已按 Angel 083 CLogin `v62 = 7000*(v63-a2)/v62/2` 改**线性插值**（删 smoothstep S 曲线，注释引 login-flow-angel-calibration-2026-11 README §6 D5 推荐(a)）→ 按 Angel 修复过，保留不动。
- `IO/UITypes/UIWorldSelect.cpp:1210-1217` `BtViewChoice → emplace<UIRecommendWorld>(this)` 真窗路由、`UIWorldSelect.h:64-72` 三 getter、`MapleStory.vcxproj:428` 编译项保留。

## 5. 自测门禁（全绿）

- `tools/test_recommendworld_layout.py`：**46 静态守卫 + 19 场景**，`STATIC GUARDS checked=46 failed=0` / `SCENARIOS 19, bad=0` / `PASS` / `TEST_RC=0`。守卫覆盖 083 立即数（540/167、三按钮坐标、{78,61,212,110}、换行宽 200、id→行为）、真节点名、旧手绘/错节点痕迹清除、三处错注释修正、UICashShop 无重复赋值、LoginBackground 线性插值仍在；场景覆盖 move_world 环绕（wc=0/1/3、负向、多圈）、箭头 enable/disable、select_world 空/非空。
  - harness 自身 1 次口径校准（不计任务失败）：初版把 .cpp 留证注释「旧实现误写 ["Alert"]["Backgrnd"]」里的错误节点字面量当成残留命中 → 让留证注释不复用错误节点的精确字面量（既保取证链又避免日后 grep 真假难辨），复跑全绿。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3741089c0f)`。
- `git diff --check` → `DIFFCHECK_CLEAN`。
- 真重编 `MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c tools\build_smoke.bat` → `GATE OK` ×2 + `EXITCODE=0`；`build_log.txt` **3073B(21:53) → 460448B(23:16:51) 真重编**，`UIRecommendWorld.cpp` 编到（2 次）、`UICashShop.cpp`(32) / `UIWorldSelect.cpp`(18) 覆盖，`ERROR_COUNT 0`；`MapleStory.exe` 23:16:50 / 4943872B 重链接。

## 6. 范围边界（如实披露，不冒充完全精确）

- 083 反编译未给世界名牌图 / 消息文本的精确 anchor 立即数 → 按 `backgrd.png` 左右浅色面板几何居中放置（`name_pos(45,116)` / `message_pos(283,68)` / 名牌图 x145 居中 y72）。**精确像素对齐登记 PLT 真机观感验收**，不宣称「完全精确」。
- 083 `CCtrlScrollBar id=1003` 与按 `'\r'` 分行、`SetScrollRange` 未复刻（WORLD_D 消息是服务端下发的多行描述，当前端 `event_message` 为单行文本）；长消息滚动条属后续增强，登记 backlog。
- 083 `SelectWorld` 用 worldID、当前端用数组下标 `worldid`，语义不完全相同 → 按当前端 `change_world(World)` 语义落地，不照搬 083 参数（校准≠改回原生）。
- 本轮**不动** `RecommendedWorldMessageHandler`（opcode 27 目前只把推荐消息当聊天提示，未存列表、未弹窗）；弹窗由 `BtViewChoice` 真窗路由驱动，与 083 `CLogin::ViewRecommendWorld` 的服务端触发路径不同 —— 是否接 opcode 27 自动弹窗属协议级决策，登记待裁决。

## 7. 真机 backlog（交给用户启 s9 + 截屏）

- 66-1：`BtViewChoice` → 推荐世界弹窗真背景（540×167 深棕边框 + 金色标题 + 左右浅色面板）观感，与 `C:\Users\11036\Desktop\正确客户端截图\` 原生截图对照。
- 66-2：三颗真位图按钮（左右箭头 28×30 / 选择 62×26）位置、四态（disabled/normal/mouseOver/pressed）、点击左右箭头切世界、点「选择」关窗并落到对应世界。
- 66-3：世界名牌图 / 名称 / 消息文字在左右面板的精确对齐；消息超 200 宽换行；多世界时箭头启用、单世界/零世界时禁用态位图。
- 66-4：消息区 {78,61,212,110} 点击直接选中世界是否符合原生手感。
- 66-5：LoginBackground 线性插值滚动补间观感（本轮确认按 Angel 修复过，保留）——与原生滚动速度对照。

## 8. 提交 scope 与口径差异披露

- 提交文件（9）：`IO/LoginBackground.cpp` / `IO/UIElement.h` / `IO/UITypes/UIRecommendWorld.{h,cpp}` / `IO/UITypes/UIWorldSelect.{h,cpp}` / `IO/UITypes/UICashShop.cpp` / `MapleStory.vcxproj` / `tools/test_recommendworld_layout.py`。零署名、精确 `git add`。
- **口径差异（需用户追认，本轮不替用户销案）**：`docs/features/development-and-fix-plan-2026-11/README.md` §11.1 把「L-B 全部 12 项保持 s9 优先、无变更」记为 m04292 裁决，但 m04292 原文只是授权「开发和修复、自测、测试完成提交、不能自测的记录下来一起测」，并未逐条裁决跨面 D 矩阵；之前挂着的 K3+K4 64 条（残差 28）待裁决被外部程序整体销案。本轮不改动该裁决口径，仅在 §11.3 补一条如实披露。
- §11.2 的「2-1 ✅ 已构建部署」在外部程序写就时 UICashShop.cpp 守卫尚未提交、未进构建日志；本轮真重编已覆盖 `UICashShop.cpp`（32 次），口径现在与现场对齐。

## 9. 证据优先级与教训

- 优先级：live 运行时行为 > 抓包 > 主动服务资源 > 进程配置 > 持久化状态 > 生成产物 > 源码 > 注释/死代码。本轮「资源缺失」结论被 `.nx` 实测节点 + 视觉导出直接推翻，注释链不能反压运行时事实。
- 教训 1：**大小写 / 拼写错的 IMG 节点查询会伪装成「资源缺失」**，进而诱出自造兜底 —— 立项「R6 降级」前必须先把真实节点名枚举出来对拍。
- 教训 2：**用户硬指令「UI 布局、动画一定要确认是否按 Angel 修复过」要逐文件核**，本轮就用它把「自行修复」的推荐世界弹窗翻案，同时确认 LoginBackground 动画确实按 Angel 改过予以保留。
- 教训 3：留证注释不要用错误节点的精确字面量（harness 会误命中），改写措辞而非放宽审计口径。
- 教训 4：CRLF + tab + 中文的 `old_string` 用 `edit` 工具易失配，本仓定式走 python 精确替换脚本 `tools/_rw_patch.py`。
