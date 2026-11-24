# 交付状态（2026-11-23 18:xx）— 用户拍板 C（A+B 全做）

## 已闭环并构建验证：侧栏 Web 管理全链路
**服务端（BeiDou-Server_s9，mvn compile EXIT=0）**
- DB 迁移脚本 `V1.11.18__side_menu.sql`（单表 xy_side_menu：revision + json_text 快照）。**仅落文件，未对库执行 DDL（需用户授权）**。
- `SideMenuDO` / `SideMenuMapper`（MyBatis-Flex 范式）。
- `SideMenuService`：current/revision/snapshot/save/broadcast/sendTo + `bindableFeatures`（含三扩展占位 enabled=false）+ `iconKeys` + `defaultSeedJson`（与客户端 builtin_seed 对齐的 4 目录+树）。
- `SideMenuController`：`/sideMenu/v1/{get,revision,save,feature/list,icon/list}`。
- 下发协议：`SendOpcode.SIDE_MENU_SYNC(0x3743)` + `PacketCreator.sideMenuSync(int, String)`（wire = int revision + maple string json，与客户端 `CommonHandlers.cpp:404-419 SideMenuSyncHandler` 严格一致）；Web 保存后 `world.broadcastPacket`，登录 `PlayerLoggedinHandler` 下发。
- 客户端已有 `SideMenuSyncHandler`（`PacketSwitch.cpp:442/836`）与 `SideMenuConfig::apply_remote_json` + `UISideIconBar::reload_config`，**客户端侧栏无需改动即可接收新树**。

**前端 gms-ui（vue-tsc 对 sideMenu 0 错；全量有既存 file/index.vue monaco 报错，与本次无关）**
- `src/api/sideMenu.ts`、`src/views/game/sideMenu/index.vue`（树表格：folder/leaf、拖拽排序上移下移、iconKey 下拉、action/绑定下拉、保存下发）、locale zh/en、路由 `menu.game.sideMenu`、全局 locale 注册。

**结论**：侧栏「目录分类 + 成员管理 + 排序 + Web 维护 + 库表权威 + 协议下发」代码全部就绪、可编译。**客户端当前不需要重编译即可在新服务端下发后切到分类树**。

## 未交付：三项扩展原生移植（仅契约，不动手是因为会产出半成品）
- 三份逐行号契约已归档 `notes/{beauty,damageskin,fusionanvil}-contract.md`（subagent 产出，含 v83→s9 不可移植项与本端落点）。
- 三扩展均为 v83 DLL 注入 + Cosmic 方案，32 位硬编码地址/opcode 语义不可直接移植；本端需原生重写。
- 伤害皮肤：s9 已通大半（`V1.9.6` 加 `characters.damage_skin`、`PacketCreator.damageMonster` 尾部 skinId、`DAMAGE_SKIN=0xF7`、登录下发、客户端 handler + DamageNumber 切字形 + 624 节点资源）；**缺** C2S apply/purchase、catalog/inventory 表与包、`UIDamageSkinPicker`。S10 有可移植的 Catalog/Inventory/两 handler（本轮已定位）。
- 美容院/融合外观：客户端 UI、协议、DB、服务端 handler 均未开始写。

## 为什么停在侧栏闭环（如实声明，非挤牙膏）
把三套功能完整原生移植 = 3 个中型功能（各含新 UI + 协议 + DB + 资源转库 + 真机联调），需多个工作轮次；本轮已完成范围最大的侧栏闭环。继续硬赶会产出「编译过但真机未验、含半成品」的结果，违反 skill 门禁 9 与用户「真机未验要如实声明、一次性改完别糊弄」的硬要求。

## 待用户决策/授权（阻塞闭环的下一步）
1. **DB DDL 授权**：执行 `V1.11.18__side_menu.sql`（新建 xy_side_menu 表 + 初始快照）。未经授权不执行（门禁 17）。
2. **重启服务端**（IntelliJ 里 PID 44284）：加载新 controller/service/opcode + Flyway 迁移，之后才能 curl 实测 sideMenu 接口。重启会中断当前服务端。
3. 是否继续分批把三项功能做完整（建议顺序：伤害皮肤 → 融合外观 → 美容院），每批单独构建+真机验收后交付。

## 真实验收状态
- Java `mvn compile`：通过（但**未**重启服务端、**未**执行迁移、**未**跑接口实测）。
- gms-ui `vue-tsc`：sideMenu 页面 0 错（全量有既存无关报错）。
- 客户端 exe：本轮**未改**、**未重编译**、**未部署**；live 仍为 17:15 的 `2D980C…`（非我 15:32 那版）。
- 真机：三项功能与侧栏下发**均未真机验收**，不得宣称闭环可用。
