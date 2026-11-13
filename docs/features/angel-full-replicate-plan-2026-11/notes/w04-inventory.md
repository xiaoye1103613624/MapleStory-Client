# W04 inventory/item — 执行记录

## 目标

背包 / 装备窗 / Tooltip 语义与排版对齐；**白名单 Tooltip 类型不得改回**。

## 已有证据

- IDA r16 inventory  
- 工作区未提交：`EquipTooltip`/`ItemTooltip`/`TextTooltip`（BodyLine、ColorBox、半透明 α）

## 本波代码改动

**本波不新增实现**；确认工作区 Tooltip 改动属于白名单保留项，纳入本面「已对齐自研」：

- `Text` → `BodyLine`（冒号中线排版）  
- `IconBase` Texture → `ColorBox` 自绘灰底  
- `TooltipBackdrop` FILL_ALPHA=0.88 / EDGE_ALPHA=0.92  

未动：单件攻魔 short 宽度（level-cap 文档明示 tooltip 单件维持 short）。

## 阻塞

- 宠物装备三页穿戴（`UIEquipInventory` 预留）：依赖协议批次 → W11/gap  
- 联机未验证  

## 回滚

勿回滚白名单 Tooltip 类型。
