# Wave 1 · 一致性小票（报警跟随角色 / 键位持久 / 大地图玩家层 / 进图预热）

> 日期：2026-11-04 ｜ 构建：`tools/build_smoke.bat` GATE OK / EXITCODE=0（多轮，产物 exe 18:49）
> 事实源：s9 源码逐字段对账（`E:\project\BeiDou-Server_s9`）；真机 auto-login + perf 探针验收。

## 交付清单

| # | 票 | 状态 | 关键证据 |
| --- | --- | --- | --- |
| 1.1 | HP/MP 报警阈值跟随角色 | ✅ 上行+回灌+探针 | s9 `SetHpMpAlertHandler`（byte×2 step 0..19，`HpMpAlertService`: step/20=百分比）；回灌 `UpdateHpMpAlertHandler` 写回 `ui.sysopt.*_warn`；commit 点 `to_step=(pct+2)/5` 钳 0..19 |
| 1.2 | 快捷栏触发键存储/下行 | ✅ 存储闭环；⏸ 改键 UI 接线显式预留 | s9 `QuickslotBinding.encode`：QUICKSLOT_INIT(0x9F)= bool+8×int=**触发键**（非槽绑定，槽绑定走 KEYMAP135）；旧 handler 把触发键写进键位表槽号（`(key&0xFF)` 有污染常规键位风险）→ 改存 `Keyboard::quickslot_keys[8]`；`ChangeQuickslotPacket(183)` 登记（8×int） |
| 1.3 | WORLD_MAP_PLAYERS(376) 回包 | ✅ | `WorldMapPlayersHandler` 逐字段对账 s9（int mapid/short count/N×{string,short,byte}）；选中即请求+陈旧应答丢弃+右上区渲染；`select_map_spot` 切图清层 |
| 1.4 | PICK_ALL_CHAR | ✅ **证伪为已实现** | `UIViewAllChar.cpp:161/201/213` 三处派发 `SelectCharPacket`/`SelectCharPicPacket`——gap 表登记陈旧，本票销案 |
| 1.5 | RELOG(28) | ✅ **裁决"明确不做"（证据反转）** | s9 `RelogRequestHandler.validateState=!isLoggedIn`，收到只回 `RELOG_RESPONSE(byte 1)`，零重登逻辑；本端发送只会触发垃圾 UIAdmin tip。gap-inventory 原"可实现"分类按此收紧；枚举不占位（维持"枚举全有构造点"门禁不变量），注释+本档记录裁决 |

## 实现要点

- **上行判据探针**：commit 点 `[PERF] HPA out hp=X mp=Y pct=A/B`；回灌点 `[PERF] HPA hp=X mp=Y`；QSI 点 `[PERF] QSI custom=0|1 [8 键值]`（Release 唯一观测通道 = debug.perf_log）。
- **真机实测**（auto-login，map=16 出发点）：`[PERF] t=25779ms QSI custom=0`——服务端 0x9F 下行被正确接收并走"等于默认→清表"分支；`SETFIELD total cum_ms=22`——新图预热后淡入回调 22ms。
- **HPA 下行未出现 = 服务端配置正确**：s9 `use_server_auto_pot=false`（V1.7.0 迁移种子默认）→ `PlayerLoggedinHandler:230` 门不开、不发 updateClientSettings。开启该项属服务端配置变更（碰库），按门禁 17 未经同意不动；上行路径（commit→dispatch→s9 收→step 限幅入库）已代码级对账，待该开关放开即可端到端。
- **本地默认表与 s9 `DEFAULT_QUICKSLOTS` 逐项一致**（0x2A,0x52,0x47,0x49,0x1D,0x53,0x4F,0x51）；键值全 <0x100 无 byte 截断撞车。
- 换角色串号防护：`custom=false` 分支强制 `clear_quickslots()`（sendMacros 同款 switching-characters 问题的 quickslot 版）。

## 显式预留（门禁 9）

1. **1.2 改键 UI 接线**：v83 KeyConfig 无原生快捷页编辑面（KEYSETTING 钮仅建未成页）；待 Wave 6 原生窗体线实体化后把 `ChangeQuickslotPacket` 接上。存储层已就绪，接线是纯 UI 活。
2. **1.1 端到端**：需 s9 `use_server_auto_pot=true`（DB 配置，等用户授权）；打开后预期日志序列：点确定→`HPA out`，重登→`HPA hp=…`。

## 触碰文件

`IO/Keyboard.{h,cpp}`、`IO/UI.cpp`、`IO/UITypes/UIOptionMenu.cpp`、`IO/UITypes/UIWorldMap.{h,cpp}`、`Net/OutPacket.h`、`Net/Packets/MiscOutPackets.h`、`Net/Packets/SkillMacroPackets.h`、`Net/Handlers/WaveAShellHandlers.cpp`、`Net/Handlers/WorldMapHandlers.{h,cpp}`(新)、`Net/PacketSwitch.cpp`、`MapleStory.vcxproj`。

## 矩阵读数（gen_opcode_matrix.py）

c2s MATCHED 99 → **105**；s2c UNHANDLED_EMITTED 3 → **2**（剩公会副本 287/288）；`client enums never constructed = 0`（RELOG 不占位裁决保持该不变量）。
