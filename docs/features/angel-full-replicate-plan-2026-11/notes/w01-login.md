# W01 login — 执行记录

## 对照

- IDA：`user-ida-pro-mcp` → `` `Angel.i64 ``（`server_health.idb_path`）
- 已有：[login-flow-angel-calibration-2026-11](../../login-flow-angel-calibration-2026-11/README.md)

## 本波改动

1. **LAST_CONNECTED_WORLD → set_focus_world**（已落地）  
   - `IO/UITypes/UIWorldSelect.{h,cpp}`：`set_focus_world` / `pending_focus_world_id`  
   - `Net/Handlers/WaveAShellHandlers.cpp`：`LastConnectedWorldHandler` 接线

2. **60 频道翻页（本轮阻塞解除）**  
   - Angel 证据：`CLogin::ChangeStep` step1 分支 `sub_5FDD2D(15)` / `sub_5FDD2D(60)`（容量 15 世界 / 60 频道）  
   - 实现：`CHANNEL_CAPACITY=60`、`CHANNEL_PAGE_SIZE=30`、可视槽仍 30；`channel_page` + `BtChPagePrev/Next`（`WorldSelect/alert/BtArrowL|R`）  
   - `channelid` 改为**逻辑频道**（跨页）；键盘 / SoftKey 先翻页再点槽  
   - 文件：`IO/UITypes/UIWorldSelect.h`、`IO/UITypes/UIWorldSelect.cpp`

## 资源备注（不破 W1）

- orange-wz：`Login.img/WorldSelect/channel` 仅有 `0..19` + `chSelect/chgauge/chEvent`（无 `20..59` 独立位图）  
- 第 2 页复用同页槽位图（标签可能仍显示 CH.1..30 样式），但发包 / 选中用逻辑下标 30..59  
- **不**盲补 WZ 频道位图；若需像素级 CH.31+ 标签，另立资源面

## 未改 / 保留

- `UIRecommendWorld` / BtViewChoice 已接线  
- `channel_count<2` 旧 P0 已销案  
- 多分辨率 / i18n / IME / s9 opcode 未动

## 验收

- 构建：见 README 本轮摘要  
- 联机：未实机（需服务端 `channel_count>30` 的世界才能看到翻页钮）
