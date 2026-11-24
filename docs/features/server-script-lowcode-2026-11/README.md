# BeiDou-Server_s9 服务端脚本规则研究 + 低代码开发/维护 + NPC 对话窗实时预览 设计与分析

> 性质：**只读取证 + 方案设计**（本轮未修改任何业务代码、未写库、未改任何脚本文件）。
> 目标仓库：`E:\project\BeiDou-Server_s9`（gms-server 服务端 + gms-ui 管理后台）；客户端参考：`E:\project\MapleStory-Client`（BeiDou-xy / Journey 派生 C++ 客户端）。
> 结论性事实全部带**文件路径 + 行号**证据；设计部分明确标注「设计建议」；未取证项逐条声明。用户硬要求：逐条行号级结论，禁止「应该没问题」。

---

## 〇、先回答三个最初的问题（TL;DR）

| 问题 | 结论 | 关键证据 |
| --- | --- | --- |
| 1. 服务端脚本能否做成低代码开发、导出 JS？ | **能**。脚本是标准 GraalJS、入口函数固定、对话可抽象成状态图；低代码 JSON→JS 生成器完全可行。建议用「FlowGraph + 节点/连线 + BEGIN/END 双区共存」。 | `AbstractScriptManager.java:49`（graal.js）、各 Manager 的 `invokeFunction("start"/"action"/"end"/"act")`、`NPCConversationManager.java:1209-1349`（sendXxxLevel 函数路由） |
| 2. 能否「jar 包模式直接修改外挂脚本文件」？ | **本项目不成立「jar 内脚本」说法**——脚本是**外部目录文件**，不在 jar 内。正确做法是「外挂目录：备份→staging→校验→原子替换→reload」。 | `pom.xml` 无 `<resources>` 配置、`target/BeiDou.jar` 内无脚本、2087 个 `.js` 在 `scripts-zh-CN/` 且 git 全量跟踪、`RuntimePaths.java:45-130` 运行时探测路径 |
| 3. 脚本写完后能否在服务端实时渲染 NPC 对话窗？ | **服务端 alone 不能真正渲染**。服务端只决定「发什么类型、什么文字」（NPC_TALK 包字段）；实际成像必须依赖运行中的真实客户端（含 WZ/IMG 资源）。可选：真客户端 live preview / Web 伪渲染 / 对话树可视化预览。 | 客户端 `NpcInteractionHandlers.cpp:397-408`（唯一 change_text 调用点）、`UINpcTalk.cpp:83-311`（本地绘窗）、缺资源则 end_talk（`:381-393`）；仓库内无任何可视预览工具 |

---

## 一、服务端脚本规则研究（证据）

### 1.1 目录全景（2087 个 .js）

