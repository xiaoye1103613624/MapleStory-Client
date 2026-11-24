# 最终交付（2026-11-23 19:xx）— 用户授权 C（A+B 全做）

## 一、侧栏 Web 管理（全链路闭环，实测通过）
- 服务端：`V1.11.22__side_menu.sql`（xy_side_menu: revision+json_text）、`SideMenuDO/Mapper/Service/Controller`（/sideMenu/v1/{get,revision,save,feature/list,icon/list}）、默认分类种子（4 目录+root 叶子+三扩展叶子，folder/leaf 均带 order）。
- 下发：`SendOpcode.SIDE_MENU_SYNC(0x3743)` + `PacketCreator.sideMenuSync`（int revision + maple string json）；Web 保存后 `world.broadcastPacket`，登录 `PlayerLoggedinHandler.sendTo`。客户端 `SideMenuSyncHandler`/`apply_remote_json`/`UISideIconBar::reload_config` 已就绪，**无需改客户端**。
- 前端 gms-ui：`src/api/sideMenu.ts`、`src/views/game/sideMenu/index.vue`（树表格：目录/功能、↑↓排序、iconKey 下拉、action/绑定下拉、保存下发）、zh/en locale、路由。
- **实测**（JWT，8686）：`/feature/list` 25 项（damageskin/beautysalon/fusionanvil enabled=true，中文 UTF-8 正确）；Flyway 迁移全部执行（含新表）。

## 二、三项扩展原生移植（均已编译通过）
### 伤害皮肤（服务端 + 客户端全链）
- 服务端：`V1.11.23` catalog/inventory 表、`DamageSkinCatalog/Inventory`、`DamageSkinApply/PurchaseHandler`、`PacketCreator` 4 包、`SendOpcode 0x3746..0x3749`、`RecvOpcode 0x3746/0x3747`、登录下发目录/拥有、登录 `PlayerLoggedinHandler`。
- 客户端：`OutPacket.DAMAGE_SKIN_APPLY/PURCHASE`、`PacketSwitch` 4 收包 emplace、`CommonHandlers` 4 handler、`DamageSkinState`、`UIDamageSkinPicker`（catalog 列表、购买/应用、结果提示）、`FeatureRegistry.damageskin`、侧栏叶子。
- 注意：catalog 种子是示例 id（1/2），需管理员按实际 `Effect/DamageSkin.img/<id>` 校正；picker 空目录显示提示，不发假数据。

### 美容院（服务端 + 客户端）
- 服务端：`V1.11.24` beautystorage/beautyunlock 表、`org.gms.server.beauty.{BeautyData,BeautyStorage}`、`BeautyHandler`、`PacketCreator.beautyOpen/beautyData`、`SendOpcode.BEAUTY_RESULT(0x174)`/`RecvOpcode.BEAUTY_ACTION(0x174)`、`@beauty` 命令、5920000 解锁槽。
- 客户端：`OutPacket.BEAUTY_ACTION(372)`、`PacketSwitch.BEAUTY_RESULT(0x174)` + emplace、`CommonHandlers.BeautyResultHandler`、`BeautyState`、`UIBeautySalon`（3 tab×6 槽、保存/应用/删除/解锁）、`FeatureRegistry.beautysalon`、侧栏叶子。
- 限制：未做 avatar 冻结预览（后续增强）；槽位列表纯文本。

### 融合外观（服务端 + 客户端）
- 服务端：`V1.11.25` inventoryequipment.anvilItemId 列、`Equip.anvilItemId` getter/setter、`ItemFactory` INSERT 31→32 + load 侧 setAnvilItemId、`PacketCreator.addCharEquips` 视觉替换（anvilItemId!=0 时外观换 skin id）、`UseCashItemHandler.itemType==590` 分支（skin/base 双槽校验+消耗+刷新）、侧栏叶子。
- **关键安全决策**：**没有**把 anvilItemId 追加到 `addEquip` 装备块尾部——复合包中客户端无法用 length 可靠判断该字段存在，强行 writeInt 会让旧客户端把下一项当 anvil 读（s9 exp 错位翻版）。自己角色外观由 `forceUpdateItem`+`addCharEquips` 视觉替换覆盖；他人天然正确。
- 客户端：`Net/Packets/FusionAnvilPackets.h`（USE_CASH_ITEM + 5900000 + short skinPos/basePos）、`UIAnvilDialog`（槽位 +/- 调整 + 应用）、`FeatureRegistry.fusionanvil`、侧栏叶子。

## 三、构建与部署
- 服务端：`mvn -q -pl gms-server clean package -DskipTests` EXIT=0；启动后 8686 监听、Flyway 全部成功。
- 前端：`vue-tsc` sideMenu 0 错；`vite build` EXIT=0（dist 已更新）。
- 客户端：全量构建 `build_damageskin_ui_20261123.bat` EXIT=0；live `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 已同步，SHA256=`1FB930C47803C2679F630B77939E523286B6199E1DE74681AC3C472731F43284`，备份 `MapleStory.exe.bak-allfeatures-20261123`。
- 注意：新增了 3 个 UIElement::Type（DAMAGESKINPICKER/BEAUTYSALON/ANVILDIALOG），EnumMap 尺寸变化，必须全量构建（已做）。

## 四、实测与真机状态（如实）
- 编译/构建：全通过（Java mvn compile/package、gms-ui vite build、客户端全量 build EXITCODE=0）。
- **接口实测通过**（JWT @ 8686）：
  - `/sideMenu/v1/get` → code=20000，revision=1，items=23，4 目录（日常/战斗/活动/工具）+ 三扩展叶子 damageskin/beautysalon/fusionanvil，JSON 合法可解析。
  - `/sideMenu/v1/feature/list` → 25 项，三项扩展 enabled=true，中文 UTF-8 正确（curl 终端显示乱码是其 ANSI 问题，原始 JSON 字节为正确 UTF-8）。
  - `/sideMenu/v1/save` → code=20000，revision 1→2；随后 `/revision` = 2。写链路 + 版本号递增闭环。
  - Flyway：V1.11.22/23/24/25 全部成功（V1.11.25 首次因 COMMENT 位置语法错误失败，已修 SQL + 清理 flyway 失败记录后重跑成功）。
- **真机客户端联调未做**：需用户启动新客户端+登录，验证侧栏分类树 flyout、伤害皮肤 picker、美容院 UI、融合外观 UI 的实际交互与外观替换。
- 已知限制：伤害皮肤 catalog 目前是示例 id（1/2），需管理员按 `Effect/DamageSkin.img/<id>` 实际资源校正；美容院无 avatar 冻结预览；融合外观自己角色的背包图标仍显示真 itemId（外观渲染层已替换，符合契约），anvil 道具 5900000 需游戏内实际拥有才能用。
- 登录器回滚风险：部署后已延迟复核 SHA 仍一致（`1FB930…F43284`）；后续启动若被回滚需重新同步。
