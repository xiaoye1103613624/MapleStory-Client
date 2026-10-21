# UI manifest — buttons / v1

- 样式来源：原生
- scale：1x
- 目标尺寸：见下表
- 状态列表：各钮均为 MapleButton 四态 `normal` / `mouseOver` / `pressed` / `disabled`
- 样板：无（v083 经验基线）
- 采样路径 / 客户端：未实测（见 `notes/client-path.md`）
- 目标节点路径：待定
- 文件→节点映射：见下表
- psd：
  - `./buttons.psd`（首批：Search/Buy/Bid/Prev/Next/Qty±，与 `tabs/v1/tabs.psd` 同源）
  - `./buttons_rest.psd`（续批：List/CancelListing/Claim/ClaimAll/FilterToggle/StatusEntry）
- 组件列表（**已齐 PLAN §4 buttons**）：
  - BtSearch（木色 48×18）
  - BtBuy（蓝色 56×20）
  - BtBid（绿色 56×20）
  - BtList（木色 56×20）
  - BtCancelListing（木色 72×20）
  - BtClaim（青绿 56×20）
  - BtClaimAll（青绿 72×20）
  - BtFilterToggle（木色 56×18）
  - BtPrev / BtNext（20×18）
  - BtQtyUp / BtQtyDown（18×14）
  - BtStatusEntry（状态栏入口 28×28）
- 生成说明：像素硬边斜切；四态提亮/压暗/降饱和；空白标签区，拼装时叠文字层
- 验收：§13 四态可辨、硬边、无禁区风；待路径后 v2

## 文件表

| 前缀 | 尺寸 | 色系 | 源 PSD |
| --- | --- | --- | --- |
| BtSearch_* | 48×18 | 木色 | buttons.psd |
| BtBuy_* | 56×20 | 蓝 | buttons.psd |
| BtBid_* | 56×20 | 绿 | buttons.psd |
| BtPrev_* / BtNext_* | 20×18 | 木色 | buttons.psd |
| BtQtyUp_* / BtQtyDown_* | 18×14 | 木色 | buttons.psd |
| BtList_* | 56×20 | 木色 | buttons_rest.psd |
| BtCancelListing_* | 72×20 | 木色 | buttons_rest.psd |
| BtClaim_* | 56×20 | 青绿 | buttons_rest.psd |
| BtClaimAll_* | 72×20 | 青绿 | buttons_rest.psd |
| BtFilterToggle_* | 56×18 | 木色 | buttons_rest.psd |
| BtStatusEntry_* | 28×28 | 蓝灰 | buttons_rest.psd |