`E:\project\BeiDou-Server_s9\gms-server\scripts-zh-CN\` 递归 2087 个 `.js`，8 个一级子目录 + 根级 4 个模板/测试文件：

| 目录 | .js 数 | 用途 |
| --- | --- | --- |
| `npc/` | 771 | 按 NPCID 索引的 NPC 对话（文件名即 ID） |
| `portal/` | 470 | 传送点（语义名） |
| `quest/` | 325 | 按 QuestID 索引的任务 |
| `reactor/` | 292 | 按 ReactorID 索引的反应堆 |
| `event/` | 110 | 活动/PQ（语义名，生命周期回调） |
| `map/` | 94 | 地图脚本（唯一有二级目录：`onUserEnter/` 91、`onFirstUserEnter/` 3） |
| `BeiDouSpecial/` | 18 | 北斗自定义管理/工具脚本（中文文件名） |
| `item/` | 3 | 道具脚本 |
| 根级 | 4 | `NPC Base.js`、`QUEST Base.js`、`REACT Base.js`、`devtest.js` |

- 校验和：4+18+110+3+94+771+470+325+292 = **2087**。
- 语言双树：同级还有英文 `scripts/`（1976 个 `.js`），`scripts-zh-CN/` 为本地化；`AbstractScriptManager.java:54-69` 文件级 i18n（优先 `scripts-<lang>/`，回退 `scripts/`）。
- 编码：**UTF-8 无 BOM**（`file -bi` = utf-8；hex 无 `EF BB BF`；中文直接字面量，如 `npc/1002000.js:7` `var 明珠港_1 = "…"`）。注意引擎按 UTF-8 读（`AbstractScriptManager.java:81`）。
- 命名：文件名即实体 ID（npc= NPCID、quest= QuestID、reactor= ReactorID、map 子目录= 地图ID）；event/portal/item/BeiDouSpecial 用语义名。
- 同 ID 多版本：npc 下 9 组 `X.js`/`X_old.js`，1 组 `1032001.js`/`1032001_nextLevel.js`；44 组字节级重复文件（md5 相同，如 `portal/Spacegaga_out0~3`、`reactor/6702003~6702011`、`quest/2300~2306`）。
- 文件头 4 种风格：OdinMS AGPL 长头（`npc/1012100.js:1-21`）、HeavenMS AGPL 头（`NPC Base.js:2-3`）、Reactor 短头（`reactor/9208001.js:23-27`）、北斗中文 JSDoc（`BeiDouSpecial/Example1.js:1-4`）。**均声明 AGPL-3.0**。

### 1.2 五类脚本规则（入口 / 状态机 / 注入对象）

| 类别 | 入口函数 | 状态模型 | 注入对象 | 证据 |
| --- | --- | --- | --- | --- |
| NPC | `start()` + `action(mode,type,selection)` | `status` 状态机（-1→++，mode=1 进/0 退/-1 关闭）；另有 nextLevel 函数路由 | `cm` = `NPCConversationManager` | `NPC Base.js:27-50`、`npc/1012112.js:28-48`、`NPCScriptManager.java:146` |
| Quest | `start(mode,type,selection)` + `end(mode,type,selection)` | 各自 `var status=-1` | `qm` = `QuestActionManager`（继承 cm） | `QUEST Base.js:25-73`、`QuestScriptManager.java:83/135/177/221` |
| Reactor | `act()`（另有 `hit()` 等具名回调） | 状态存 `eim.setProperty()`，非脚本 status | `rm` = `ReactorActionManager` | `REACT Base.js:26-28`、`reactor/9208001.js:37-65`、`ReactorScriptManager.java:139` |
| Event | 生命周期回调集：`init/setup/playerEntry/changedMap/playerExit/scheduledTimeout/end/timeOut/monsterKilled/...` | 事件实例 `eim` 字符串属性 | `em` = `EventManager`（`engine.put("em",…)`） | `event/0_EXAMPLE.js:17-119`、`event/q3239.js:5-74`、`EventScriptManager.java:44/137/140` |
| Map/Portal/Item | `start(mapScriptMethods)` / `enter(portalInteraction)` / 走 NPC 路径 | 无状态 | 参数为 `MapScriptMethods`/`PortalPlayerInteraction`，item 注入 `im` | `MapScriptManager.java:65/81`、`PortalScriptManager.java:79`、`NPCScriptManager.java:82/134` |

- NPC 两种写法并存：
  - 传统 `status++/--`：`NPC Base.js:32-50`；老 OdinMS 式 `status-=2`（`npc/1002000.js:39-54`、`npc/1012100.js:77-88`）。
  - 北斗 nextLevel（`BeiDouSpecial/Example2.js:10-58`）：`start()→levelStart()→cm.sendSelectLevel(text)`，框架按选项路由到 `level0()/level1()/…`；`NextLevelType.java:6-17`、`NPCScriptManager.java:190-245`。**这是天然的「低代码式」函数路由范式**。

### 1.3 对话 API 与文本标记

- `cm` 类 = `org.gms.scripting.npc.NPCConversationManager`（`NPCScriptManager.java:146 engine.put("cm", cm)`）。**不存在** `MapleClientScriptManager`；**不存在** `sendAccept/sendTalk/sendDecorate`（全仓 grep 0 命中），对应能力为 `sendAcceptDecline`/`sendOk`/`sendStyle`。
- 对话 API → 发包（`NPCConversationManager.java`）：`sendNext:155`(msgType 0 + endBytes "00 01")、`sendPrev:160`("01 00")、`sendNextPrev:165`("01 01")、`sendOk:170`("00 00")、`sendYesNo:179`(msgType 1)、`sendAcceptDecline:184`(0x0C)、`sendSimple:189`(msgType 4)、`sendGetNumber:239`(msgType 3)、`sendGetText:244`(msgType 2)、`sendAskBoxText:261`(msgType 13)、`sendSelectLevel:1262`、`sendYesNoLevel:1344`。
- 底层发包 `PacketCreator.getNPCTalk:3554-3563`：`OutPacket.create(SendOpcode.NPC_TALK)` → writeByte(4)/writeInt(npc)/writeByte(msgType)/writeByte(speaker)/writeString(talk)/writeBytes(endBytes)。
- **类型码以代码为准**（用户早期口述的「4=输入框、5=选择列表」与代码不符）：4=选择列表、2=单行文本输入、3=数字输入、5=InitialQuiz、13=AskBoxText、0x0C=AcceptDecline。证据：`NPCConversationManager.java:157/162/167/172/181/186/191/241/246/263`、`PacketCreator.java:3594/3608/3670/3692/3649`。
- **`#` 标记服务端零替换**：`getNPCTalk` 直接 `writeString(talk)`（`PacketCreator.java:3560`），`ByteBufOutPacket.writeString:78-82` 仅按字符集编码；`#b/#r/#k/#e/#n/#t/#z/#p/#i/#m/#L#l` 全部原样下发，由**客户端**渲染（见 §四）。

### 1.4 脚本引擎、缓存与热更

- 引擎：**GraalJS**（非 Nashorn）。`AbstractScriptManager.java:49 getEngineByName("graal.js").getFactory()`；`:73-76` 强转 `GraalJSScriptEngine`；`:120-124` 开启 host access 允许 `Java.type()`；`:81` UTF-8 读取、`:82 engine.eval(br)` 全文件执行。无 `CompiledScript`、无 `ScriptObjectMirror`。
- 路径解析：`SCRIPT_DIRECTORY="scripts"`（`:45`），语言目录 `scripts-zh-CN` 由 `ServiceProperty.language=zh-CN`（`application.yml:59-61`）拼出；根路径 `RuntimePaths.detectHome:88-130`：①`-Dbeidou.home`/`BEIDOU_HOME` → ②jar 目录及父/祖父 → ③CWD 及父 → ④CWD 回退。`NPCTalkHandler.java:94 start(c, npc.getId(), oid, null)` 传 null→直接用 npcid。
- **缓存两层**：①按 Client：`Client.java:129 Map<String,ScriptEngine> engines`（非线程安全 HashMap），key=`"scripts/"+相对路径`（`AbstractScriptManager.java:91-99`）；②全局：Map/Portal/Event 的 `Map<String,Invocable>`（`MapScriptManager.java:39`、`PortalScriptManager.java:41`、`EventScriptManager.java:46`）。
- 清缓存：NPC/Quest 靠 `dispose→resetContext`（`NPCScriptManager.java:247-260`、`QuestScriptManager.java:190-195`）；全局 `reloadQuestScripts:209-212`、`PortalScriptManager.reloadPortalScripts:89-90`、`MapScriptManager.reloadScripts:45-47`、`EventScriptManager.reload:147-150`。
- 热更能力：**无通用 `@reloadscript`**；只有 GM 命令 `reloadevents/reloadportals/reloadmap`（`CommandsExecutor.java:296/298/299`，REST `CommandController.java:37-56`）。已缓存脚本不感知文件变化（无时间戳检测）。修改 NPC/Quest 后：在线玩家当前会话仍用旧引擎，dispose 后下次才生效；event/portal/map 不执行 GM reload 一直是旧脚本。

