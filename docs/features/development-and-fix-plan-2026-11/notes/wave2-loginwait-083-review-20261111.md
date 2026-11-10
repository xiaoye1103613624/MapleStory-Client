# round73 · UILoginWait 加载进度条按 Angel 083 CLogin 等待态只读取证复核

## 0. 授权与性质
- 目标原文（m00690 等，goal-11388c42）：以 Angel(13339) 083/v83 带符号 IDB 为权威逐功能校准；门禁 18「REA→IDA→IMG 只读取证与对照清单，交决策点 + 一次性 Plan 后再进入实现」。
- User said (m09859) 硬指令：「若碰到ui布局、动画一定要确认是否修复了，如果不是按照Angel 布局修复过的，可以重新修复；之前有很多参照BeiDou或者自行修复的ui、动画都有些问题」；User said (m11549)：「记录一次有可见 PUA 输出的交付，不记录评分。」
- m04292 授权：改源码/自测/测试完成提交/遇占用直关/不能自测记 backlog。
- 本轮性质：**只读取证 + 对照清单 + 决策点 + 一次性 Plan + 零风险注释口径纠正（0 行为字面量）**。

## 1. 选面（全仓扫描后聚焦）
- 宽扫 `自绘|手绘|兜底|回退|截图 SAD|rs.cpp|ColorBox|半透明` → 50 文件 245 命中（多为正常防御性回退，噪声）。收窄 `自造|截图|SAD|逐格模板|rs.cpp|凭空|本端扩展` → 158 命中，绝大多数是「截图 SAD 自造版式」，正是 m09859 点名类型。
- 选 `UILoginWait.cpp:122-126`：注释自称「2026-11-05 正确端截图逐像素定标」「截图仅见第 0 段 8x8 橙块」，且是登录链路起点 + 动画，与 goal「从登录链路开始」吻合。

## 2. 083 权威（活 IDB 现场反编译，禁凭记忆）
- `*LoginWait*` / `*LoginWaiting*` / `*CUIWait*` / `*Loading*` / `*CUILoading*` / `*CUIConnecting*` / `*CUIWaiting*` / `*WaitDlg*` / `*SetWait*` **全 0 命中** → 083 **无独立 Loading 窗类**，不得凭空给 083 编一个窗口类（门禁 5）。
- 等待态挂在 `CLogin::Update @0x5f4c16 (size 0x752)`：**`if (v6!=0 && v2-v6>10000)` 十秒超时链** → `RemoveNoticeConnecting(this)` + `ResetVAC(this)` + `GotoTitle(this,0,0)` + return（pending==0 分支）；pending!=0 则 `MakeVACDlg` + `ChangeStep(5)`。
- `?RemoveNoticeConnecting@CLogin@@QAEXXZ @0x5fac26 (size 0xe)`：`return (*(**(this+70)+52))(a1:*(this+70), a2:3);`；8 个 xrefs（CLogin::Update / OnViewAllCharResult x6 / MakeVACDlg）。
- `OnStepChanged @0x5f5962` / `OnPacket @0x5f80ff` / `OnCheckPasswordResult @0x5f83ee` 一并落盘 `tools/_ida_loginwait/*.dec.txt`。
- 字符串表：唯一 `*Loading*` 是 `w.img/UserList/Guild/GuildBoard/Loading`（公会看板，无关）；`Login.img/Notice/Loading` 是动态属性路径，不在 exe ASCII 明文里。

## 3. orange-wz AGENTS.md 门禁 + 合规 .nx 回退
- `tools/_lw_probe.py`：endpoint `http://127.0.0.1:10012/mcp`，IMG=`E:/MXD/BeiDou-Client-xy/Data/UI/Login.img`，65536B GMS key（`b64_len 87384` / `sha256_16 268a8407a47ee76c` / `iv TSPHKw==`），`load_files exclusive:false` → `loadedCount=1 rootCount=1 generation=1`（结构层 WORKS）。
- 7 个节点 query（`Notice` … `Notice/Loading/BtCancel`）**全部 `IMG 解析失败: Login.img`**（GBK 原文留证 `tools/_lw_probe_stdout.txt`）；`unload_all rootCount=0 ok=true` → 合规 `.nx` 回退触发条件满足。

## 4. .nx 实测（决定性：当前端用真资源，是反例）
- 真根 = `Login.img/Notice/Loading`（**不是** 顶层 `Loading.img`；todo 初稿路径已现场纠正）。
- `backgrnd` = `bitmap 282x144 origin(0,0)`（卷轴弹窗，含「正在连接到服务器。」+ LOADING 凹槽）。
- `bar/0..10` = **11 帧真位图，每帧 109x8**；0..9 帧 `delay [int] 100`，10 帧无 delay 子节点（`frame_delay()` 按缺省回退 100）。视觉导出 `tools/_lw_visual/bar_{0,5,10}.png`：0 帧仅最左 ~8px 橙可见、5 帧橙→绿半满、10 帧满格 109px。
- `BtCancel/normal/0` = `bitmap 65x23`。
- **结论**：当前端 `pick_loading_anim()` + `bar_frames` 逐帧取的就是这 11 张真位图，配合真 `backgrnd` / 真 `BtCancel` —— **不是「自行修复/参照 BeiDou 的手绘兜底」，是同族病里的反例**。按 m09859 逐面确认后应保留，不照搬前几轮重修。

