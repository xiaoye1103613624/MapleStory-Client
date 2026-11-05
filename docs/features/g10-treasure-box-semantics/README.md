# G10 — TreasureBox 触发语义纠正

> 状态：代码已闭环；真机 → pending-live-tests  
> 承接：`map-object-audit` G10  
> 日期：2026-11-06  
> 产物：`docs/features/g10-treasure-box-semantics/`

## 1. 选型

本轮在 open-issues / parity / pending 中跳过 step4、婚礼/嘉年华、Title shell、DualMobGauge、等真机项后，选 **G10**：证据充分、纯客户端可闭合。

| 候选 | 证据充分度 | 结论 |
| --- | --- | --- |
| **G10 TreasureBox** | s9 `UseTreasureChestHandler`→`UseTreasureBox(int)`；HitReactor 对任意 `state>0` 超发 UI | **本轮闭合** |
| atlas_dbg 清理 | 工程债，非玩法缺口 | 下一项候选 |
| C9 EXP 数字位 | coords 已落地，缺新截图差异证据 | 等真机/截图 |
| G8/G9 | 缺 SpecialEff / status 资源 | 显式预留 |

## 2. 证据

| 源 | 结论 |
| --- | --- |
| s9 `UseTreasureChestHandler` | 双击 ETC `4280000/4280001` + 钥匙 → `UseTreasureBox(itemId)` |
| s9 `PacketCreator.UseTreasureBox` | `SUCCESS_IN_USE_GACHAPON_BOX` + **`writeInt(type)`** |
| 客户端 `UIItemInventory` | 已发 `UseTreasureChestPacket(slot, item_id)` |
| 修前 `HitReactorHandler` | `state>0` 即 `emplace<UITreasureBox>` + 英文 `"Chest opened"` 场标 → **任意反应堆超发** |
| 修前 `GachaponBoxHandler` | 误读 **byte** 并开 `UIRemoteGachapon`（远程扭蛋壳） |
| NX `UIWindow.img/TreasureBox/{0,1}` | 各有 `backgrndClosed` / `backgrndOpened` 170×172 |

**【已证实】** 金/银宝箱 UI 属**道具开箱结果**，不是地图 HitReactor。  
**【差异】** HitReactor 超发窗+英文场标；入站包绑错 RemoteGachapon 且宽度错（byte vs int）。  
**【推荐】** HitReactor 只 `reactors().trigger`；入站开 `UITreasureBox` 并按 itemId 选皮肤。无待决项。

## 3. 实现

- `HitReactorHandler`：删除 TreasureBox / `"Chest opened"` 分支
- `GachaponBoxHandler`：读 `int itemId` → `UITreasureBox::apply_result`
- `UITreasureBox`：`load_variant(0|1)`（4280000→0 / 4280001→1）；撤构造期 SoftKey 假开箱

## 4. 验证

- 构建：见 `builds/`
- 真机（本轮不跑）：踩普通 Reactor 不应弹宝箱窗；双击金/银宝箱（有钥匙）应见 opened 皮肤

## 5. 触碰

仅客户端 `MapObjectHandlers` / `WaveAShellHandlers` / `UITreasureBox` + 本文档 / audit / open-issues / pending；未动服务端。