---

## 二、管理后台与部署现状（证据）

- 已有 Vue3 + Vite + Arco Design Pro 管理后台 `gms-ui`，集成 `@guolao/vue-monaco-editor`。
- **已有在线脚本编辑器**：路由 `/game/file`（`gms-ui/src/router/routes/modules/game.ts:108`），左侧 a-tree + 右侧 Monaco，`useDebounceFn` 1 秒自动保存；API `POST /file/v1/tree|tree/read|tree/write`（`gms-ui/src/api/fileTree.ts:18-26`、`FileController.java:15-38`）。
- `FileTreeService.java:24-31` 限制目录白名单 `scripts/scripts-zh-CN/wz/wz-zh-CN`；`:56-60 Files.writeString(path, UTF_8)` **直接覆盖写，无备份、无 diff、无 hash 校验**（并发冲突最大来源）。
- 脚本**不在 jar 内**：`pom.xml` 无 `<resources>`，仅有 `spring-boot-maven-plugin repackage`（`:227-245`，finalName=BeiDou）；脚本是外部目录，与 jar 同级部署。启动脚本 `launch.bat:5`/`launch.sh:4` 不显式设脚本路径，靠 `RuntimePaths` 探测。
- 备份现状：脚本全量 git 跟踪（2087/2087，HEAD=`966d633eba`，取证时 1 个 modified），**无自动备份/diff/回滚工具**。
- 可借鉴范式：`WitchConfigManager` + `POST /witchConfig/v1/reload` + 前端按钮（用户前序已完成的全链路 DB 化热更模式）。

---

## 三、低代码开发 + 导出 JS + 备份替换 设计（设计建议）

### 3.1 元模型：FlowGraph + Node + Edge

四类脚本共用一套图结构，仅入口与注入变量不同：

- `FlowGraph = {id, kind: npc|quest|reactor|event, targetKey, entry, nodes[], edges[], meta}`
- `Node = {id, type, props, outPorts[]}`；`Edge = {from, to, guard?}`
- 映射：
  - **NPC** → 一张对话树；`start` 节点 = 会话入口；`showText/showMenu/showYesNo/input/action/condition/end`。nextLevel 方法名映射：每节点生成 `level<Id>()` 函数（`NPCConversationManager.java:1209/1250/1344` 按名路由）。
  - **Quest** → `startGraph`/`endGraph` 两张子图，导出 `function start()/function end()`。
  - **Reactor** → 单入口 `act()` 一张图，action 节点调 `rm.*`。
  - **Event** → 每个生命周期回调绑一张子图，`em` 注入。

NPC 对话树 JSON Schema 草案（节点 `showText/showMenu/showYesNo/input/action/condition/end`）完整定义见本节末「附：Schema 草案」。示例：

```json
{
  "schemaVersion": 1, "kind": "npc", "targetKey": 9000000,
  "entry": "n_welcome", "vars": { "cost": "1000" },
  "nodes": [
    { "id": "n_welcome", "type": "showText",
      "text": "欢迎来到#b明珠港#k，要传送吗？", "method": "sendYesNo",
      "yesNext": "n_choose", "noNext": "n_end", "levelRouting": true },
    { "id": "n_choose", "type": "showMenu", "text": "#b请选择目的地：#l",
      "options": [
        { "label": "#m104000000# 勇士部落", "next": "n_warp1", "selection": 0 },
        { "label": "#m100000000# 射手村", "next": "n_warp2", "selection": 1 }
      ] },
    { "id": "n_warp1", "type": "action",
      "statements": ["cm.gainMeso(-cost);", "cm.warp(104000000);"], "next": "n_end" },
    { "id": "n_warp2", "type": "action",
      "statements": ["cm.gainMeso(-cost);", "cm.warp(100000000);"], "next": "n_end" },
    { "id": "n_end", "type": "end", "method": "dispose" }
  ]
}
```

### 3.2 代码生成器（Java 端新增 `scripting/lowcode` 包）

模块：`FlowModel`（反序列化/校验）、`SchemaValidator`（Schema + 语义：所有 next 指向存在节点）、`NpcEmitter/QuestEmitter/ReactorEmitter/EventEmitter`、`ExpressionSanitizer`（action/condition 仅白名单 `cm./qm./rm./em.` 调用，禁 `Java.type/import/process`）、`HeaderWriter`（UTF-8 无 BOM）、`MarkerRegistry`（生成区边界）。

伪代码：

```
exportFlow(flow):
  validate(flow)
  sb += headerComment(schemaVersion, sha256(flow), GEN_VERSION)
  sb += "// >>> LOWCODE:BEGIN v1 hash=..."
  sb += emitVars(flow.vars)                      # var cost = 1000;
  switch kind:
    NPC:     emitNpc    → start(){ level<entry>(); } + 每节点 level<Id>(){...}
    QUEST:   emitQuestSubgraph(startGraph,"start") + (endGraph,"end")
    REACTOR: emitReactor → function act(){...}
    EVENT:   emitEventCallbacks → function init/setup/playerEntry/...
  sb += "// <<< LOWCODE:END"
  sb += emitPreservedHandwritten(existingFile)    # 区外手写原样保留
  writeUtf8NoBom(stagingPath, sb)
```

约束：字符串 JSON.stringify 等价转义；`#b#k#r#m##l` 当普通文本保留（不转义 `#`）；入口函数名必须与 `invokeFunction` 一致。

