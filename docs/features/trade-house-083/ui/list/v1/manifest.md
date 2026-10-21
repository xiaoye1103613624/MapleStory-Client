# UI manifest — list / v1

- 样式来源：原生
- scale：1x
- 目标尺寸：见下表
- 状态列表：
  - ColHeader：`unsorted` / `asc` / `desc`
  - Row：`normal` / `mouseOver` / `selected`
  - Scrollbar_top / mid / bottom / thumb：`State: static`（零件）
- 样板：无（v083 经验基线）
- 采样路径 / 客户端：未实测（见 `notes/client-path.md`）
- 目标节点路径：待定
- 文件→节点映射：见下表
- psd：./list.psd
- 组件列表：
  - ColHeader_unsorted|asc|desc（80×16，右缘排序箭头差分）
  - Row_normal|mouseOver|selected（200×18 行底 tile；selected 含左侧青蓝强调条）
  - Scrollbar_top（16×16 上箭头块）
  - Scrollbar_mid（16×32 轨道）
  - Scrollbar_bottom（16×16 下箭头块）
  - Scrollbar_thumb（12×20 滑块）
- 生成说明：ExtendScript 像素硬边；表头木色凹凸，行底羊皮纸；选中行浅蓝高亮 + 左强调条；滚动条木色斜切
- 验收：§13 本批尺寸/状态/禁区观感已过；待路径后 v2 校准

## 文件表

| 文件 | 尺寸 | 说明 |
| --- | --- | --- |
| ColHeader_unsorted.png | 80×16 | 未排序 |
| ColHeader_asc.png | 80×16 | 升序箭头 |
| ColHeader_desc.png | 80×16 | 降序箭头 |
| Row_normal.png | 200×18 | 常态行 |
| Row_mouseOver.png | 200×18 | 悬停行 |
| Row_selected.png | 200×18 | 选中行 |
| Scrollbar_top.png | 16×16 | 上 |
| Scrollbar_mid.png | 16×32 | 中 |
| Scrollbar_bottom.png | 16×16 | 下 |
| Scrollbar_thumb.png | 12×20 | 滑块 |
| list.psd | 400×220 | 源 |
