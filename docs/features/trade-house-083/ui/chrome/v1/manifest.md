# UI manifest — chrome / v1

- 样式来源：原生
- scale：1x
- 目标尺寸：见下表（逻辑像素）
- 状态列表：
  - frame/* · titleBar · panel_bg · listBg · searchBg · divider：`State: static`
  - BtClose：`normal` / `mouseOver` / `pressed` / `disabled`（MapleButton 四态）
- 样板：无（v083 经验基线）
- 采样路径 / 客户端：未实测（见 `notes/client-path.md`）
- 目标节点路径：待定（`UIWindow.img/.../TradeHouse/` 入库时再定）
- 文件→节点映射：见下表
- psd：./chrome.psd
- 组件列表：
  - frame/nw, frame/n, frame/ne, frame/w, frame/c, frame/e, frame/sw, frame/s, frame/se（九宫格，角 16×16，边可平铺）
  - titleBar（22px 高青蓝标题条样板条）
  - panel_bg（米黄羊皮纸底板 tile）
  - listBg（列表区略深羊皮纸 tile）
  - searchBg（内凹搜索底 64×18）
  - divider（分隔线 80×2）
  - BtClose/normal|mouseOver|pressed|disabled（13×13）
  - assembly_preview.png（640×420 拼装预览参考）
- 生成说明：Photoshop ExtendScript 像素硬边绘制（v083 木框/羊皮纸/青蓝标题）；BtClose 四态分别绘制（pressed 压暗、mouseOver 提亮、disabled 降饱和）；整窗预览由小组件层叠拼装，非单张烤死
- 验收：§13 清单本批已过观感/尺寸/多状态/分层；待客户端路径后可 v2 对照样板校准

## 文件表

| 文件 | 尺寸 | 说明 |
| --- | --- | --- |
| nw.png / ne / sw / se | 16×16 | 角 |
| n.png / s.png | 32×16 | 水平边 |
| w.png / e.png | 16×32 | 垂直边 |
| c.png | 32×32 | 中心（羊皮纸） |
| titleBar.png | 96×22 | 标题条 tile |
| panel_bg.png | 48×32 | 内容底 |
| listBg.png | 48×32 | 列表底 |
| searchBg.png | 64×18 | 搜索底 |
| divider.png | 80×2 | 分隔 |
| BtClose_*.png | 13×13 | 关闭四态 |
| assembly_preview.png | 640×420 | 拼装预览 |
| chrome.psd | 860×460 | 源：终稿区+素材区 |