**生成/手写共存（关键）**：单文件 `// >>> LOWCODE:BEGIN v1 hash=…` / `// <<< LOWCODE:END` 双区；区外视为手写原样保留。导出前比对生成区 hash：一致→安全覆盖；不一致（手改了生成区）→拒绝覆盖报冲突，由 UI 选「采用模型重新生成」或「反向同步模型」。源模型落 sidecar `scripts-zh-CN/.lowcode/<kind>/<targetKey>.flow.json`（与 js 成对纳入 git），文件头写模型 hash + 相对路径。

### 3.3 热更与部署闭环

**按缓存特点分类**（证据见 §1.4）：

| 类型 | 缓存位置 | reload 策略 | 风险 |
| --- | --- | --- | --- |
| NPC/Item | 按 Client engines | MVP：靠 dispose 自然失效；增强：`ScriptReloadRegistry` 逐 Client 清 key（主线程串行，HashMap 非线程安全） | 在线会话延迟一个周期 |
| Quest | 按 Client engines + 全局 qms | 同上；`reloadQuestScripts()` 仅兜底（会丢进行中任务状态） | 进行中任务影响 |
| Event | 全局 EventEntry | 复用 GM `reloadevents`（`CommandController.java:37`） | cancel+init 中断进行中事件 |
| Portal/Map | 全局 Map（无状态） | 复用 GM `reloadportals/reloadmap`（`:45/:54`） | 安全 |
| Reactor | ReactorScriptManager | 需新增清缓存入口 | 需确认缓存结构 |

**新增 REST API 草案**：

- `POST /script/v1/reload` — 入参 `{scriptType, path, mode:"selective|all", force}`；返回 `{affectedClients, strategy, warnings}`。幂等；失败无持久副作用。
- `POST /script/v1/deploy` — 入参 `{source:{kind:"model|inline", modelRef, content}, target:{scriptType,path}, options:{previewOnly, backup, reloadAfter, expectedHash}}`。
  - `previewOnly=true`：返回 `{generatedJs, diffHtml, validation:{schemaOk,syntaxOk,encodingOk}, risk, plan}`，不落盘。
  - apply：按 §3.4 流程执行；以 `(target.path, expectedHash)` 乐观锁，不符返回 409。

编排顺序：`validate → backup → write staging → verify（UTF-8 无 BOM + JS 语法 + 入口函数齐全）→ atomicReplace → reload(selective) → audit log`。任一失败即回滚。

### 3.4 备份 / 原子替换 / 并发（用户最关心的「源文件生成备份文件，新文件命名为原文件」）

流程（每步都有失败回退）：

```
1) lock(target)                                  # 进程内路径锁
2) currentHash = sha256(target); 校验 == expectedHash (否则 409 Conflict)
3) backupPath = backups/<yyyyMMdd-HHmmss>-<sha8>-<gitCommit>__<kind>__<encodedPath>.bak
   Files.copy(target, backupPath, REPLACE_EXISTING)
4) stagingPath = target + ".staging-" + UUID; writeUtf8NoBom(stagingPath, generatedJs)
5) verify(staging): 无 BOM / JS 语法可 parse / 入口函数齐全 / 大小合理
6) atomicReplace: Files.move(staging, target, ATOMIC_MOVE, REPLACE_EXISTING)
   失败退化 REPLACE_EXISTING + warning（Windows 占用重试 2~3 次）
7) reload（§3.3）；8) audit log；9) unlock
回滚：backupPath 存在则 Files.move(backup, target, REPLACE_EXISTING)，清 staging，必要时再 reload
```

- **Windows 文件占用**：脚本由 `AbstractScriptManager.java:81` 读入即 close，引擎运行期不持文件句柄，正常可直接覆盖；风险仅外部编辑器/杀软短暂占用，用 staging 同目录 ATOMIC_MOVE 缓解。
- **备份与 git 关系**：2087 脚本全量 git 跟踪，git 本身是最强备份/diff/回滚。建议：deploy 自动 `git add+commit`（或至少可读 message），`.bak` 作为「毫秒级即时快照」短周期（7 天）保留后清理；无 git 环境必须保留 `.bak`。
- **并发冲突**：expectedHash 乐观锁 + 进程内 `ConcurrentHashMap<path,ReentrantLock>`；多实例共享目录则需文件锁/DB 锁。**现有在线编辑器 `FileTreeService.writeFile:56-60` 无锁无 hash 校验，是最大冲突来源，建议统一写服务改造。**

### 3.5 「jar 包模式直接修改外挂脚本文件」辨析

- **结论**：本项目脚本**不是 jar 内资源**（pom 无 resources、`target/BeiDou.jar` 内无脚本、2087 `.js` 全量 git 跟踪在外部目录），所以「改 jar 内脚本」不适用。`AbstractScriptManager.java:58-70` 用 `RuntimePaths.getScriptsHome()` 定位外部路径，非 ClassLoader。
- **正确做法**：对 `RuntimePaths.getScriptsHome(language)` 解析出的实际目录做 §3.4 流程；路径必须由 `RuntimePaths` 运行时解析，禁止硬编码。deploy API 返回「实际解析路径」供核对。
- **风险**：①路径探测不一致（deploy 写 A、服务端解析 B → 不生效）；②语言目录陷阱（`scripts` 与 `scripts-zh-CN` 并存、文件级回退，须用与加载端相同 actualPath 判定）；③替换后不清 GraalJS 缓存（最常见失效原因）；④无 jar 内备份，误删只能靠 git/.bak 恢复。

### 3.6 分期路线图

