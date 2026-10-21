# UI manifest — inputs / v1

- 样式来源：原生
- scale：1x
- 目标尺寸：见下表
- 状态列表：
  - Textfield / FieldNarrow / FieldBid：`idle` / `focus` / `disabled`
  - Combo：`idle` / `focus` / `disabled`
  - FieldQty：本批仅 `idle`（focus/disabled 复用 FieldNarrow 同色契约；拼装时 ± 钮用 buttons）
- 样板：无（v083 经验基线）
- 采样路径 / 客户端：未实测（见 `notes/client-path.md`）
- 目标节点路径：待定
- 文件→节点映射：通用底板；业务字段名（FieldKeyword 等）拼装时复用对应宽度
- psd：./inputs.psd
- 组件列表：
  - Textfield_*（96×18）— 关键词/名称等
  - FieldNarrow_*（48×18）— 价格区间/属性/等级
  - FieldBid_*（120×18）— 出价输入
  - FieldQty_idle（36×18）— 数量框
  - Combo_*（80×18 + 右缘下拉箭头区）— 职业/部位/时长
- 生成说明：内凹羊皮纸输入底；focus 青蓝描边；disabled 降饱和灰边；Combo 右侧斜切箭头块
- 验收：§13 三态可辨；待路径后 v2

## 文件表

| 文件前缀 | 尺寸 | 状态 |
| --- | --- | --- |
| Textfield_* | 96×18 | idle/focus/disabled |
| FieldNarrow_* | 48×18 | idle/focus/disabled |
| FieldBid_* | 120×18 | idle/focus/disabled |
| FieldQty_idle | 36×18 | idle |
| Combo_* | 80×18 | idle/focus/disabled |
| inputs.psd | 360×140 | 源 |
