# PLAN-R28 —— 频道窗版式实测修正 / 学院窗无端弹出 / 目录层级 / 系统设置全量真生效 / 思源宋体替换

> 起因：R27-B7 交付后用户第三轮验收反馈（原话）——
> 「频道更换ui还是不对，点击"更换频道"怎么打开学院ui了？；系统设置ui也是错乱的，系统设置内部设置功能我没有测试，你要确认一下设置是否真的实现了；"目录"按钮点击后出现的"主菜单"ui层级有时候不对，会被遮挡；换开源字体」
>
> 前置：[PLAN-R27.md](PLAN-R27.md)、[b7-4-settings-split.md](evidence/w7-option-settings/b7-4-settings-split.md)、[channel-layout.md](evidence/w7-option-channel/channel-layout.md)
> 原生对照帧：`C:\Users\11036\Desktop\正确客户端截图\`（频道变更 / 选择频道页 / 选中频道页 / 系统设置 / 游戏设置）
> 原生 RE 取证：095 dump（`E:\MXD\Maplestory95.i64`，IDA 会话 `3c1162a2`）

---

## 0. 目标与非目标

### 目标
1. `UIChannel`（游戏内「频道变更」窗）版式与原生帧**逐格**对齐（不再只是"窗能开、格能点"）。
2. 消除"点频道变更却弹出学院窗"这类**协议回包无端开窗**的行为。
3. 「目录」→「主菜单」（`UIGameMenu`）不再被其它窗遮挡。
4. **系统设置 9 项全部真生效**，且生效语义按原生 RE 证据实现，不是"落盘即算实现"。
5. 非位图文字改用开源 CJK 字体（思源宋体，SIL OFL），并**删除仓库内 SimHei**（授权风险 + R27 遗留项 #11）。

### 非目标（本轮不做，理由见 §6）
- 095 口令泄露处置（用户 2026-11-03 明示「095 的暂时不管」）⇒ 任务 #10 保持挂起，不换钥、不改写历史、不强推。
- `ChangeChannelPacket` 那 4 字节占位字段的真机语义（服务端不读，无证据）。
- MapleComboBox「展开列表」的通用控件抽象（本轮只为系统设置两个下拉补列表，见 B4-8）。
- 游戏内鼠标**外观贴图**、系统级鼠标加速（原生不动 OS 设置，本包也不动）。

---

## 1. 已确认决策（本轮用户拍板 + 一条按新规纠正的偏差）

| # | 事项 | 用户决策 | 落地口径 |
| --- | --- | --- | --- |
| D1 | 系统设置内部功能 | **「全部都需要实现」** | B4 九项全接真管线，不允许"仅落盘" |
| D2 | 字体 | **「思源宋体（SIL OFL）」**；范围**「全替 + 删 SimHei（推荐）」** | B5 |
| D3 | 开窗处理器 | **「全部改成『窗已开才更新』（推荐）」** | B2 四处处理器 |
| D4 | 怪物情报 | **「三档都做，接受 HP 陈旧」** | B4-6，含"只读缓存值、不补协议"的显式声明 |
| D5 | 鼠标速度 / 画质 | 用户立的**更高优先规则**：「类似这些决策的最好先找一下原生客户端 rea 是否有这些逻辑，如果有就按照它实现，没有再找我抉择」 | ⇒ 见下方偏差说明 |
| — | 窗口占用策略 | 「等它自己关，我重试链接」 | 构建/部署不结束并发会话进程 |

### ⚠ 与用户先前选项的一处**有意偏差**（必须先说明）
- 我曾给「画质」的选项是 **"接 VSync + 特效密度"**，用户当时选它。
- 但按 D5 规则复查原生 RE，**原生「画质调整」不是 VSync、也不是特效密度**：它是 **0..3 档地图层 LOD**（`GetVideo_MagLevel` → `SetFieldMagLevel`，仅在点 OK 时生效，`bApplyVideo`）。
- 故 **B4-2 按原生实现**（层 LOD），`window.vsync` 保持为它自己已有的独立配置项，不并入画质。
- 同理「画面效果」开关在原生是**技能特效**（`EffectLayer`），与画质是两件事 ⇒ B4-3 单列。
- 鼠标速度：用户的推断（「应该影响的是游戏内的鼠标移动速度，不影响客户端外的操作」）**与原生一致**——原生只做客户端窗口内的光标增益（`(n+5)/10`，0..19，限制在客户区，仅窗口化时才回写光标位置），不改系统鼠标设置。⇒ B4-1 按此实现，客户区内相对增益，不触碰 OS。

---

## 2. 事实依据（全部可复核，无推断）

### 2.1 频道窗（B1）
- 原生帧 `频道变更.png` 实测 **428×122**；空频道板 x 区间 `216..282 / 286..352 / 356..422` ⇒ 板宽 **68**、板间距 **70**、**板 x = 6 + 70·i**、 band y = `67..84`。
- 本包现状 [IO/UITypes/UIChannel.h:58-63](file:///E:/project/MapleStory-Client/IO/UITypes/UIChannel.h#L58)：`SLOTS_PER_ROW=6, GRID_TOP=67, SLOT_PITCH_X=70, SLOT_PITCH_Y=20, SLOT_LABEL_DX=8, SLOT_LABEL_DY=5`；[UIChannel.cpp `slot_x()`](file:///E:/project/MapleStory-Client/IO/UITypes/UIChannel.cpp) 基址写的是 **9** ⇒ 整体右偏 3px（3 格累计让最右板溢出右缘）。
- 另两处已目视确认的差异（`E:/project/_r28_chan_cmp.png` 并排）：世界条比原生**高约 4px**；原生世界条左端的**蜗牛图标**本包没画。

### 2.2 学院窗（B2）
- [Net/Handlers/WaveAShellHandlers.cpp:284-285](file:///E:/project/MapleStory-Client/Net/Handlers/WaveAShellHandlers.cpp#L284)：`FamilyInfoHandler` 取不到 `UIFamily` 就 `emplace`，取到但没开就 `makeactive()` ⇒ **服务端每次登录都发的 opcode 95（`FAMILY_INFO_RESULT`，[Net/PacketSwitch.cpp:170](file:///E:/project/MapleStory-Client/Net/PacketSwitch.cpp#L170)）会无条件弹学院窗**。
- 同文件 `:369-370`、`:414-415` 同模式；[Net/Handlers/MessagingHandlers.cpp:902/945/982](file:///E:/project/MapleStory-Client/Net/Handlers/MessagingHandlers.cpp#L902)（升级 105 / 结婚 106 / 转职 107）为"该发一条聊天文字"而弹 UIGuild/UIFamily 整窗。
- R27-B7-3 的"PASS"是**我自己的误判**：抓帧 `_r27_b73c_06_after_change.png` 里学院窗就压在频道窗上，我的驱动脚本随后按 ESC 把它关了，症状被隐藏。本条按"元素在位 ≠ 版式修好"的既有教训重新定标自证判据。

### 2.3 目录层级（B3）
- [IO/UIStateGame.cpp:757](file:///E:/project/MapleStory-Client/IO/UIStateGame.cpp#L757) `pre_add` 只把 **`is_toggled()`** 的元素移到 `elementorder` 末尾；`UI::makeactive()` 从不重排 ⇒ 非 toggled 窗永远抬不到前面。
- [IO/UITypes/UIMiscShells.h:594-596](file:///E:/project/MapleStory-Client/IO/UITypes/UIMiscShells.h#L594)：`UIGameMenu : UIDragElement<PosNOTICE>`，`FOCUSED=true`、**`TOGGLED=false`** ⇒ 正是会被遮挡的那个。

### 2.4 系统设置消费端现状（B4）
- 诚实表已在 [UIOptionMenu.cpp:655-660](file:///E:/project/MapleStory-Client/IO/UITypes/UIOptionMenu.cpp#L655)：只有 **背景音乐 / 效果音响 + 两个静音**接了 `Audio`；其余**仅按 `ui.sysopt.*` 落盘**。
- 配置项齐备：`SysOptQuality` [Configuration.h:792](file:///E:/project/MapleStory-Client/Configuration.h#L792) / `MouseSpeed:799` / `HpWarn:806` / `MpWarn:813` / `SkillEffects:820` / `WindowMode:828` / `AltEnter:835` / `MobInfo:842` / `ScreenshotPath:849`；另有**重复**的 `Fullscreen:456` 与 `VSync:477`。
- 可插入点（已逐个读到行）：
  - 光标：[IO/Window.cpp:639-640](file:///E:/project/MapleStory-Client/IO/Window.cpp#L639) `case SDL_EVENT_MOUSE_MOTION: send_cursor_position(event.motion.x, event.motion.y)`（SDL3，**客户区相对坐标**，正是原生增益的施力点）。
  - 层 LOD：[Gameplay/Stage.cpp:140-150](file:///E:/project/MapleStory-Client/Gameplay/Stage.cpp#L140) 的 `for (auto id : Layer::IDs)` 循环里同时画 mobs/npcs/chars/player ⇒ **禁止在循环本体加门禁**；安全点是 [MapTilesObjs::draw:62-65](file:///E:/project/MapleStory-Client/Gameplay/MapleMap/MapTilesObjs.cpp#L62)（逐层入口）与 `MapBackgrounds::drawbackgrounds`。
    背景侧的既有缺陷：[MapBackgrounds.cpp:173-192](file:///E:/project/MapleStory-Client/Gameplay/MapleMap/MapBackgrounds.cpp#L173) 建表时**丢掉了层号 `no`**，只留 `front/back` 两个扁平 vector ⇒ 按档裁层前必须先把 `no` 存进 `Background`。
  - 特效：[Graphics/EffectLayer.cpp:49-64](file:///E:/project/MapleStory-Client/Graphics/EffectLayer.cpp#L49) 四个 `add()` 重载 ⇒ 单一早退点。
  - HP/MP 闪：[UIStatusBar.cpp:1915-1922](file:///E:/project/MapleStory-Client/IO/UITypes/UIStatusBar.cpp#L1915) 已有 `gauge["hpFlash"] / gauge["mpFlash"]` sprite，`:2356-2366` 已有"值一变就闪 5 帧"的计时器 ⇒ 警报只差**阈值条件**，不需新资源。
  - 怪物名牌：[Gameplay/MapleMap/Mob.cpp:404-410](file:///E:/project/MapleStory-Client/Gameplay/MapleMap/Mob.cpp#L404) `if (showhp) { namelabel.draw(); hpbar.draw(); }` 是唯一门禁。
  - 窗口化：`Window::toggle_fullscreen()` 已存在；`Fullscreen`(:456) 与 `SysOptWindowMode`(:828) 是**两套真源**，需合一。
  - 截图：`KeyAction::Id::SCREENSHOT` 已在 [IO/KeyAction.h:71](file:///E:/project/MapleStory-Client/IO/KeyAction.h#L71) 且默认绑 SCROLL_LOCK（[UIKeyConfig.h:144](file:///E:/project/MapleStory-Client/IO/UITypes/UIKeyConfig.h#L144)），但**分发处 `UIStateGame.cpp:248-483` 的 switch 里没有它的 case** ⇒ 按下无反应。`includes/stb/stb_image_write.h` 在库但**无实现 TU**。
  - 下拉：[UIOptionMenu.cpp:854-858](file:///E:/project/MapleStory-Client/IO/UITypes/UIOptionMenu.cpp#L854) 只 log "expanded list not implemented"。

### 2.5 字体（B5）
- 两条路径：[Configuration.h:527 / :534](file:///E:/project/MapleStory-Client/Configuration.h#L527) 默认 `fonts/SimHei/simhei.ttf`；FreeType-only，`Text::Font` 12 slot → 2 face，CJK 按需生成，图集上限 `WIDE_CELL = 22`，每字体 484 字形缓存，行高自动 `height*1.35+1`。
- **手抄 `bitmap_top` 常数**是 R27-B1 的成因，现存 6 处：[Graphics/Text.h:131-135](file:///E:/project/MapleStory-Client/Graphics/Text.h#L131)、[Graphics/GraphicsGL.h:79](file:///E:/project/MapleStory-Client/Graphics/GraphicsGL.h#L79)、[GraphicsGL.cpp:1422](file:///E:/project/MapleStory-Client/Graphics/GraphicsGL.cpp#L1422)、[UIChatBar.h:219](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.h#L219)、[UIStatusBar.cpp:1896](file:///E:/project/MapleStory-Client/IO/UITypes/UIStatusBar.cpp#L1896)、[UIStatsInfo.cpp:213](file:///E:/project/MapleStory-Client/IO/UITypes/UIStatsInfo.cpp#L213) —— 换字体后这些数值**全部失效**。

---

## 3. 任务分解

> 顺序即依赖顺序。每批 = 一次实现 + 一次自证 + 一次原子提交（pathspec 只提本批文件）。

### B1 —— UIChannel 版式逐格对齐（小、先行，为 B2 让开验证通路）
**改**：`slot_x` 基址 9→6；由原生帧重新标 `SLOT_LABEL_DX/DY`；世界条高度减 4px；补世界条左端蜗牛图标（资源取自 `Channel.img` / `UIWindow.img` 对应节点，先 `find_node` 取证再画，不猜节点名）。
**判据**：与 `频道变更.png` 并排（`_r28_chan_cmp.png` 方法），三格空板 x 落在 `216..282 / 286..352 / 356..422`，误差 ≤1px；世界条上下缘对齐；蜗牛可见。
**验证**：真机抓帧（已跑通的 `_drive.py` / `_cap2.py` 链，登录→进图→ESC→目录→频道变更），叠图比对，非"元素在位"。

### B2 —— 回包不再无端开窗（4 处）
**改**：`FamilyInfoHandler`、`:369`、`:414`（FamilyJoinResult）三处 + `NotifyLevelUp/NotifyMarriage/NotifyJobChange` 三处，统一为
`get_element<T>()` → **仅当 `ui && ui->is_active()` 才 `set_info/update`**，否则只更新数据层（家族/公会信息仍要存，开窗时自取）。
升级/结婚/转职三条**改为聊天栏一行提示**（走既有 i18n 文案，禁止烘焙位图）。
**判据**：冷启动登录链全程抓帧，`UIFamily`/`UIGuild` 不出现在任何未主动打开的帧里；手动开学院窗后信息仍完整（服务端数据没丢）。
**副作用面**：只改 handler 的开窗条件，不改协议收发 ⇒ 不动 opcode 门禁。

### B3 —— 激活即置顶
**改**：在 `UI::makeactive()` 的 **false→true 跃迁处**把该 type 移到 `elementorder` 末尾（新增 `UI::bring_to_front(type)`）。**不改** `pre_add` 的 toggled 分支语义（那是每帧抬升，改了会让所有活动窗互相抢位）。`UIGameMenu` 的 `TOGGLED=false` 保持。
**判据**：先开装备窗/聊天窗，再按 ESC→点目录，主菜单必须完整压在上面；连开两次不抖动、不闪烁。
**回归**：抽 3 个 toggled 窗（底栏、小地图）确认层级行为未变。

### B4 —— 系统设置九项真生效（按原生语义）
> 全批共用一个窗体测量前提：`SysOpt` 版式已按原生帧还原（R27-B7-4），本批只补行为与两处必补的 UI（下拉展开、勾框命中）。

- **B4-1 鼠标速度**：`Window.cpp:639` 对客户区相对位移乘 `(n+5)/10`，`n` 由 `ui.sysopt.mouse_speed`(0..100) 映射到原生 0..19；结果 clamp 在客户区内；**不回写系统光标、不改 OS 设置**（与用户判断一致）。 fullscreen 下只做 clamp 不做 warp（对齐原生"仅窗口化 SetCursorPos"）。
  - ⚠ 默认值提醒：原生滑钮实测在 52%，按该公式增益≈1.5×。实现后若手感明显偏快，我会回报并给出"以 26% 为中位"的备选映射，等你一句定夺，不擅自改默认。
- **B4-2 画质（原生 0..3 层 LOD）**：`ui.sysopt.quality`(0..100) → `mag_level 0..3`；门禁放在 `MapTilesObjs::draw(layer,…)` 与 `MapBackgrounds::drawbackgrounds`（**先给 `Background` 补回层号 `no`**）；越界层跳过绘制，**实体（mob/npc/char/player/drop/reactor）一律不裁**；仅点 OK 时生效（同原生 `bApplyVideo`）。VSync 留在 `window.vsync` 不动。
- **B4-3 画面效果（技能特效）**：`EffectLayer::add()` 早退；关闭时仍保留必要状态类特效（无敌/异常状态可视化不能吞，逐调用点判定，不是一刀切）。
- **B4-4 窗口化 / ALT+回车**：`SysOptWindowMode` 与 `Fullscreen` 合一（以 `ui.sysopt.window_mode` 为唯一真源，`Fullscreen` 降级为只读别名，避免双写打架）；复用 `Window::toggle_fullscreen()`；`SysOptAltEnter` 为真才响应 ALT+ENTER（在 `Window.cpp` KEY_DOWN 分支加拦截）。
- **B4-5 HP/MP 警报**：`UIStatusBar` 现有闪帧逻辑加阈值条件——`hp*100/maxhp <= hp_warn 且 hp_warn>0` 才闪（0=「没有」）；复用 `gauge/hpFlash`、`gauge/mpFlash`。**原生是否有警报提示音**：本机资源里无对应 Sound 节点，做显式预留并在文档写明。
- **B4-6 怪物情报三档**：`ui.sysopt.mob_info` → `Mob::draw` 的门禁拆成 `namelabel` / `hpbar` / 名称+HP条 三档（原生下拉序数与本包三档的对应，以 `UIWindow.img/SysOpt` 里烤死的下拉文案 + 095 的 NameTag 分支为准，实现前先取证）；HP 取现有缓存 ⇒ **允许陈旧**（用户已接受）。档序文案走 i18n。
- **B4-7 截图**：新增 `Graphics/Screenshot.cpp`（`STB_IMAGE_WRITE_IMPLEMENTATION` 唯一落点）+ `glReadPixels` 回读 → PNG；在 `UIStateGame.cpp` 的 switch 补 `case KeyAction::Id::SCREENSHOT`；`SysOptScreenshotPath` 从"序号"改为**可用的目录串**（默认客户机目录下 `Screenshots/`，下拉只做「MapleStory 文件夹 / 文档文件夹」两项）。文件名含时间戳，成功/失败各留一条上线级日志。
  - ⚠ 需改 `MapleStory.vcxproj` 加源文件 —— 与并发会话可能同文件冲突，我会**只追加一行、提交前 `git status` 复核**。
- **B4-8 两个下拉可展开**：把 `draw_combo` 的 `ComboBox/normal[0..2]` 接成真列表（点合→展、选项高亮、选中即收起并写值），命中区按展开态实际矩形（现在只有按钮条热区）。这是通用控件缺失，本轮**只为这两个下拉实现**，抽象留到下轮。

### B5 —— 思源宋体全替 + 删 SimHei
1. 取字：Source Han Serif SC（思源宋体）Regular + Bold 官方发行件（SIL OFL，随包 LICENSE/OFL 文本入库）；实现前用搜索确认官方发布页，不猜 URL。
2. 落 `fonts/SourceHanSerifSC/`，改 `Configuration.h:527/:534` 默认路径；删 `fonts/SimHei/simhei.ttf`（9.3 MB，微软系统字体，公开仓库授权风险 = 遗留项 #11）。
3. **不重标像素常数，而是消灭它们**：§2.5 列的 6 处手抄 `bitmap_top` 统一改为经 R27-B1b 的 `Text::y_for_ink_top()`（运行时从 FreeType 取真值）取，注释保留为"历史成因"一句。
4. 校 `WIDE_CELL=22`：宋体在 11/12/14px 的最宽 CJK 若超 22 需上调并复测图集装箱（484 字形上限）。
5. 回归重点：金币数值右对齐、底栏职业名、状态栏数值行、装备 tooltip 属性行——R27-B1 的同类偏移复发即判失败。
6. ⚠ 体积：两款 OTF 约 30–40 MB 入库（比 SimHei 大 3–4 倍）。若你嫌重，备选是只留 GB 常用字子集（约 5 MB），但会丢生僻字道具名/玩家名 —— 默认按**全字库**做，改不改等你一句话。

### B6 —— 门禁、回归、归档
- `python tools/gen_opcode_matrix.py --check`（当前对 8 处 `client_sent_at` 报 `GATE STALE (not blocking)`，属并发会话在飞的 `CHANGE_CHANNEL/39` 引用，不视为本批失败）。
- 构建：MSBuild Release x64；**默认增量 Build**。若 B4 改了公共头文件成员布局（`UIOptionMenu.h` / `Background` / `UIChannel.h`）出现运行期异常，按 [msbuild-header-layout-stale-objs] 教训升 `-t:Rebuild` —— 但全量 Rebuild 会连带编译并发会话**未提交的公开头改动**，届时**先停下来问你协调窗口**，不擅自触发。
- 回归抓帧清单：登录→选世界→选频道→进图→ESC 目录→频道变更（换频两次）→系统设置九项逐项改→截图键→装备/状态/聊天三窗字体位。
- 产物：`evidence/w8-channel-layout-fix/`、`evidence/w8-window-zorder/`、`evidence/w8-sysopt-live/`、`evidence/w8-font-swap/`（含每格比对表 + 095 RE 摘要脱敏）；本文件补「最终实现逻辑」小节；`CHECKLIST.md` 属并发会话在编辑，**不动**，进度写在本 PLAN 的批次勾选上。
- 提交：B1/B2/B3/B4(可拆 2 笔)/B5 各一次原子提交，只用显式 pathspec；提交前 `git log -1` + `git status` 复核，凭据扫新增行；**不 push**（本轮 7 笔历史均未获 push 授权）。

---

## 4. 涉及文件预估

| 批 | 文件 |
| --- | --- |
| B1 | `IO/UITypes/UIChannel.{h,cpp}` |
| B2 | `Net/Handlers/WaveAShellHandlers.cpp`、`Net/Handlers/MessagingHandlers.cpp`、`locale/zh-CN.ini`、`locale/en-US.ini` |
| B3 | `IO/UI.{h,cpp}`（`bring_to_front`）、必要时 `IO/UIStateGame.cpp` 只读确认 |
| B4 | `IO/Window.cpp`、`IO/UITypes/UIOptionMenu.{h,cpp}`、`IO/UITypes/UIStatusBar.cpp`、`Gameplay/MapleMap/Mob.cpp`、`Gameplay/MapleMap/MapBackgrounds.{h,cpp}`、`Gameplay/MapleMap/MapTilesObjs.cpp`、`Graphics/EffectLayer.cpp`、新增 `Graphics/Screenshot.{h,cpp}`、`Configuration.h`、`IO/UIStateGame.cpp`、`MapleStory.vcxproj`、locale |
| B5 | `fonts/SourceHanSerifSC/*`、删 `fonts/SimHei/*`、`Configuration.h`、`Graphics/Text.{h,cpp}`、`Graphics/GraphicsGL.{h,cpp}`、`IO/UITypes/UIChatBar.h`、`UIStatusBar.cpp`、`UIStatsInfo.cpp`、`docs/features/ui-v83-window-restore/tools/measure/*` |
| B6 | 文档与 evidence |

**不碰**（并发会话在写）：`UIEquipInventory.{h,cpp}`、`UIStatusBar.cpp` 之外… 具体为 `Icon.{h,cpp}`、`UIItemInventory.cpp`、`UIKeyConfig.cpp`、`UISkillBook.{h,cpp}`、`UIWorldSelect.cpp`、`ChatBalloon.cpp`、`Gauge.cpp`、`UIMapInfoPanel.cpp`、`Net/Packets/CashShopPackets.h`、`Net/PacketSwitch.cpp`、`Net/SocketPosix.cpp`、`Net/KeyMapTranslate.h`、`Net/Handlers/PlayerHandlers.cpp`、`Net/Packets/PlayerPackets.h`、`notes/open-issues.md`、`CHECKLIST.md`、`s9-*` / `s10-*` / `s11-*` 文档。
> 注：B4-5/B5 需动 `UIStatusBar.cpp`（并发会话也在改）。开工前先看该文件是否处于 modified 状态；若冲突，**该子项先停下问你**，不抢改。

---

## 5. 验证门禁（每项都要落到"用户会看的那一档"）

| 批 | 自证判据（= 验收判据） |
| --- | --- |
| B1 | 与原生帧叠图/并排，格位误差 ≤1px；不是"窗能开" |
| B2 | 冷启动全链抓帧中无学院/公会窗；手动开窗信息完整 |
| B3 | 遮挡场景实机复现后主菜单在顶层（这条**需要你上手 20 秒**复核一次，因层级问题对前台焦点敏感） |
| B4 | 每项**改变量→看现象**：光标速度手感、远层背景消失/出现、特效开关、窗口化切换、HP 低于阈值才闪、怪物名牌三档差异、`Screenshots/` 里真出现 PNG、下拉真能选 |
| B5 | 帧比对 4 处历史偏移位；`git ls-files fonts/` 无 simhei；图集无缺字（抽 20 个生僻道具名） |
| B6 | 构建 0 error；opcode 门禁不新增失败项；临时调试日志已删（`GraphicsGL.cpp:35 dbgatlas()` 等）；凭据扫新增行 0 命中 |

---

## 6. 风险 / 预留 / 需要你裁决的时机

1. **共享工作树并发提交**：同 exe、同 OutDir。策略：只用显式 pathspec 提交；不结束对方进程（按你的选择等它自己关）；构建失败若源自对方文件，不代改。
2. **全量 Rebuild 的连带风险**：见 B6。可能被迫停在"增量过了但布局存疑"，届时把证据摊给你选。
3. **鼠标速度默认值**（B4-1 ⚠）：公式照原生，默认 52% ⇒ 约 1.5×。若你实测觉得快，我给两种映射供选，**不擅改**。
4. **画质档↔层号映射**：原生 `SetFieldMagLevel` 的具体层号阈值表，095 dump 里能读到函数但阈值需再取证；取不到时按"档位越高、裁掉的远层越多"的单调实现 + 显式写明与原生可能差 1 档。
5. **字体体积**（B5 ⚠）：默认全字库 ~30–40MB 入公开仓库。
6. **截图目录语义变更**：`ui.sysopt.screenshot_path` 由 Byte 序号改为 String 路径 ⇒ 存档兼容按"旧值 0/1 映射到新默认目录"处理，不报错。
7. **怪物情报 HP 陈旧**：不补协议，接受显示滞后（你已确认）。
8. **095 口令**：本轮不动（你已明示）；等你要接 095 新功能时再提。
9. 遗留待议（不在本轮）：目录气球二次点击是否收起、MapleComboBox 通用化、退出路径不持久化窗口位置、僵尸 `MapleStory.exe` PID 34476、服务端不建 Messenger 房间。

---

## 7. 开工信号

以上无需再抉择的项我已按原生/你的决策定死；**回复"开工"即按 B1→B6 顺序执行**，每批完成自证后继续下一批，最后把「需要你上手的验证项」（B3 层级、B4 手感类）汇总成一张单子给你一次跑完。