- **MVP**：JSON 模型 → 导出器（NPC/Quest/Reactor）→ 备份+staging+校验+原子替换 → 手动 reload（复用 GM 命令）。验收：导出 NPC 可游戏内对话/菜单/传送，UTF-8 无 BOM，入口函数正确；bak 内容=部署前文件；语法/编码错误在 staging 拦截；失败可从 bak 恢复。
- **增强**：gms-ui 低代码编辑器（节点拖拽连线+属性面板）、`/script/v1/reload|deploy` 完整实现（preview+apply+乐观锁）、diff/回滚 UI、双区共存、NPC/Quest 逐 Client 清缓存。验收：Web 端建模→预览 diff→部署→reload 闭环；乐观锁 409 生效；回滚可恢复到历史版本并 reload。
- **理想**：JS→模型反向解析（golden test 行为等价）、AI 辅助生成、CI 校验（Schema/语法/编码/入口/模型-js 一致性）。
- **风险清单**：语法/入口名错、编码 BOM、git 冲突、缓存不一致、并发、状态机语义丢失（复杂副作用用 `action` 节点原样嵌入 JS 保留）、jar 与目录混淆、全量 reload 性能——每项缓解措施见下文风险表。

| 风险 | 缓解措施 |
| --- | --- |
| 语法/入口名错 | staging 阶段 JS parser 语法预检 + 必需入口函数清单校验 + golden 回归；action/condition 白名单过滤 |
| 编码 BOM | 用 `StandardCharsets.UTF_8` 直接输出、禁带 BOM writer；staging 校验首字节 ≠ `EF BB BF`（引擎按 UTF-8 读，`AbstractScriptManager.java:81` 无兜底） |
| git 冲突 | deploy 自动 commit + 可读 message；expectedHash 乐观锁；与在线编辑器统一写服务；冲突 409 由 UI 处理 |
| 缓存不一致 | 强制 reload 编排；NPC/Quest 逐 Client 清 key 且主线程串行（HashMap 非线程安全 `Client.java:129`）；event/portal/map 用现成 GM reload |
| 并发 | expectedHash 乐观锁 + 进程内路径锁；多实例用文件锁/DB 锁；sidecar 与 js 同锁更新 |
| 状态机语义丢失 | 低代码优先覆盖对话/简单分支子集；复杂副作用用 `action` 节点原样嵌入 JS；复杂脚本标记「仅手写，不纳入低代码」 |
| jar 与目录混淆 | 文档明确脚本为外挂目录；路径统一由 `RuntimePaths` 解析；deploy 返回实际解析路径核对 |
| 全量 reload 性能 | selective 优先、低峰执行、分批限流；`ScriptReloadRegistry` 用 Set 索引；event reload 公告 |

### 附：NPC 对话树 JSON Schema 草案（design proposal）

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "NPCDialogFlow",
  "type": "object",
  "required": ["schemaVersion","kind","targetKey","entry","nodes"],
  "properties": {
    "schemaVersion": { "const": 1 },
    "kind": { "const": "npc" },
    "targetKey": { "type": "integer", "description": "npcId" },
    "scriptName": { "type": "string" },
    "fallbackTarget": { "enum": ["npc","BeiDouSpecial","item"] },
    "entry": { "type": "string", "description": "起始节点 id，导出为 function start()" },
    "vars": { "type": "object", "additionalProperties": { "type": "string" } },
    "nodes": { "type": "array", "items": { "oneOf": [
      { "$ref": "#/$defs/showText" }, { "$ref": "#/$defs/showMenu" },
      { "$ref": "#/$defs/showYesNo" }, { "$ref": "#/$defs/input" },
      { "$ref": "#/$defs/action" }, { "$ref": "#/$defs/condition" },
      { "$ref": "#/$defs/end" }
    ] } }
  },
  "$defs": {
    "base": { "type": "object", "required": ["id","type"], "properties": {
      "id": { "type": "string" }, "type": { "type": "string" },
      "label": { "type": "string" }, "next": { "type": "string" }
    } },
    "showText": { "allOf": [{"$ref":"#/$defs/base"}, {"type":"object",
      "required":["text"], "properties":{
        "type":{"const":"showText"},
        "text":{"type":"string","description":"支持 #b#k#r#m##l 等标记"},
        "speaker":{"type":"integer"},
        "method":{"enum":["sendNext","sendOk","sendSimple","sendNextPrev"],"default":"sendNext"},
        "nextLevel":{"type":"boolean","default":false}
      }}] },
    "showMenu": { "allOf": [{"$ref":"#/$defs/base"}, {"type":"object",
      "required":["text","options"], "properties":{
        "type":{"const":"showMenu"}, "text":{"type":"string"},
        "options":{"type":"array","items":{"type":"object","required":["label","next"],
          "properties":{"label":{"type":"string"},"next":{"type":"string"},"selection":{"type":"integer"}}}},
        "method":{"enum":["sendSimple"],"default":"sendSimple"}
      }}] },
    "showYesNo": { "allOf": [{"$ref":"#/$defs/base"}, {"type":"object",
      "required":["text","yesNext","noNext"], "properties":{
        "type":{"const":"showYesNo"},"text":{"type":"string"},
        "yesNext":{"type":"string"},"noNext":{"type":"string"},"levelRouting":{"type":"boolean","default":false}
      }}] },
    "input": { "allOf": [{"$ref":"#/$defs/base"}, {"type":"object",
      "required":["prompt","var","next"], "properties":{
        "type":{"const":"input"},"prompt":{"type":"string"},"var":{"type":"string"},
        "numeric":{"type":"boolean"},"next":{"type":"string"}
      }}] },
    "action": { "allOf": [{"$ref":"#/$defs/base"}, {"type":"object",
      "required":["statements"], "properties":{
        "type":{"const":"action"},
        "statements":{"type":"array","items":{"type":"string","description":"白名单 JS 语句，如 cm.gainMeso(-1000);"}},
        "next":{"type":"string"}
      }}] },
    "condition": { "allOf": [{"$ref":"#/$defs/base"}, {"type":"object",
      "required":["expression"], "properties":{
        "type":{"const":"condition"},"expression":{"type":"string"},
        "trueNext":{"type":"string"},"falseNext":{"type":"string"}
      }}] },
    "end": { "allOf": [{"$ref":"#/$defs/base"}, {"type":"object","properties":{
      "type":{"const":"end"},"method":{"enum":["dispose","ok"],"default":"dispose"}
    }}] }
  }
}
```

---

## 四、NPC 对话窗「实时渲染」可行性（证据 + 方案）

### 4.1 客户端渲染链事实

- **Opcode**：`Net/PacketSwitch.cpp:149 NPC_DIALOGUE=304`、`:798 emplace<NpcDialogueHandler>()`、`:1177 "NPC_TALK"`。
- **Handler**：`Net/Handlers/NpcInteractionHandlers.cpp:41-409 NpcDialogueHandler::handle`：
  - `:45` skip speakerType、`:47` npcid=read_int、`:48` msgtype=read_byte、`:49` speaker=read_byte；
  - `switch(msgtype)` `:91-365`：case0 Say、1 YesNo、2 GetText、3 GetNumber、4 Simple、5/6 Quiz、7/8 Avatar、9/10 AskPet、12 AcceptDecline、13 BoxText、14 Mirror；与服务端 `PacketCreator` 写出顺序一致；
  - `:397-408` 取/建 `UINpcTalk` 槽并 `change_text(...)`——**全仓唯一 change_text 调用点**；`:378-393` 缺 `UtilDlgEx` 资源则 end_talk 不开窗。
- **UI 类**：`IO/UITypes/UINpcTalk.h:35 class UINpcTalk : UIElement`；窗口类型枚举 `:38-57`（0=Say,1=YesNo,2=GetText,3=GetNumber,4=Simple,7=Avatar,12=AcceptDecline,13=BoxText）；构造/资源加载 `UINpcTalk.cpp:83-163`（`UtilDlgEx["t"/"c"/"s"/"bar"]` `:109-112`，按钮 `BtClose/BtOK/BtYes/BtNo/BtNext/BtPrev` `:116-130`）；draw `:165-311`（打字机 `:313-384`）；按钮分发 `:394-596`。
- **`#` 标记两层客户端解析**：
  1. `UINpcTalk.cpp:870-1120 format_text()`：`#h`玩家名、`#p`NPC名(String/Npc.img)、`#t/#z`道具名(ItemData)、`#q`技能名、`#m`地图名、`#o`怪物名、`#c`持有数、`#x`任务进度、`#E/#I/#S/#K`剥除、`#L` strip_simple_choices(`:1119`)；
  2. `Graphics/GraphicsGL.cpp:1035-1277 LayoutBuilder::add case '#'`：`#b`蓝(`:1041`)、`#c`橙、`#d`紫、`#e/#E`粗、`#f`图标、`#g`绿、`#i/#v`物品icon、`#k`深灰(`:1138`)、`#l/#L`skip、`#n/#N`取消粗、`#r`红(`:1211`)、`#s`技能icon。
