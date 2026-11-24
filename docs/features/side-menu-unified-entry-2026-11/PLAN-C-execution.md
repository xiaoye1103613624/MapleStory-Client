# 侧栏菜单 Web 管理 + 三扩展原生移植 — 执行 Plan（用户拍板 C，2026-11-23）

## 决策摘要
- 范围 C = A+B 全做：①侧栏菜单 Web 管理闭环；②美容院/伤害皮肤/融合外观 原生移植并挂载。
- Web 路径：沿用 BeiDou-Server_s9 Spring admin + gms-ui（与天气/装备管理同范式）。
- 1A（目录右侧横向 flyout）+ 2A（后台库表权威下发 JSON）沿用既有决策。
- 三项 v83 源码是 Kaentake.dll 注入 + Cosmic 服务端 + 32 位硬编码地址 + opcode 0x174，**不可直接移植**，只取协议/UI 语义，用本端原生组件重写。

## 任务分解（有序，含依赖）

### P0 协议与 DB 权威（服务端）
1. `gms-server`: 新建 `side_menu` 三表（menu_item/icon_group/feature_registry 或单表 JSON 快照，取最小：一张 `side_menu_config` 存 revision+json + 一张 `side_menu_feature` 存 bindable 列表），Flyway 迁移（同 changelog-tables.xml 范式）。
2. `SideMenuController`（/sideMenu/{v}/get|save|revision|feature/list）→ `SideMenuService`（DB CRUD + 版本号 + 下发）。
3. 下发：登录/热更包（沿用 0x3743 或新增 opcode，须 REA/服务端 evidence 校准，禁止编造）；客户端 `apply_remote_json` 已有，只接协议入口。
4. `FeatureRegistry` 服务端快照：注册绑定下拉的 feature id（新增三扩展 id）。

### P1 Web 管理（gms-ui）
5. `src/api/sideMenu.ts`：get/save/revision/feature/list（沿用 interceptor 信封）。
6. `src/views/game/sideMenu/`：树编辑（目录/叶子拖拽排序、enabled、iconKey 选择、action 绑定下拉：内置功能/npcId/shop/chat），保存 → revision++。
7. locale zh/en、路由/菜单注册（按现有 game 区约定）。

### P2 客户端侧栏树闭环
8. `UISideIconBar` 已实现 tree+flyout（1A）；修 5s 热重载只比 revision；接服务端下发入口（P0.3）。
9. `side_icons.ini` 旧格式 → `SideMenuConfig::migrate_from_ini` 已在 `SideMenuConfig.cpp:477-527` 实现；确保 live 从老六槽迁到 side_menu.json 树（切换开关）。
10. 三个红灯（checkin/damagerank/colorprism）迁到新树后保留功能，图标换非红底资源或沿用。

### P3 三扩展原生移植（客户端 UI + 服务端逻辑 + 协议）
11. 美容院：`UIBeautySalon`（Hair/Face/Skin 3 tab × 6 slot，avatar 预览用本端 CharLook 快照），item 5920000 解锁槽；服务端 `BeautyHandler`+表（`beauty_slot`/`beauty_unlock`）+opcode（复用现有未用值，不抄 0x174）；WZ 资源经 orange-wz 转密钥入库。
12. 伤害皮肤：`UIDamageSkinPicker`（选皮肤预览），伤害数字渲染改走皮肤帧；服务端 `damageskin_catalog`/`characters.activeDamageSkin`；opcode 独立；WZ Effect/Item 入库。
13. 融合外观：`UIFusionAnvil`（base/skin 双选 + 应用），服务端 `inventoryequipment.anvilItemId` + 外观广播替换（对齐本端 Player/CharLook 线路）；opcode 独立；WZ 入库。
14. 三项注册进 `FeatureRegistry`（P0.4 的下拉 + 客户端 open），挂到 `SideMenuConfig::builtin_seed` 对应 folder（工具/外观/战斗）。

### P4 验证与交付
15. 单测/构建：Java `mvn package`（或项目脚本）、gms-ui `vue-tsc`/`vite build`、客户端 VS2019 build。
16. 真机：启动新 exe + 服务端，验证 Web 保存→下发→侧栏变更、三项功能开闭。
17. live 部署 + SHA 复核（登录器回滚门禁）；产物归档 docs/features/**。

## 证据与约束
- 三扩展核心证据：`美容院/BeautySalonv83/client/src/beautyshop.cpp:116-157`（item 5920000/opcode 0x174/32 位地址）、`java/README.md:20-70`（Cosmic opcode/SQL/install）；`伤害皮肤/DAMAGE_SKIN_SERVER.md`、`融合外观/FUSION_ANVIL_SERVER-SIDE.md`。仅语义参考。
- live exe 当前 SHA 2D980C…（17:15），非我 15:32 部署；本 Plan 产物部署后须实时复核。
- DB 写操作（Flyway/建表）按门禁 17：库表变更脚本只落文件，执行需用户授权；本 Plan 只生成迁移脚本，不擅自对库 DDL/DML。
- orange-wz MCP 10012 在线（本轮探测返回 jsonrpc 缺 session 头属正常），资源改写走 MCP + 跨版本转 key，禁止裸拷。
- 协议 opcode 新增属公共 API，须以服务端现有 opcode 表避空 + 客户端 handler 成对实现，禁止抄扩展 0x174。

## 风险/回滚
- 三项原生移植工作量大，分批实现但一次性交付；每批构建过。
- WZ 资源转 key 后须 live --resource-probe 短启（AGENTS.md 硬门禁）。
- 登录器回滚 exe：部署后延迟复核 SHA，不符则换名/禁更新。
