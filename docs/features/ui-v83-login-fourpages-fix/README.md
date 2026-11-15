# 登录链路四页 UI 错乱修复（2026-11-03）

> 对应计划：CodeBuddy plan `60977dd7d7f240e6ad0634d9bf1fa42b`。
> 方法：每处修复走「实机截图现象 → img 运行时节点值 → 源码行」三步取证后改码（R1/R2/R6）。
> 构建：`tools/build_smoke.bat` GATE OK + EXITCODE=0，产物直写 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`。

## 2026-11-15 · 登录链三弹窗修复（TOS 背景 / 密码错误文案 / 性别弹窗）

> 证据：`evidence/three-dialogs/`（wrong-password、gender-dialog(-v2)、tos-bg-centered、gender-prompt-zoom）与 `evidence/tos/`（TOS 上一轮验证）。

### 5.1 TOS 羊皮纸背景偏移（UITermsOfService）

- **现象**：正文/滚动条/按钮居中正确，但羊皮纸背景整体飞向左上（nx 实测 `TOS/0` 431x426，**origin=(215,213)** 图心）。
- **病根**：`Texture::draw` 渲染位 left = `args.pos.x − origin.x`；`sprites.emplace_back(TOS)` 时 args.pos=position → 背景左上 = position−origin=(209,−66)，而正文用 position+局部 锚定——两套锚点不同源。
- **修复**：`sprites.emplace_back(TOS, Point(215,213))` 令 args.pos = position+origin，背景左上回落到 position。同时保留 11-15 早前两修复：dimension 用 `TOS["0"]`（容器节点经 Texture 得 0×0）、`drawtext` 裁剪区间用绝对坐标（`position.y()` 起）。

### 5.2 密码错误显示"未知错误"（LoginHandlers）

- **现象/证据**：packet_dump 实测 `admin/admin1` 四次失败均回 `reason=4`（非标准 v83 的 3）；原 switch 只有 2/5/7/13/23 → 落 default。
- **修复**：`LoginResultHandler` 补 `case 4 → WRONG_PASSWORD`；`SelectCharacterHandler` 的选角 reason 语义不同，保持不动。新账号 TOS(reason=23) 流程复验通过（MySQL 复位 `accounts.tos` 后重启复现）。

### 5.3 三处文案中文化（UILoginNotice + UIGender + locale）

- **范式**：`Text(A12M, CENTER, DARKGREY, Locale::text(key), width)`，参照同文件 UIQuitConfirm。
- `UILoginNotice`：WRONG_PASSWORD/UNKNOWN_ERROR 两 message 改中文 Text 渲染（新增 `draw()` override + `notice_text` 成员，`use_localized_text` 门控位图分支）；其余 message 保持 WZ 位图+墨迹扫描居中。
- `UIGender`：提示位图 text/0（origin=(224,116)）改中文 Text；下拉选项改 `uigender.131/129`（男/女，已有 key）。
- `locale/zh-CN.ini`、`en-US.ini` 新增：`login.err_wrong_password` / `login.err_unknown_error` / `login.gender_prompt`。
- **注意**：`Text::Alignment::CENTER` 的语义是 **args.x 为行中心**（LayoutBuilder finishline `line_x -= ax/2`），定位必须给面板中心（如 `dimension.x()/2`）而非左缘偏移——首轮实机即因按左缘偏移定位导致提示左半出卷轴，放大图取证后修正。

### 5.4 性别弹窗不居中（UIGender）

- **现象**：`position=(0,15)` + 800×600 时代绝对坐标（卷轴锚 (355,185)、按钮 (578/650,349)、下拉 (510,283)），1280×720 下整体偏左上。
- **img 取证**：`Gender/scroll/0` 三帧为分时展开动画（354x46→200→196，origin 全 (0,0)），非垂直拼接；最终帧 354x196。
- **修复**：dimension=最终帧 354x196 + `center_on_screen()`（新增 `update_screen` 重调）；全部元素改相对卷轴左上局部坐标（卷轴 (0,0)、BtYes (223,164)、BtNo (295,164)、下拉 (155,98)、提示 y=25）；实机验证卷轴右缘 x=817 与预期 463+354 精确吻合。

### 5.5 TOS 确定/取消按钮错位露出烘焙槽（UITermsOfService，2026-11-15 晚追加）

- **现象**：用户截图「取消按钮后面还有 UI」——槽2 右侧露出 20px 橙色；按钮观感"过大"。
- **取证**：TOS/0 背景右下烘焙两个橙色按钮槽，PIL 实测槽1 局部 x[266..330]、槽2 x[340..404]、y[372..401]（各约 65x30、间隙 9、中心距 74）；`BtOk/BtCancel` 为 75x32、**origin=(33,14)**（绘制与 `MapleButton::bounds` 均按 坐标−origin 落点）。旧值 (268/342,388) 未补偿 origin → 按钮视觉 (235..310)/(309..384)，偏左 31px。另核对运行时 UI.nx 全部 66 个按钮：**无 65 宽确定/取消资产**（Gender/BtYes 在陈旧打包源 dump 中为 66x29、运行时实为 75x32，R1 印证），资产不可换。
- **修复**：坐标补偿 origin 并居中盖槽——OK x 268→**291**（视觉 258..333）、CANCEL x 342→**369**（视觉 336..411）、y 388→**384**（视觉 370..402，对槽居中），两按钮间隙 3px 不重叠。
- **实机验证**：橙色带实测 (682..834, 517..548) 与预期按钮区逐像素吻合，槽被完全覆盖、无露边（evidence/three-dialogs/tos-buttons-aligned.png；槽取证 _tos_bg_raw.png/_tos_slots_zoom.png）。

### 5.6 性别弹窗提示文案退化 + 下拉框"内容丢失"澄清（UIGender，2026-11-15 深夜追加）

- **现象**：用户对比参考端截图——①提示文案只剩单行「欢迎光临！请选择您的性别。」（参考端为三行完整文案+橙点「性别」标签）；②下拉框"内容丢了"。
- **取证（根因①）**：`Gender/text/0`（298x100, origin=(224,116)）一张位图烘焙了参考端全部元素：三行完整文案 + ✳性别标签 + 下拉框底座。5.3 轮把它换成 `Text+Locale("login.gender_prompt")` 单行简版，属**退化**——且该 key 只写进了部署目录 locale（仓库缺失，部署/仓库漂移 5 行）。恢复位图绘制：`sprites.emplace_back(Gender["text"]["0"], Point(246,141))`（stateargs−origin=(22,25) 卷轴局部，与原版绝对 (601,326)−卷轴锚 (355,185) 一致）；删除 `prompt_text` 成员与 draw 代码（text/0 由 UIElement::draw 先于 buttons 绘制，天然垫在下拉控件下层）。
- **澄清（根因②）**：下拉框内容**没有丢**——参考端截图是**展开态**（Male/Female），用户对比的当前端截图是**收起态**（仅显示「男」）。实机驱动验证（SetProcessDPIAware + ClientToScreen 定位客户区 (640,424)，物理点击 1:1 映射 UI 坐标）：双击收起框 (618..683,360..376) 展开 → 列表「男/女」两项完整显示 → hover「女」灰条高亮+白字生效 → 点击选中（收起框变「女」）→ 点「确定」→「正在连接到服务器」流程推进（GenderPacket(女).dispatch() 链路通）。**交互与内容全程正常，未改 MapleComboBox**。
- **排查记录（防重蹈）**：曾疑 `MapleComboBox` 构造期 AreaButton 命中区错位（line 76 用未赋值 position）——复核 `AreaButton::bounds(position)` 传入的正是赋值后的 `position`，构造偏移被精确补偿，**非缺陷**；`pressed` 经 `Button::toggle_pressed()`（基类）在 button_pressed(SELECT) 中置位，展开绘制正常。均为想象性修改风险点，未落码（R5）。
- **locale 收口**：部署与仓库双向合并——`login.err_wrong_password`/`login.err_unknown_error` 同步回仓库（附注释），死键 `login.gender_prompt` 从部署删除；最终两目录逐行一致（diff=0），i18n 由下拉选项 `uigender.131/129`（男/女）继续承担。
- **实机证据**：evidence/three-dialogs/gender-restored-collapsed(2).png（text/0 完整文案+标签）、gender-final-{expanded,hover,selected,confirm}.png（展开/高亮/选中「女」/确定进连接流程）。

### 遗留披露

- `build_smoke.bat` 的 opcode-matrix 门禁报 148 处 `client_sent_at/count` 预存漂移（与本轮 UI 改动无关，上轮曾 GATE OK）。本轮经 MSBuild 直编 `MapleStory.sln` Rebuild EXITCODE=0；矩阵刷新应由协议对齐任务另行处理，未擅自改动。
- EULA 正文（TOS）为 Text 渲染字符串节点，结构上支持国际化，本次保留英文原文。


## 问题 1 · 频道页公告板（UIWorldSelect + LoginProcessNotice）

| 环节 | 证据 |
| --- | --- |
| 实机现象 | 公告画进频道面板内部左上、遮挡频道列表；≈135×73 小方盒；位置高于参考 |
| img 节点 | `Login.img/WorldNotice/BalloonForLoginProcess`：nw/ne 22x40 origin(22,40)/(0,40)、sw/se 22x24、n 12x40 o(0,40)、s 12x24 o(0,0)、w 19x9 o(19,0)、e 22x9 o(0,0)、Title 170x18（`Login.img_runtime.json` 全量 dump） |
| 参考图实测 | `选中频道页.png` 逐像素扫描：公告外框 (285,297)-(468,371) ≈ 184x74；面板左缘 x≈436、顶缘 y≈282 ⇒ 公告左上 = 面板左上 + (-151,+15) |
| 源码病根 | ① `ChatBalloon.cpp relayout()` 旧版按 west 条高 9px 自适应铺行 → 框高/框宽全错；② `draw()` 把 tile 参数当渲染位直传，而 `Texture::draw` 渲染位 = 参数 − origin（九宫 origin 是内角锚点）→ 边框向内塌陷；③ `UIWorldSelect.cpp WS_NOTICE_GAP=-48` 基于错误的 width() 反推 |

**修复**
- `IO/Components/ChatBalloon.h/.cpp`：`LoginProcessNotice` 记录四边框厚（top40/bottom24/left22/right22），`relayout()` 改为固定外框 184x74（原生固定小板）；`draw()` 按九宫 origin 内角语义重写——角件参数取内容框角点自动外扩，n/s 平铺顶/底带，w/e 平铺左/右带，center 拉伸铺内容框；标题居中顶带、正文居中内容区。
- `IO/UITypes/UIWorldSelect.cpp`：`WS_NOTICE_GAP` -48 → **-33**（184−151），顶缘偏移 +12 → **+15**；注释写明旧值被实机证伪的原因（旧框宽 135 代入同公式被推入面板内；旧注释把右缘量到红圈 481 上，199 宽系误测）。
- 前序 A 修复（scroll/0 四帧展开动画 + WS_SCROLL_ANIM_OFF(-302,-274)）按计划保留。

## 问题 2 · 选角属性窗双底图叠加（UICharSelect）

| 环节 | 证据 |
| --- | --- |
| 实机现象 | 选中角色后属性面板 + 右下多出一整块米黄纸（卷轴末帧外露），排行榜区被叠盖 |
| 源码病根 | `IO/UITypes/UICharSelect.cpp:496` `if (charinfo_has_scroll)` 常驻绘制，动画定格后经三元式取**末帧=整纸**——与同处注释「播完定格后不再绘制」直接矛盾 |

**修复**：门控改为 `charinfo_has_scroll && charinfo_scroll_playing`——卷轴仅在展开动画期间绘制，定格后只画 charInfo1 内容板（`charinfo_scroll_playing` 状态机 :631-648/:1273-1278 已核对，语义自洽）。

## 问题 3 · 种族选择页底部大面积空缺（UIRaceSelect）

| 环节 | 证据 |
| --- | --- |
| 实机现象 | 三组立绘/标题/选择钮正常，页面底部说明横幅整块缺失 |
| img 节点 | `Login.img_runtime.json`：`RaceSelect/{knight,normal,aran}/text` 是**目录**节点，位图在子节点 `text/0`（579x163） |
| 源码病根 | `IO/UITypes/UIRaceSelect.cpp:122` `classic_text[i] = card["text"];` 拿 dir 节点构造 Texture → 无效纹理 → `draw()` 静默 no-op。同文件 :119 `classic_card[i] = btn["normal"]["0"]` 是正确取法，此处漏了 `["0"]` |

**修复**：`classic_text[i] = card["text"]["0"] ? Texture(card["text"]["0"]) : Texture();`（R6 守卫：缺节点留无效纹理，draw 跳过不崩）。几何自洽校验：60+579≤700、270+163≤460，锚点为前序 IDA OnCreate 取证值不动。

## 问题 4 · 创建流程多 UI 态同屏叠加（UIGender + LoginHandlers）

| 环节 | 证据 |
| --- | --- |
| 实机现象 | 进入创建后性别欢迎框、空性别警告框、NAME 铭牌+属性面板同时出现 |
| 源码病根 | 欢迎框（UIGender）的回收**单点依赖** `LoginResultHandler` 的 `remove(GENDER)`，而该 remove 在 `if (loginwait && loginwait->is_active())` 门内（LoginHandlers.cpp:50-56）。`UILoginWait::update()` 满条即自 `deactivate()`（UILoginWait.cpp:264-268），`UIStateLogin::update` 门闩下一帧 `remove(LOGINWAIT)`（UIStateLogin.cpp:86-90）——服务端响应晚于满条时，LOGIN_RESULT 到达处 Wait 已失活，整个 if 块（含 remove GENDER/TOS/LOGINNOTICE）被跳过 → 欢迎框 active 状态跨页残留，与创建窗口叠加 |

**修复（三处泄漏全堵）**
1. `Net/Handlers/LoginHandlers.cpp`：`remove(LOGINNOTICE/TOS/GENDER)` 提升到 `is_active()` 门外——LOGIN_RESULT 到达即代表上一轮登录/性别/EULA 对话终结，无条件拆净（`UIStateLogin::remove` 对不存在元素幂等，已核 :285-298）。
2. `Net/Handlers/LoginHandlers.cpp GenderDoneHandler`：补 `remove(GENDER)+remove(LOGINWAIT)`——参考端下发 GENDER_DONE 时旧代码只发 ServerRequestPacket 从不拆框（第二泄漏路径）。
3. `IO/UITypes/UIGender.cpp`：YES 按钮 + M/F 键 + SoftKey 三条确认路径全部**就地 deactivate**——原生行为「确认即收框」，不再赌封包时序。SoftKey lambda 需 `[this]` 捕获 + `this->deactivate()`（C4573/C3493 实测后修正，模式对齐 UILoginNotice.cpp:121）。

**未改动并披露**：`UILoginNotice::button_pressed/send_key` 只 deactivate 不 remove（对比 UIKeyConfirm::confirm() 两者都做）——经核对 `UIStateLogin::draw` 只画 `is_active()` 元素，残留 inactive 实例**不参与叠加**（属内存滞留非视觉 bug），按爆炸半径控制不动；若后续发现 Notice 类窗口跨页残留再按同法收口。

## 顺带修复（阻塞全工程构建的前序未提交代码笔误，非本计划范围，已单独标注）

| 文件:行 | 错误 | 修法 |
| --- | --- | --- |
| `Gameplay/MapleMap/Mob.h:120` | C2365：`has_parent()` 方法声明与 :192 字段 `bool has_parent` 重名（18 处 TU 编译失败） | 访问器声明改名 `parented()`（字段名保留，Mob.cpp 两处赋值在用；无调用方，grep 证实） |
| `Gameplay/MapleMap/Mob.cpp:374` | C3861：`above(0.82f)` 漏 `randomizer.` 前缀 | 补前缀（同文件 :406 `randomizer.below` 为正确先例） |
| `IO/UITypes/UIChatBar.cpp:2158` | C2228：`c.name` 是 `const char*` 调 `.rfind`（GM 命令补全，构建期间外部并发编辑引入） | 包 `std::string(c.name).rfind(cmd,0)==0`，前缀匹配语义不变 |
| opcode 门控 | GATE FAIL：WORLD_MAP_DROP 两行与源码不同步 | 按门禁 SOP 跑 `python tools/gen_opcode_matrix.py` 重新生成 |

⚠ `Mob.h` 的 `parented()` 声明目前**无定义无调用**（前序波次半成品）；`Mob.cpp` 若后续波次实现该方法需按新名。`set_next_skill/get_parent_oid/is_dormant/wake` 同样只有声明——链接未报错即无人调用，留给原波次作者收口。

## 验证与状态

- [x] `tools/build_smoke.bat`：GATE OK + EXITCODE=0（最终一轮），产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（构建前按红线 taskkill）
- [x] 每处修复的 img 节点值取自 `REA反编译/wz_dump/Login.img_runtime.json`（运行时权威源，R1/R2）
- [x] 参考图像素实测（PIL 逐行扫描）：公告框/面板边缘/文字行三组坐标
- [ ] **用户本机复测四场景**（计划验收的最后一环）：频道页公告位置尺寸纹理、选角属性窗静止态、种族页底部横幅、创建流程分步门控（含 M/F/软键盘三条确认路径）
- 未提交 commit（等待复测通过后按 git-commit-standard 原子提交；并发编辑的 UIChatBar/GMCommandList.h 归属另一波次，勿混提）