- **选项列表**：非独立控件，`UINpcTalk.h:279-296 SimpleChoice` + `UINpcTalk.cpp:1138-1221 strip_simple_choices` 解析 `#L<n>#label#l`，`:1223-1240 layout_simple_choices` 用 `Text::position_of` 反取命中框。
- **现有工具**：仓库内**无任何可视预览/模拟 UI 工具**。仅有 `tools/test_npctalk_simple_visible.py:1-185`（几何/滚动数值断言，不渲染）、`_tools/_verify_angel_npctalk.py:1-10`（IDA 逆向核对）。`IO/Components/NpcText.cpp` 全文在注释块内（`:22-166`），是死代码。

### 4.2 技术边界结论（必须如实声明）

> **服务端 alone 不能真正渲染 NPC 对话窗。** 服务端只能决定 NPC_TALK 包中的「npcId、msgtype、speaker、文本、input 参数/style(endBytes)」——这是边界内可完全掌控的部分。窗口实际成像必须依赖**运行中的真实客户端**：`handler → UINpcTalk::change_text → format_text/LayoutBuilder 展开 # 令牌并排版 → 用 UIWindow.img/UtilDlgEx* + Npc.img + String.img + Basic.img 本地资源绘窗`；资源缺失时客户端直接 end_talk 不开窗（`NpcInteractionHandlers.cpp:381-393`）。服务端自身既无 UI 引擎也无这些资源，不能凭空渲染出画面。

### 4.3 可选实现路径对比

| 路径 | 能做什么 | 不能做 | 成本/风险 |
| --- | --- | --- | --- |
| A. 纯服务端对话树/状态机可视化预览 | 画出节点、文本、分支、跳转，检查逻辑闭环 | 不是真实 NPC 窗口画面 | 低；可与低代码编辑器同页，MVP 首选 |
| B. 管理后台 Web 伪渲染 NPC 对话框 | 用 CSS 仿 `UtilDlgEx` 样式显示文本/颜色/选项，所见近似真窗 | 非真实客户端像素，字体/图标/立绘无法 1:1 | 中；适合文案/流程快速核对 |
| C. 挂真实客户端 live preview（推荐做真实验收） | 连测试服，脚本写完 reload → 游戏内点 NPC 看真实窗 | 需客户端+资源+账号，自动化难 | 中高；唯一真实成像验收手段 |
| D. 截图/录屏自动化验收 | 固定机位截图对比布局/文案 | 非编辑时实时；需真机 | 中；回归测试用 |

**推荐组合**：MVP 用 A（逻辑预览）+ B（Web 伪渲染）做「写完即可看效果」的快速闭环；真实成像验收走 C（挂真客户端 live preview），D 做回归。三种互补，不可互相替代。

### 4.4 与低代码编辑器的集成闭环（设计建议）

`编辑模型 → 导出 JS → 备份+原子替换 → reload → 预览`：

