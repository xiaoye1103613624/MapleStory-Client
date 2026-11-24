# 取证摘要（行号级，供后续会话复用）

> 全部来自只读取证；本文件是 README.md 的证据底稿，供接续会话直接引用，避免重复烧上下文。

## 服务端脚本引擎（gms-server）
- 引擎 GraalJS：`org/gms/scripting/AbstractScriptManager.java:49` `getEngineByName("graal.js").getFactory()`；`:73-76` 强转 GraalJSScriptEngine；`:81` UTF-8 读取；`:82 engine.eval(br)`；`:120-124` 开启 host access（Java.type 可用）。无 Nashorn/CompiledScript。
- 语言 i18n：`:45 SCRIPT_DIRECTORY="scripts"`、`:54-69` 优先 `scripts-<lang>/` 回退 `scripts/`；`application.yml:59-61 gms.service.language=zh-CN`。
- 路径根：`org/gms/util/RuntimePaths.java:88-130 detectHome`（beidou.home/BEIDOU_HOME → jar 目录及父/祖父 → CWD）；`:55-60 withLanguage` 拼 `scripts-zh-CN`。
- 缓存：`Client.java:129 Map<String,ScriptEngine> engines`（HashMap 非线程安全），`:1215-1225` get/set/remove；`AbstractScriptManager.java:91-99` key=`"scripts/"+相对路径`；`:114-116 resetContext` 清 key。
- 清缓存/热更：NPC `NPCScriptManager.java:247-260 dispose→resetContext`；Quest `QuestScriptManager.java:190-195/209-212`；Event `EventScriptManager.java:147-150 reload()`；Portal `:89-90`；Map `:45-47`。GM 命令 `CommandsExecutor.java:296/298/299`（reloadevents/reloadportals/reloadmap）；REST `CommandController.java:37-56`。**无通用 @reloadscript、无文件时间戳检测。**
- 注入对象：`NPCScriptManager.java:146 engine.put("cm", cm)`（cm=NPCConversationManager，`:85`）；Quest `QuestScriptManager.java:83/135/177/221` qm=QuestActionManager；Reactor `:139` rm；Event `EventScriptManager.java:44/137/140` em=EventManager；Map `:65/81`、Portal `:79` 参数注入；item 注入 im（`NPCScriptManager.java:82/134`）。
- 对话 API（`NPCConversationManager.java`）：sendNext:155、sendPrev:160、sendNextPrev:165、sendOk:170、sendYesNo:179、sendAcceptDecline:184、sendSimple:189、sendGetNumber:239、sendGetText:244、sendAskBoxText:261、sendSelectLevel:1262、sendYesNoLevel:1344。底层 `PacketCreator.java:3554-3563 getNPCTalk`（NPC_TALK opcode，写 msgType/speaker/text/endBytes）。
- 类型码：4=选择列表、2=GetText、3=GetNumber、5=Quiz、13=BoxText、0x0C=AcceptDecline（**用户口述「4=输入框、5=选择列表」与代码不符**）。`#` 标记服务端零替换（`PacketCreator.java:3560` 直接 writeString）。
- nextLevel 路由：`NextLevelType.java:6-17`、`NPCScriptManager.java:190-245`、`NPCConversationManager.java:1209-1349`。

## 脚本目录（scripts-zh-CN）
- 2087 个 .js：npc 771 / portal 470 / quest 325 / reactor 292 / event 110 / map 94（onUserEnter 91 + onFirstUserEnter 3）/ BeiDouSpecial 18 / item 3 / 根 4。
- UTF-8 无 BOM；中文直接字面量（`npc/1002000.js:7`）。命名：文件名即实体 ID。
- 同 ID 多版本：9 组 X.js/X_old.js、1 组 1032001/_nextLevel；44 组 md5 重复；模板残留（NPC Base.js 等）。
- 语言双树：scripts/ 1976（英文）+ scripts-zh-CN/ 2087（中文）。
- 入口分布：start 1180 / action 613 / enter 470 / end 288 / act 283 / event 生命周期 ~110。
- 被 git 全量跟踪（2087/2087，HEAD=966d633eba）。

