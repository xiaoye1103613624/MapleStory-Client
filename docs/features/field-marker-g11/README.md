# G11 场标贴图化（Catch / Chalkboard / CharBox / ItemFX）

> 状态：代码已闭环；真机 → pending-live-tests  
> 承接：`map-object-audit` G11；`map-object-parity` 文本场标遗留  
> 日期：2026-11-05  
> 产物：`docs/features/field-marker-g11/`

## 1. 选型

本轮在 open-issues / parity / pending 中跳过 step4、婚礼/嘉年华、Title shell、已完成项与 BossHP DualMobGauge 后，选 **G11**：资源与串池证据充分，可纯客户端闭合。

| 候选 | 结论 |
| --- | --- |
| BossHP DualMobGauge | **否决**（既定：PartyRace 专用，非多目标队列） |
| C14 `set_stance` | 代码已钳 `animations.find`，台账过期 |
| TemporaryStatView / Title | 已有窗 / 显式预留 |
| **G11 场标贴图** | Catch/ItemEff/黑板 sample/店招 signboard 均有部署件 |

## 2. 证据

| 源 | 结论 |
| --- | --- |
| NX `Effect/BasicEff.img/Catch/{Success,Fail}` | Success 8 帧、Fail 5 帧位图 |
| V17 串池 `b1400c`/`b14000` | 字面量 `Catch/Success`、`Catch/Fail` |
| NX `Item/Cash/0537.img/05370000/info/sample` | 黑板可视件 80×68 |
| NX `Item/Cash/0503.img/05030000/skin/signboard` | 雇佣店招牌；回退 `UIWindow.img/PersonalShop/select` |
| NX `Effect/ItemEff.img` | ShowItemEffect 既有 `try_load_anim` 路径 |

**【已证实】** 四类场标均有原生贴图键；修前一律 `Kind::DOOR/SUMMON` + 黄字名牌。  
**【差异】** Catch 不播圈、黑板/店招无底板、ItemFX 叠英文「ItemFX」。  
**【推荐】** 增 Kind + 专用加载；有动画/底板时隐藏黄字。无待决项。

## 3. 实现

- `FieldMarker::Kind`：`CATCH` / `CHALKBOARD` / `CHARBOX` / `ITEMFX`
- Catch：`skill_id!=0`→Success，否则 Fail；handler `remove` 后重建
- Chalkboard：537 sample + A11M 白字（宋体路线）
- CharBox：503 signboard + 白字店名
- ItemFX：Kind::ITEMFX + itemid 走 ItemEff；无「ItemFX」名牌

## 4. 验证

- 构建：见 `builds/`
- 真机（本轮不跑）：捕捉球成败圈；黑板 537；开店店招；现金道具 ItemEff

## 5. 触碰

仅客户端 `FieldMarker` + `MapObjectHandlers` + 本文档 / audit / open-issues / parity；未动服务端。