- 预览页读取**待部署的 JSON 模型**（或 staging JS），A/B 路径不依赖文件已落盘；编辑时即时更新。
- C 路径（真客户端）必须在文件替换 + reload 成功后触发（否则看到旧脚本）；NPC/Quest 注意 §3.3 的会话延迟——reload 后需让目标玩家结束当前对话（dispose）再点 NPC。
- 备份/原子替换与预览共存：预览只读 staging/模型，不占用文件；部署成功后预览切到新模型快照。

### 4.5 分期与验收

- MVP：A+B 预览上线，脚本写完 Web 端可见对话树与伪渲染窗；验收：示例 1012112.js 对话树完整渲染、分支可达。
- 增强：C live preview（测试服一键 reload + 真客户端窗口），D 截图回归；验收：Web 预览与真机窗口文案/分支一致。
- 验收硬要求（用户规则）：**真机未验收时如实声明**；截图须确认来源端（本端 vs 参考端），禁止拿参考端截图当本端验收证据（见用户 USER.md 教训）。

---

## 五、Harness 面向另一仓库维护的前提（证据 + 建议）

- **证据**：`~/.dsh/storages/workspace.json` 中 `E:\project\BeiDou-Server_s9` **已是 DSH 注册工作区**（与当前 MapleStory-Client 并列，有独立 session 目录 `--E-project-BeiDou-Server_s9--` 和自身 `AGENTS.md/CLAUDE.md`）。`dsh-sdk-protocol/src/types.ts:130-146/216-217` 定义 `session/working-directory/get|set`。subagent 带 `cwd` 参数可指向外部仓库根（`dsh-tool-subagent/README.zh.md:28`）。MCP 支持 project/global 作用域（`dsh-mcp-connector/README.md:147/287/333`）。
- **建议**：为 BeiDou 仓单开工作区/会话维护 2087 脚本 + Java + Vue（写沙箱与 repo 根一致、AGENTS/CLAUDE 自动生效、project MCP 隔离）；不建议复制进当前 workspace（破坏 git/串扰规则）或符号链接挂载（文件工具不穿透链接）。
- 当前会话具备 `todo_write`/`create_goal`/`schedule_*`/后台 job/`workflow`/`subagent` 等长周期能力；无独立 Plan 工具（用 todo+goal+schedule 组合承载）。

---

## 六、实施结果（2026-11-23 已完成，用户 7 项决策全部采纳）

### 6.1 已落地的代码（全部实测编译/构建通过）

**服务端 gms-server（新增包 `org.gms.scripting.lowcode` + 3 DTO/Controller/Service 改动）**
- `FlowModel.java`：FlowGraph POJO（npc/quest/reactor/event 四类共用节点模型）
- `FlowValidator.java`：schema+语义校验（出边存在性、入口唯一、JS 标识符合法）
- `ExpressionSanitizer.java`：action/condition 白名单，拒绝 `Java.type/import/require/process/eval/new Function` 等宿主逃逸
- `ScriptEmitter.java`：四类型导出器。签名严格对齐实读源码：`sendNextLevel(next,text)`(:1209)、`sendYesNoLevel(no,yes,text)`(:1344)、`sendSelectLevel(prefix,text)`(:1273)、`sendNextSelectLevel(next,text)`(:1286)、`getInputNumberLevel(next,text,def,min,max)`(:1302)、`getInputTextLevel(next,text)`(:1315)、`sendAcceptDeclineLevel(decline,accept,text)`(:1329)、`sendLastNextLevel(last,next,text)`(:1236)。UTF-8 无 BOM 输出 + `BEGIN/END` 双区标记 + 模型 hash。
- `ScriptDeployService.java`：统一写服务 = expectedHash 乐观锁 → 备份（含时间戳/hash/git commit）→ 同目录 staging（UTF-8 无 BOM）→ 校验 → ATOMIC_MOVE 原子替换（失败退化 REPLACE_EXISTING）→ git add+commit → 7 天清理备份；失败自动从备份回滚。路径白名单仅 `scripts`/`scripts-zh-CN`，防逃逸。
- `ScriptController.java`：`POST /script/v1/preview|deploy|reload|hash`。热更策略：npc/quest/item 下次对话生效（依赖 `dispose→resetContext`，不主动清引擎缓存，零并发风险）；event/portal/map 复用 `CommandService.reloadEventsByGMCommand/reloadPortalsByGMCommand/reloadMapsByGMCommand`（`CommandService.java:218/227/233`）；quest 可选 `QuestScriptManager.reloadQuestScripts()`（`:209`）。
- `LowCodeDeployDTO.java`；`FileWriteDTO.java` 加 `expectedHash`；`FileController.java` 加 `/tree/hash`、写入传 expectedHash；`FileTreeService.java` 的 `writeFile` 委派到统一写服务（保留旧签名兼容）。

**管理后台 gms-ui（新增 api + view + locale + route）**
- `src/api/lowCodeScript.ts`：FlowModel 类型 + `previewFlow/deployFlow/reloadScript/readScriptHash` + `EMPTY_FLOW` 模板。
- `src/views/game/lowCodeScript/index.vue`：低代码编辑器三栏（左节点列表 / 中属性面板+生成JS / 右 Web 对话窗预览），工具条含 kind 切换、targetKey、生成JS、部署、热更、hash；支持 NPC/Quest/Reactor/Event 四类节点的属性编辑。
- `src/views/game/lowCodeScript/NpcTalkPreview.vue`：**Web 模拟客户端对话窗**——按 msgType 渲染按钮组合（0 Say / 1 YesNo / 2 GetText / 3 GetNumber / 4 Simple / 12 AcceptDecline）、`#L<n>#label#l` 菜单可点、`#b/#r/#k/#d/#g/#c/#e/#n` 颜色粗体复刻 `GraphicsGL.cpp:1035-1277`、数据类 `#t/#z/#p/#m` 占位显示、从入口播放/点击分支跳转。贴图/立绘/像素字体为占位并如实标注。
- 路由 `/game/lowCodeScript`（admin）；zh-CN/en-US 双语 locale 接入 `src/locale/*`。