## 5. 对照清单与真实缺口
| 维度 | 083 | 当前端 | 判定 |
| --- | --- | --- | --- |
| 资源 | `Login.img/Notice/Loading`（动态路径，无 ASCII 明文） | 同根 + 11 真帧 | ✅ 一致 |
| 窗类 | 无独立类，CLogin 状态机 | `UILoginWait : UIElement`（Journey） | 架构差异，校准≠改回原生 |
| bar 几何 | 无布局立即数 | `(131,98)` 截图 SAD 量出 | 截图派生，真机像素核对（PLT 39-1）|
| 循环/停留 | 不可表达 | Journey 循环至 begin_finish | 本端设计；旧注释「参考端」冒称权威已纠 |
| **10s 超时** | `v2-v6>10000 → RemoveNoticeConnecting + GotoTitle` | **无**（服务端不回包则永久循环）| **真缺口 → 决策点 D-D** |
| 注释口径 | n/a | 把 109x8 整帧误读成「8x8 橙块」、拿截图当 083 权威 | ✅ 已纠（纯注释）|

## 6. 决策点 D-D + 一次性 Plan（交用户裁决，本轮不实施）
- D-D：是否给 `UILoginWait` 补 083 的「10 秒未响应自动 `RemoveNoticeConnecting`→回标题」超时。属交互/行为级，会改变当前「等到服务端响应才切页」的语义（s9 登录慢时可能误回标题），需用户裁决 + 真机验证。
- 一次性 Plan（裁决通过后）：① `UILoginWait` 增 `wait_elapsed_ms` 累加 `Constants::TIMESTEP`；② 达 10000 且未 `finish_pending` → `close()` 等价（reconnect/回登录页，按用户选定语义）；③ `begin_finish()` 清零/停用计时；④ harness 补超时场景 + 真机 5 屏；⑤ 单独原子提交。**本轮零实施**。
- 保留项：`IO/LoginBackground.cpp:77` 线性插值 `static_cast<double>(elapsed)/static_cast<double>(tween_duration)`（删 smoothstep，引 Angel 083 CLogin `v62=7000*(v63-a2)/v62/2`）经复核确属按 Angel 修复过，保留不动，并写进 harness 不回归守卫。

## 7. 本轮补丁（纯注释，0 行为字面量）
- `IO/UITypes/UILoginWait.cpp`：① `:122-126` 整块换成 083 依据（CLogin::Update 10s 链 / RemoveNoticeConnecting 8 xrefs / .nx 11 帧 109x8 delay100 / 真资源反例 / (131,98) 截图派生不作 083 背书 / D-D）；② `:254` `:257` `:277-278` 三处「参考端」冒称改为「本端 Journey 设计，083 无此窗类仅作对照」。
- `IO/UITypes/UILoginWait.h`：`begin_finish` 注释 + `BarFrame` 注释补 083 真符号与 109x8 真帧规格。
- `git diff --check` 干净；行为字面量（`kBeiDouBarOff(131,98)` / `fill_total` / `finish_pending` / `TIMESTEP` / `BtCancel` / 11 帧收集循环）逐字不动。

## 8. 提交 scope（7 文件，与 commit 清单逐字一致）
`UILoginWait.cpp` / `UILoginWait.h` / `tools/test_round73_loginwait_083.py` / `evidence/loginwait-083-round73.json` / `notes/wave2-loginwait-083-review-20261111.md` / dev-plan README §4 2-28 / PLT §39。
一次性脚本（`tools/_lw_ida.py` / `_lw_extract.py` / `_lw_probe.py` / `_lw_deliver.py`）、`tools/_ida_loginwait/*`、`tools/_lw_visual/*`、`tools/_lw_probe_stdout.txt` 仅本地证据链留存，不进版本库；evidence JSON 已汇总路径可追溯。

## 9. 教训
1. 门禁 18 的「截图是备选」是硬的：083 无独立 Loading 窗类时，**不能**把截图 SAD 量出的坐标叫「083 布局」，只能标「截图派生、真机待核」。
2. 反例也必须取证后才敢判「保留」：先看 `.nx` 真帧 109x8 / 11 帧 / delay100，才敢说「这不是手绘兜底」，避免把好的实现当同族病误改。
3. `bar/0` 帧「仅 8px 橙可见」不等于「整个 bar 是 8x8」——整帧画布 109x8，旧注释的措辞把「可见像素」偷换成「帧尺寸」，是依据链里的读数错。
4. 真正的 083 差异不在版式而在状态机：10s 超时缺口要交决策点，不因「看起来小」就夹带进行为零风险补丁（校准≠改回原生 / s9 优先）。
