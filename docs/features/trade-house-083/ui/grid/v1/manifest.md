# UI manifest — grid / v1

- 样式来源：原生
- scale：1x
- 目标尺寸：见下表（已拍格 32×32；选中框外扩 1px → 34×34）
- 状态列表：
  - WonSlot：`empty` / `filled`（框）/ `selected`
  - BadgeStashOld：静态角标（R1 久存警告，叠于格右上）
- 样板：无（v083 经验基线）；格尺寸对齐 wireframes 10×5·32×32
- 采样路径 / 客户端：未实测（见 `notes/client-path.md`）
- 目标节点路径：待定
- 文件→节点映射：见下表
- psd：./grid.psd
- 组件列表：
  - WonSlotEmpty（32×32 空槽内凹）
  - WonSlotFrame（32×32 有物外框）
  - WonSlotSelected（34×34 高亮外环）
  - BadgeStashOld（10×10 红底叹号角标，R1）
- 生成说明：木框 + 羊皮纸内底；选中黄高亮外扩 1px；角标可叠在任意格角
- 验收：§13 尺寸/状态/R1 角标已过；待路径后 v2 对照 inventory 格样板

## 文件表

| 文件 | 尺寸 | 说明 |
| --- | --- | --- |
| WonSlotEmpty.png | 32×32 | 空格 |
| WonSlotFrame.png | 32×32 | 有物框 |
| WonSlotSelected.png | 34×34 | 选中（含 1px 外高亮） |
| BadgeStashOld.png | 10×10 | 久存警告角标 |
| grid.psd | 160×80 | 源 |
