# UI manifest — overlay / v1

- 样式来源：原生
- scale：1x
- 目标尺寸：BidDialog 预览 280×160；九宫格角 12×12
- 状态列表：
  - BidFrame/* · BidDialogBg · BidTitleBar：`State: static`
  - BidDialog_BtClose：本批单态 normal（四态可复用 `chrome/v1/BtClose_*`）
- 样板：无（v083 经验基线）
- 采样路径 / 客户端：未实测（见 `notes/client-path.md`）
- 目标节点路径：待定（`UITradeHouseBidDialog`）
- 文件→节点映射：见下表
- psd：./overlay.psd
- 组件列表：
  - BidFrame_nw/n/ne/w/e/sw/s/se（出价弹层九宫格）
  - BidDialogBg（羊皮纸底板 tile）
  - BidTitleBar（青蓝标题条 tile）
  - BidDialog_BtClose（13×13）
  - BidDialog_preview.png（280×160 拼装预览）
- 生成说明：小组件九宫格 + 标题条 + 底板拼装预览；按钮/输入复用 buttons/inputs；文字层拼装阶段叠加
- 验收：§13 本批静态件与预览已过；待路径后 v2

## 文件表

| 文件 | 尺寸 | 说明 |
| --- | --- | --- |
| BidFrame_*.png | 角12 / 边24×12或12×24 | 九宫格 |
| BidDialogBg.png | 40×28 | 底板 tile |
| BidTitleBar.png | 64×20 | 标题条 |
| BidDialog_BtClose.png | 13×13 | 关闭 |
| BidDialog_preview.png | 280×160 | 拼装预览 |
| overlay.psd | 520×200 | 源 |