## 管理后台
- gms-ui Vue3 + Vite + Arco Design Pro + Monaco；在线脚本编辑器 `/game/file`（`gms-ui/src/router/routes/modules/game.ts:108`）。
- API `POST /file/v1/tree|tree/read|tree/write`（`gms-ui/src/api/fileTree.ts:18-26`、`FileController.java:15-38`）；`FileTreeService.java:24-31` 白名单四目录、`:56-60` 直接覆盖写无备份/diff/hash。
- 脚本不在 jar：`pom.xml` 无 `<resources>`，仅 spring-boot repackage（`:227-245`）；外部目录部署。
- 可借鉴：`WitchConfigManager` + `/witchConfig/v1/reload` 热更范式。

## 客户端渲染链（MapleStory-Client）
- Opcode `Net/PacketSwitch.cpp:149 NPC_DIALOGUE=304`、`:798 emplace<NpcDialogueHandler>()`。
- `Net/Handlers/NpcInteractionHandlers.cpp:41-409 NpcDialogueHandler::handle`：`:45-49` 解包 speaker/npcid/msgtype/speaker；`:91-365 switch(msgtype)` case0-14；`:397-408` 唯一 `change_text` 调用；`:381-393` 缺资源 end_talk 不开窗。
- UI `IO/UITypes/UINpcTalk.h:35`：类型枚举 `:38-57`、按钮 `:215-240`、SimpleChoice `:279-296`；`UINpcTalk.cpp:83-163` 构造（UtilDlgEx 资源）、`:165-311 draw`、`:313-384 打字机`、`:394-596 按钮`、`:870-1120 format_text`、`:1138-1240 #L 选项解析`、`:835-868 msgType→类型/style→prev,next`。
- `#` 两层客户端解析：`UINpcTalk.cpp:870-1120`（数据类 #h/#p/#t/#z/#q/#m/#o/#c/#x）+ `Graphics/GraphicsGL.cpp:1035-1277`（样式/图标 #b/#c/#d/#e/#f/#g/#i/#k/#l/#n/#r/#s）。
- 资源 key：`UI/UIWindow.img|UIWindow2.img/UtilDlgEx[t,c,s,bar]` + `UtilDlgEx_Avatar/Pet`、`Npc/<7位id>.img/stand/0`、`String/Npc.img/<id>/name`、`UI/Basic.img/BtOK2|BtCancel2`。
- 无可视预览工具：仅 `tools/test_npctalk_simple_visible.py:1-185`（数值断言）、`_tools/_verify_angel_npctalk.py:1-10`（IDA 核对）；`IO/Components/NpcText.cpp:22-166` 全文注释（死代码）。

## 核心边界
服务端 alone 不能渲染 NPC 对话窗——它只写 NPC_TALK 包（npcId/msgtype/speaker/text/style），成像依赖真实客户端 + WZ/IMG 资源。预览三选：A 对话树可视化 / B Web 伪渲染窗 / C 真客户端 live preview（唯一真实成像验收）/ D 截图回归。推荐 A+B 快速闭环 + C 真实验收。

## 实施落点（2026-11-23，用户澄清后按 B 方案）
用户最新澄清「要的是 Web 管理界面模拟客户端对话窗效果」，故决策 6 由 C 改 B，客户端零改动。已落地：gms-server 的 org.gms.scripting.lowcode（FlowModel/FlowValidator/ExpressionSanitizer/ScriptEmitter）+ ScriptDeployService + ScriptController + /script/v1/*；gms-ui 的 /game/lowCodeScript 编辑器 + NpcTalkPreview.vue（Web 仿真窗）+ api + 双语 locale。验证：mvn compile/test-compile/package EXIT 0、golden 12 PASS、vue-tsc 本功能 0 错误、vite build EXIT 0。详见 README.md §六。

## Harness 前提
`~/.dsh/storages/workspace.json` 中 BeiDou-Server_s9 已是注册工作区（独立 session 目录 + 自身 AGENTS/CLAUDE）；SDK `session/working-directory/get|set`（`dsh-sdk-protocol/src/types.ts:130-146/216-217`）；subagent 支持 cwd（`dsh-tool-subagent/README.zh.md:28`）；MCP project/global 作用域（`dsh-mcp-connector/README.md:147/287/333`）。建议单开工作区维护，不复制/不 symlink。
