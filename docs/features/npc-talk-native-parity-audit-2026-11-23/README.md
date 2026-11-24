# NPC 对话框内容渲染 + 按钮交互逻辑对照审计（2026-11-23）

> 范围：`IO/UITypes/UINpcTalk.cpp`（2043 行）+ `Net/Handlers/NpcInteractionHandlers.cpp` + `Net/Packets/NpcInteractionPackets.h` + `locale/*.ini`
> 对照源：MinMS（JCC/Cosmic fork）、v95 GMS IDB（port-095 证据，仅语义参考）、192 客户端（资源侧）、BeiDou V17.7 IDA（本端原生基准，a11 系列证据）
> 结论：**逐类型核对后，绝大多数渲染/交互与原生契约一致；发现 2 处真实缺陷并已修复。**

---

## 一、对照源可信度分级（先定证据边界）

| 对照源 | 性质 | 可用性 |
| --- | --- | --- |
| BeiDou V17.7 IDA（`BeiDou.exe.i64`，本端原生基准） | 原生逻辑证据 | **权威**（a11b/a11h/a11i 等，地址 0x9Axxxx/0x74xxxx） |
| MinMS `UINpcTalk.cpp`（JCC fork，4442 行） | 开源 fork，非原生 | 参考（1.3x 缩放、无打字机/无 AskBoxText 属 fork 定制，不作原生判据） |
| v95 GMS（`Maplestory95.i64`，port-095） | 语义参考 | 有限（响应包构造可引；与国产 095 无关，禁止作 v83 结论） |
| 192 客户端 | 资源侧 | 仅 UtilDlgEx 节点树；二进制加壳不可逆向逻辑 |

**关键提醒**：三份外部对照**都不是**本端生产目标的原生客户端（本端 = BeiDou V17.7 / s9 服务端）。原生判据以 `docs/features/npc-talk-utildlgex-align/evidence/a11*.md` 的 BeiDou V17.7 IDA 为准；MinMS/v95/192 仅交叉旁证。

---

## 二、渲染逻辑逐项结论（行号级）

| 项 | 本仓实现 | 原生/对照 | 结论 |
| --- | --- | --- | --- |
| 帧绘制 top/fill/bottom | `draw():189-203`，fill 按 c 纹理原始高度竖向平铺（cph=fill.height，末片压缩） | 老 Journey(Heaven/cms085) 是 `fill.draw(DrawArgument(pos, (0,height)))` 拉伸；MinMS fork 也是拉伸；a11c 证据要求平铺保 20px 噪声周期 | ✅ 与本端 a11c 校准一致（平铺优于老拉伸，是有意修正） |
| 正文位置 | `TEXT_X=166 / TEXT_WIDTH=310`（头文件 :124-125），`draw():222/228` | a 三帧逐列量暗字边界（左163..170/右474..476）；原生 `text left≈162` | ✅ 一致（166+310=476 贴右缘，避开确定钮） |
| 正文垂直定位 | `change_text():1420-1428`，装得下 `(height-text_h)/2+t.height+7`；滑条贴顶+7 | IDA sub_9A64FC 同公式 | ✅ 一致 |
| 打字机逐字显示 | `update():326-383`，按 UTF-8 码点整段喂入，令牌原子 | 原生有逐字显示；MinMS fork 无（fork 差异，不采纳） | ✅ 一致且更稳（CJK 不拆半） |
| 打字机点击跳过 | `send_cursor():684-688`，命中 CLICKING 且 draw_text → 整段显示 | 原生点击立即显示全文 | ✅ 一致 |
| NPC 立绘 | `change_text():1364-1372` stand/0；`draw():211-215` 不镜像 | a11 证据：希娜原图朝右，不应翻 | ✅ 一致（注释记录过 flip bug 已修） |
| 名牌 bar | `draw():207-215`，speaker 居中 + name A11M WHITE | 原生 bar + A11M 白 | ✅ 一致 |
| SENDSIMPLE 选项高亮 | `draw():231-269` 黑色实线+#919191 虚线（3 on/3 off），宽=标签+8；墨迹对齐 | IDA sub_9A3E9F（a11d）异曲同工；MinMS fork 用半透明色块（fork 差异，不采纳） | ✅ 一致（a11d 校准） |
| 输入槽（GetText/Number/BoxText） | `draw():271-310` 黑框+白底+Edit；Avatar 无框；BoxText cols/rows 定尺寸 | IDA sub_9A83F6/9A8910/9ACC20 | ✅ 一致（a11e/a11f/a11j） |
| AskAvatar 预览 | `draw():167-187` backgrnd 419×306 + CharLook + nameTag | IDA sub_9A3CED/9AC92C（a11i） | ✅ 一致 |
| 滑条 | `change_text():1392-1411`，offset 每行 18px（A12M 行距），rowmax=(over+step-1)/step | 原生无极滚动；老实现按页跳（已修） | ✅ 一致 |

