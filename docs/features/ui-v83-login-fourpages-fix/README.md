# 登录链路四页 UI 错乱修复（2026-11-03）

> 对应计划：CodeBuddy plan `60977dd7d7f240e6ad0634d9bf1fa42b`。
> 方法：每处修复走「实机截图现象 → img 运行时节点值 → 源码行」三步取证后改码（R1/R2/R6）。
> 构建：`tools/build_smoke.bat` GATE OK + EXITCODE=0，产物直写 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`。

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
