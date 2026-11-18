# 退出商城黑屏 + BGM 不变 · RCA（2026-11-18）

> 用户报"出商城后界面一直黑，背景音乐还是商城的"。截图 `XiaoYe_2026-11-18_07-51-01.png`（本端，07:51:01）。
> 该截图早于商城九项 UI 修复的 07:53 部署，故属独立退出链路缺陷，不是九项 UI 改动引入。

## 1. 根因（逐字证据）

| 环节 | 位置 | 事实 |
| --- | --- | --- |
| 进商城 | `Net/Handlers/CashShopHandlers.cpp:827-836` | `Stage::clear()` → `Stage::load(-1,0)`，Stage 停在 **ACTIVE + CashShopPreview 预览地图** |
| Stage 状态守卫 | `Gameplay/Stage.cpp:64-71`（旧） | `load()` 仅当 `state==INACTIVE` 才 `load_map()`；ACTIVE 时是**空操作** |
| 退出商城 | `IO/UITypes/UICashShop.cpp:1371-1390` `exit_cashshop()` | 直接 `stage.load(真实mapid)`，是全仓 3 个 `Stage::load` 调用点里唯一没先 `clear()` 的 → 撞上空操作 |
| 世界绘制 | `Gameplay/Stage.cpp:179` | `state!=ACTIVE` 或仍是预览地图 → 游戏视角无内容（黑） |
| BGM | `Gameplay/Stage.cpp:168` `respawn()` | 只有真正 `load_map()` 才 `Music(mapinfo.get_bgm()).play()`；空转 → BGM 停在商城曲 |
| 运行时铁证 | `E:/MXD/BeiDou-Client-xy/_audio.log` 末行 | `Bgm00.img/GoPicnic` = `MapInfo.cpp:44` 中 bgm 节点为空时的默认值；CashShopPreview 无 bgm → 正是商城预览曲；真实地图 BGM 从未播放 |
| 触发路径 | `Net/Handlers/PlayerHandlers.cpp:75-78` | `ChangeChannelHandler::handle` 调 `exit_cashshop()`；截图 "Changing channel..."（:89 的 tip）坐实走的是这条路径 |
| 服务端配合 | `BeiDou-Server_s9/.../ChangeMapHandler.java:188-207` `enterFromCashShop` | 出商城走 `getChannelChange`，服务端随后 SET_FIELD 权威刷新 |

## 2. 修复（最小改动，类级根治）

`Gameplay/Stage.cpp:62` — 把"仅 INACTIVE 才换图"改为不变量"**mapid 变化即强制 load_map**"：

```cpp
// mapid 成员未在构造期初始化，用 || 短路：INACTIVE 时不读未初始化值
if (state == State::INACTIVE || this->mapid != mapid)
    load_map(mapid);
```

进商城 `load_map(-1)` 会把 `Stage::mapid` 置 -1（Stage.cpp:137），退出 `load(真实mapid)` 满足 `mapid != -1` → 真正换图 → `respawn()` 按真实地图重播 BGM。三个调用点（SetField / CashShop enter / exit_cashshop）同源安全，无需改 `exit_cashshop` 本体。

`IO/UITypes/UICashShop.cpp:1371` — 仅更新注释说明链路，不改逻辑（避免手动 clear 与随后 SET_FIELD 的 clear→load 形成双重换图）。

## 3. 验证

- `build_cashshop9fix.bat` → EXITCODE=0，`error C/LNK` = 0
- 产物 exe mtime 08:10:06 > 源码 Stage.cpp 08:09:04 / UICashShop.cpp 08:09:48
- 部署前无客户端进程占用
- 部署 SHA（实时）：产物 = live = `0fcb2173478f1f1792ddf7a134327e349244faa14b7f5150c8ef3e8403131507`

## 4. 声明

- 真机未验收（agent 无 GUI 操作权）：请按原路径复核"出商城回地图 + BGM 切回"。
- 登录器可能回滚 exe，复核前实时核对 live SHA。
- 若复核仍黑：下一步查 `ChangeChannelHandler` 的 `parse_login`→重连→`SET_FIELD` 二次入场时序（已确认服务端会下发 SET_FIELD，见 setfield_trace.log）。
