# 风筝贴图化（SPAWN_KITE / Cash 508）

> 状态：代码已闭环；真机 → pending-live-tests  
> 承接：`map-object-parity`「风筝/商店盒/Evan 龙贴图」预留；接 G11 后下一档  
> 日期：2026-11-06  
> 产物：`docs/features/kite-texture/`

## 1. 选型

本轮候选：① 风筝贴图（IDA `sub_531325`）② C4 BtSort/BtGather 点击语义。跳过 step4 / 婚礼嘉年华 / Title / DualMobGauge。

| 候选 | 证据充分度 | 结论 |
| --- | --- | --- |
| **风筝贴图** | IDA case 271→`sub_65ACDF` + NX `Cash/0508/iconReward` + s9 `spawnKite` | **本轮闭合** |
| C4 BtSort/BtGather | 素材/叠槽已修；`GatherResult→set_sort(true)` / `SortResult→set_sort(false)` 已在；本 IDB 无 `BtGather`/`CUIItem` 串锚，按钮回调表未钉 | **下一项**（需另开 WvsContext/UI 窗函数取证） |

## 2. 证据

| 源 | 结论 |
| --- | --- |
| IDA `sub_531325` | 场包分发；270–272 → `sub_65AC81` |
| IDA `sub_65AC81` | 270=`CANNOT_SPAWN_KITE`；271=`SPAWN_KITE`→`sub_65ACDF`；272=`REMOVE_KITE` |
| IDA `sub_65ACDF` | 读 oid/item/两字符串/x/y；绘制原点 `(x-3,y-100)`；`sub_5D3322(item)` 后取子键 **1588**（iconReward） |
| NX `Item/Cash/0508.img/0508000x/iconReward` | 真风筝动画（05080000：3 帧 65×80）；`info/icon` 仅背包小图标 |
| s9 `PacketCreator.spawnKite` | `oid+item+msg+name+x+ft`；第二 short 是 **foothold**，不是 y |

**【已证实】** 场上可视件 = `iconReward`，不是 `BasicEff/ItemUp`。  
**【差异】** 修前 `try_load_kite()` 错挂 ItemUp，且 `SpawnKiteHandler` 未传 itemid、把 ft 当 y。  
**【推荐】** Kind::KITE 专用加载 + 传 item + 脚底 y-100。聊天气泡壳另预留（本版宋体 A11M 白字）。无待决项。

## 3. 实现

- `FieldMarker::try_load_kite(item_id)`：`Item/Cash/0{id/10000}.img/0{extend_id7}/iconReward` → icon → ItemUp
- 构造：`Kind::KITE` 优先；508 区间走 iconReward；非 508（Weather 误用）先 ItemEff
- 文案：A11M 白字、无黄名牌底（对齐 G11 黑板字色路线）
- `SpawnKiteHandler`：解析 x/ft；位置 `(x-3, player_y-100)`；`spawn(..., item)`

## 4. 验证

- 构建：见 `builds/`
- 真机（本轮不跑）：放 508 风筝见 iconReward 动画 + 留言白字；收起/无法放置不崩

## 5. 触碰

仅客户端 `FieldMarker` + `SpawnKiteHandler` + 本文档 / parity / open-issues / pending；未动服务端。
