# 进游戏黑屏 + 双击选角修复

> 状态：已闭环（2026-09-28）；**同日续修**：全局 atlas 误杀 1280×2540 地图背景  
> 适用：BeiDou gms083 / MapleStory-Client（HeavenClient 系）  
> 产物目录：`docs/features/enter-game-blackscreen-fix/`  
> 部署路径：`E:\MXD\BeiDou-Client-xy\`

## 1. 现象

1. 选角界面双击角色无法直接进游戏（「开始」按钮可进）。
2. 点「开始」进游戏后画面纯黑，**地图 BGM（如 `FirstStepMaster`）在播**，点击可未响应。
3. 运行目录曾出现巨量 `_atlas_skip.log`：`skip atlas bitmap 1280x2540`（每帧刷屏）。

## 2. 根因

| 项 | 说明 |
| --- | --- |
| **主因（BGM+黑屏）** | `GraphicsGL::getoffset` 误用小地图的 **2048** 软上限，拒绝上传地图背景等大图（如 1280×2540）；Stage 已 load、BGM 已播，但绘制全是空纹理 → 纯黑 |
| 双击窗过紧 | `Window` 仅认 10–200ms 两次 mouse-up |
| 双击槽位索引 | 绝对角色索引当页内按钮下标 |
| 近黑慢淡入 | fade 后 opacity≈0.025，首帧卡顿时长期黑罩 |
| 进图清空 atlas | 首次进游戏也 `unload_atlas()` 加重首帧 |

## 3. 修复

| 文件 | 改动 |
| --- | --- |
| **`Graphics/GraphicsGL.cpp`** | **去掉全局 2048 拒绝；仅拒绝超过 atlas 8192 的图；小地图仍由 UIMiniMap 门禁** |
| `IO/Window.cpp` | 双击窗 `>10 && <500`；fade 后 `opacity=1` |
| `IO/UITypes/UICharSelect.cpp` | 双击用页内槽位索引 |
| `Net/Handlers/SetFieldHandlers.cpp` | 仅换图 `unload_atlas`；重步骤 `check_events` |
| `Graphics/GraphicsGL.cpp` | `getoffset` 上传时泵事件 |
| `IO/UITypes/UIMiniMap.cpp` | canvas 元数据 + bitmap 实际尺寸双门禁 |

## 4. 验证

- [x] Release\|x64 构建 → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- [x] 证据：进图前 `_atlas_skip.log` 大量 `1280x2540`；修复后不再按 2048 跳过
- [ ] 人工：`admin` / `admin` 进图应能看到地图（非纯黑），BGM 正常
- [ ] 人工：双击选角；换图无长期黑屏

## 5. 回滚

还原 `GraphicsGL.cpp` 的 atlas 上限逻辑与其它上表文件即可。

## 6. 预留

- 超大小地图可后续降采样显示。
- 完整 v83 StatusBar 见 `docs/features/v83-hud-quest/`。
