# UI manifest — tabs / v1

- 样式来源：原生
- scale：1x
- 目标尺寸：见下表
- 状态列表：
  - MainTab：`normal` / `selected` / `mouseOver`（TwoSpriteButton + 可选悬停）
  - CategoryTab / ModeTab / RecordSubTab：`normal` + `selected`
- 样板：无（v083 经验基线）
- 采样路径 / 客户端：未实测（见 `notes/client-path.md`）
- 目标节点路径：待定
- 文件→节点映射：见下表
- psd：./tabs.psd（与 buttons 同源素材表 `TH_tabs_buttons_v1`；`buttons/v1/buttons.psd` 为同内容副本）
- 组件列表：
  - MainTab_normal|selected|mouseOver（72×20）
  - CategoryTab_normal|selected（52×18）
  - ModeTab_normal|selected（56×18）
  - RecordSubTab_normal|selected（48×18）
- 生成说明：ExtendScript 像素硬边；未选页签凹暗木色，选中页签羊皮纸色并与内容底衔接（底边无暗线）；标签文字层可在拼装阶段叠加
- 验收：§13 本批尺寸/二态/禁区观感已过；待路径后 v2 校准

## 文件表

| 文件 | 尺寸 | 状态 |
| --- | --- | --- |
| MainTab_normal.png | 72×20 | 未选 |
| MainTab_selected.png | 72×20 | 选中 |
| MainTab_mouseOver.png | 72×20 | 悬停 |
| CategoryTab_normal/selected.png | 52×18 | 二态 |
| ModeTab_normal/selected.png | 56×18 | 二态 |
| RecordSubTab_normal/selected.png | 48×18 | 二态 |
| tabs.psd | 520×280 | 源 |