---

## 三、按钮交互逐项结论（行号级）

| 项 | 本仓实现 | 原生/对照 | 结论 |
| --- | --- | --- | --- |
| Say OK/Next→(lastmsg,1) | `button_pressed():466-469` | v83/v95/BeiDou 全一致 | ✅ |
| CLOSE(Say/NEXT/PREV/YesNo/Accept)→(lastmsg,-1) | `:464/:479/:495/:514/:537` | 原生 DoModal==close；MinMS close()=(type,-1)；s9 nextLevel:221 mode==-1 dispose | ✅ |
| NO→0 / YES→1 | `:516-527` | 一致 | ✅ |
| AcceptDecline YES/NO | `:532-552`（复用 YesNo 槽，BtYes/BtNo） | a11 证据：BeiDou 无独立 ACCEPT OnCreate | ✅ |
| GetText OK→(2,1,string) | `dispatch_input_field():1850` + packet.h:95 | v83/v95 一致 | ✅ |
| GetNumber OK→(3,1,int) | `:1905` + packet.h:108 | 一致 | ✅ |
| AskBoxText OK→(13,1,string) | `:1835` + packet.h:128 | a11j 一致 | ✅ |
| Simple 点#L→NpcTalkMorePacket(selection) | `send_cursor():670` | 原生点选项直接发 selection | ✅ |
| Simple「下项」→发当前高亮 selection | `dispatch_simple_selection():1122` + `:577` | IDA sub_9AB92A case4（a11b）：BtNext 取 list[cur].sel 后收口 | ✅ |
| AskAvatar Prev/Next/OK/Cancel | `:401-426` + `dispatch_avatar_ok/cancel:2028-2041` | a11i：OK=(7,1,sel) / Cancel=(7,0) | ✅ |
| 输入校验失败弹 tip 不关窗 | `show_input_tip():1809` + 817-821 | IDA sub_9AB50A→sub_9929DD | ✅ |
| **ESC 收口** | `send_key():695-717`（本轮修复） | 旧恒发 `(wire,0)`，**与 Close 按钮 -1 不一致** | ❌→✅ 已修 |

---

## 四、发现并修复的真实缺陷（2 项）

### 缺陷 1：ESC 收口 response 与 Close 按钮不一致（按钮交互缺陷）

- **位置**：`IO/UITypes/UINpcTalk.cpp:707`（修复前）
- **问题**：ESC 恒发 `NpcTalkMorePacket(wire_msgtype(), 0)`；而 Close 按钮在 Say/NEXT/PREV/YesNo/AcceptDecline 发 `-1`（:464/479/495/514/537）。
- **后果**：s9 `NPCScriptManager.nextLevel()`（`BeiDou-Server_s9/.../NPCScriptManager.java:219-228`）中 `mode==-1` 才 dispose，`mode==0` 是「回退上一页」并 `invokeFunction(level+lastLevel)`。故 Say/YesNo 对话按 ESC 会回退/重入脚本而非关闭，与点击 Close 行为分叉；脚本 `action(mode,...)` 中 `mode==-1` 走 dispose，多数脚本 `mode==0 && type>0` 也 dispose，但类型分支不一致。
- **三方对照**：MinMS `send_key()` 与 Close 同为 `(type,-1)`；v95 `OnSay` DoModal==3 静默；本端原生 a11 证据 `-1`=关闭。
- **修复**：ESC 按类型分派——`is_input_talk()||SENDSIMPLE → 0`（与这些类型 CLOSE 分支同形），其余（Say/NEXT/PREV/YesNo/AcceptDecline）→ `-1`。Avatar 已在上方分支 return（cancel=0），不受影响。
- **行号**：修复后 `:704-716`。

### 缺陷 2：校验 tip818/819 换行转义错误（内容渲染缺陷）

