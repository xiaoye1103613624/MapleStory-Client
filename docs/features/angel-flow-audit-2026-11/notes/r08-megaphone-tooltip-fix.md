# r08 · 喇叭收端 + 装备 Tip 排版销案（2026-11-12）

> 对 r07 缺口开修 + 用户「图标旁前置例外、其余居中 / inc 冒号白字」裁决。

## 改动清单

| 文件 | 内容 |
| --- | --- |
| `Net/Handlers/MessagingHandlers.cpp` | type**8** 道具喇叭、type**0x0A** 多段喇叭独立分支；Chat+FloatNotice；不再 drain 丢字段 |
| `Net/Handlers/WaveAShellHandlers.cpp` | Avatar：先 `int itemId` 再 name+4×line+channel+ear；Chat MEGAPHONE |
| `Gameplay/Combat/Combat.cpp` | DamageReactor 填 `result.stance` / `result.skill` |
| `IO/Components/EquipTooltip.h/.cpp` | cash/mesos 徽标；交易限制行；BodyLine 居中时名/冒号/值同色；分隔线下全冒号中轴；desc CENTER |
| `IO/Components/ItemTooltip.cpp` | desc → TOOLTIPBODY |
| `locale/zh-CN.ini` / `en-US.ini` | `equip.trade.*` |

## inc 冒号居中 / 白字如何实现

1. `BodyLine::build(..., valuecolor, center_colon=true)`：名与冒号也用 `valuecolor`（不再死绑 TOOLTIPBODY）。
2. `statlabels[es].build(..., WHITE, true)` → 力量/敏捷… **名+冒号+值全白**，冒号钉在 `BODY_CENTER_X=132`。
3. 元信息（ID/分类/速度/升级/锤子）同为 `center_colon=true`，色为 TOOLTIPBODY。
4. **前置块不动**：图标、`Can/Cannot` 需求、成长、职业横排仍在分隔线上方；cash 徽标贴图标旁。

## 构建

- MSBuild Release\|x64 → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- evidence：`evidence/p4-msbuild.txt`

## 真机验收

1. 道具喇叭 / 三段喇叭 → Chat MEGAPHONE + FloatNotice，带 `[Ch.N]`
2. 情景喇叭 Avatar → 文案可读、频道前缀正确（非整包乱码）
3. 装备 tip：图标旁需求色正常；分隔线下属性 **冒号对齐且全白**；现金装有 cash 标；不可交易有橙字行
4. 普通道具 tip 描述为奶油色

## 仍预留

| 项 | 说明 |
| --- | --- |
| type8 道具图标浮层 | 现只展示文案；`addItemInfo` 完整解析另立 |
| type 0x0B gachapon 绿字 | 未本轮专项 |
| Avatar look 渲染 | 收包后仍 drain look，未画角色立绘 |
| 潜能行 083 对齐 | potflag 保留，未按 Angel 逐字段复核 |
| mesos 徽标触发条件 | 现仅非 cash 且节点存在时备用；无独立「金币价」字段接线 |