### 6.2 验证（实测，非「应该没问题」）
- 服务端 `mvn -o compile` EXIT 0；`mvn -o test-compile -DskipTests` EXIT 0；`mvn -o package -DskipTests` EXIT 0（`target/BeiDou.jar` 12:08 更新，117,747,703 字节）。
- Golden 测试 `ScriptEmitterGoldenTest.main`（12 项全 PASS）：UTF-8 无 BOM、start/level 入口、BEGIN/END 标记、`sendYesNoLevel(no,yes)` 参数顺序、菜单 `#L` 路由、`#b#k` 原样保留、Quest start/end、Reactor act()、Event init/setup、非法出边被拒绝。
- 前端 `npx vue-tsc --noEmit`：**本功能 0 错误**（仅剩 `file/index.vue:110/114/116` 3 个既有历史错误，本轮未触碰该文件）；`npx vite build --config vite.config.prod.ts` EXIT 0，产物含 lowCodeScript 代码。
- 未擅自 commit：仓库存在大量他人未提交改动（天气/装备/协议历史工作），本工具只在「部署脚本」API 内按用户决策自动 git 提交该脚本，不在会话层提交整个仓库。

### 6.3 诚实边界（按用户硬要求声明）
- **真机未验收**：Web 对话窗是语义仿真（类型/按钮/#标记一致），非真实客户端像素；未在网关内点游戏 NPC 验证实际发包与渲染。部署→reload→客户端开窗的真机闭环待用户测试服验收。
- **git 自动提交**：`ScriptDeployService` 用 `ProcessBuilder` 调 `git add/commit`；需服务端进程有仓库写权限与 git 身份配置。若 git 不可用（nogit）则记录 warning 但不阻断部署。
- **在线编辑器并发**：`/file/v1/tree/write` 已改为走统一写服务（带 lock），但旧前端 `file/index.vue` 未回填 expectedHash（仍传空→不校验），只有低代码 deploy 与新 hash 接口形成完整乐观锁；如需在线文本编辑也强校验，下一步改其前端读写 hash（本次未改，避免触碰历史错误文件）。
- 决策 6 由最初的「C 真客户端 live preview」按用户最新澄清改为「B Web 管理界面模拟客户端对话窗」——本轮实现的是 Web 仿真，客户端零改动。

---

## 六.5 第二轮修复（2026-11-23 下午，用户实机反馈"请求报错/怎么用"）

用户实机打开界面后反馈四点：①怎么用 ②自带脚本怎么预览 ③缺常用文件操作 ④请求基本都报错。逐条处理（详见 [_usage.md](_usage.md)）：

1. **请求报错根因（P0，已修）**：
   - 前端 `lowCodeScript.ts` 手动 `submit()` 包装 + `interceptor.ts:34-39` 自动包装 = 双层包装，后端 `SubmitBody.data` 全空。已改为直接 `axios.post(url,data)`，与既有 api 一致。
   - 运行中的 IDEA 后端 JVM（05:57 启动）未加载 12:04/14:08 才编译进 `target/classes` 的新类 → 新接口 404。**需用户重启 IDEA 运行实例**（不擅自杀进程，内有未提交工作）。`mvn package` 当前被他人半成品 `ClientDataSyncService.java:142/257/308/331` 阻塞，与本功能无关，`mvn compile` 已通过。
2. **自带脚本预览（已做）**：新增 `ScriptParseService` 反向解析（提取字面量对话/窗口类型，非等价回写）+ `/script/v1/parse`；前端左侧加载 `scripts-zh-CN` 目录树，点 `.js` 即解析并在 Web 窗预览，附「原始脚本」Tab 与动态对话告警。
3. **常用文件操作（已做）**：新建/另存为/删除（先备份）/导出导入模型 JSON。
4. **怎么用**：见 [_usage.md](_usage.md) 完整流程（A 预览自带脚本 / B 从零做对话并部署 / C 文件操作）。

验证：后端 `mvn compile` EXIT 0；前端本功能 `vue-tsc` 0 错误、`vite build` EXIT 0（仅 `file/index.vue:110/114/116` 3 个既有历史错误未碰）。截图来源：本端 gms-ui 工具界面，非客户端截图，无证据错配。

---

## 七、未取证项声明（如实）

1. 未逐文件读取全部 2087 个脚本；规则基于模板 + 每类 1-3 样本 + 全库 API 计数。
2. 未核实 `BeiDouSpecial/` 中文名工具脚本内部入口（仅取证 Example1/Example2）。
3. 未核实 `scripts/` 与 `scripts-zh-CN/` 全部逐一同名映射（仅抽样 4 个）。
4. 未运行服务端验证脚本可加载性；44 组重复文件仅报字节一致，不判定是否合并。
5. `sendOk` 是否强制关窗未取证（仅见其与 dispose 连用）。
6. `ReloadMapCommand` 是否触发 `MapScriptManager.reloadScripts` 未逐行展开（resetMap 内部）。
7. ReactorScriptManager 缓存结构未逐行展开，热更入口需确认。
8. 是否存在多服务端实例共享 scripts 目录未确认（决定锁粒度）。
9. DSH app.asar 内 checkout 文档不可读（虚拟归档），workspace 结论来自 ~/.dsh 运行时数据 + SDK 类型定义。
10. 未调用 orange-wz MCP（本需求为脚本/Java/C++ 文本层，未涉及 WZ/IMG 改写；若后续做 B/C 路径的图标/立绘 1:1 预览需按 AGENTS.md 规则走 orange-wz）。
11. **无真机验收**：本轮全程只读取证，未进游戏点 NPC、未 reload、未部署；所有「可生效」均为基于源码/日志的推演，真机闭环待用户确认后另做。