- **位置**：`locale/en-US.ini:59-60` / `locale/zh-CN.ini:59-60`（未提交的 i18n 收口改动引入）
- **问题**：原硬编码 C 串 `TIP_818/819` 用真 CRLF（`"…number \r\nequal…"`）；改 locale 后写成字面 `\\r\\n`（字节 `5c 5c 72 5c 5c 6e`，双反斜杠）。`GraphicsGL::LayoutBuilder::add()`（`Graphics/GraphicsGL.cpp:993-1041`）的 `case '\\'` 只识别单反斜杠 token（`\r`/`\n`/`\rn`），双反斜杠会落 `default` 打出字面乱码；`UINotice.cpp:70-76` 又只删真 CR/LF，对字面 token 无效。
- **后果**：GetNumber 越界/min 校验失败弹窗 tip818/819 出现字面 `\r\n` 或错乱换行。
- **修复**：改为单个字面 `\n`（字节 `5c 6e`），与仓库其它 locale 多行项（`cashshop.buy_confirm` 等）一致，`case 'n'` 产生一次换行 = 原生 CRLF 语义。

---

## 五、核对为「一致、无需改」的疑点（避免误改）

1. **响应 lastMsg 字节编号**：v95 用「请求 msgtype+1」的重编号，本端/s9 用请求 msgtype。经 `NPCMoreTalkHandler.java:36` + a11 证据确认本端 s9 契约即请求 msgtype，**不改**。
2. **fill 拉伸 vs 平铺**：老 Journey fork 用拉伸，本端 a11c 证据要求平铺保噪声周期，当前平铺是**正确修正**，不改回。
3. **打字机/虚线高亮/框内输入**：MinMS fork 无这些（fork 定制），本端按 BeiDou V17.7 原生实现，**不改**。
4. **未提交 i18n 的 tip817/820/821**：键在 en-US/zh-CN 均存在（:58/:61-62），`Locale::format` 单占位替换正确，**不改**。
5. **192 客户端**：二进制加壳无逻辑证据，仅资源节点树（UtilDlgEx t=519×28/c=519×13/s=519×44）；与 v83 节点命名/宿主文件不同，属版本差异，非本端缺陷。

---

## 六、验证与部署状态（如实声明）

- ✅ 静态核对：ESC 7 路 response 与 CLOSE 分支逐项对齐；tip 换行字节级确认（5c 6e）。
- ✅ 编译 + 链接：用独立中间目录（`IntDir=build-npctalk-deploy-20261123/obj`）做 Release|x64 **全量干净构建成功**（`EXITCODE=0`）。之前一次增量构建报的 LNK2001 两个无关符号是**陈旧 obj** 导致，干净构建已消除。
  - 构建产物：`build-npctalk-deploy-20261123/MapleStory.exe`
- ✅ **已部署到 live**（2026-11-23 23:40，用户要求）：
  - 覆盖 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` + `locale\{en-US,zh-CN}.ini`
  - live exe SHA256 = `2407df021b43a6ac46384a8bae31a376aaef5453329d9e8ea83a8db6908a64c2`（部署后两次核对一致，size=5577728，与构建产物逐字节相同）
  - live locale tip818/819 字节 = `5c 6e`（单字面 `\n`），已替换原坏值 `5c 5c 72 5c 5c 6e`
  - **回滚点**：`E:\MXD\BeiDou-Client-xy\_backup_npctalk_20261123_234033\`（含部署前 exe SHA `7dfed5ec…ce760a14c` 与两份原 locale）
- ⚠️ 登录器回滚风险：按 USER.md 教训，登录器可能回滚 exe。**"已部署"仅代表本次部署后 SHA 核对通过**；下次会话或启动登录器后须实时重查 live SHA，若为 `7dfed5ec…` 则回滚已发生，需重新部署。
- ❌ 真机发包/截图：未验收（无本端 live 抓包/截图证据，也没有用户确认来源端的截图）。按用户规范，不宣称已实机通过。

## 七、产物

- 本文件；证据见 `evidence/`（三方对照摘要）。
- 改动文件：`IO/UITypes/UINpcTalk.cpp`、`locale/en-US.ini`、`locale/zh-CN.ini`（tip818/819 两行）。
- 构建产物：`build-npctalk-deploy-20261123/MapleStory.exe`（SHA `2407df02…a64c2`）+ 同目录 `locale/*.ini`。
- live 部署：`E:\MXD\BeiDou-Client-xy\`（exe + locale 已覆盖，SHA 核对通过）。
- 回滚备份：`E:\MXD\BeiDou-Client-xy\_backup_npctalk_20261123_234033\`。
