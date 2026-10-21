# UI manifest — assembly / v1

- 样式来源：原生
- scale：1x
- 目标尺寸：主窗预览 640×420；BidDialog 280×160
- 状态列表：整窗为**拼装预览**（控件态取代表性 normal/selected；非全态烤盘）
- 样板：无（v083 经验基线）；零件来自同功能 `ui/*/v1/`
- 采样路径 / 客户端：未实测（见 `notes/client-path.md`）
- 目标节点路径：待定（`UITradeHouse` / `UITradeHouseBidDialog`）
- psd：各场景独立 PSD（见下表）；图层保留零件位图层 + 可编辑文字层；右侧 MATERIALS 素材区
- 组件列表（引用）：
  - chrome · tabs · list · buttons · grid · inputs · overlay · feedback（均 v1）
- 生成说明：以 `chrome/assembly_preview` / `overlay/BidDialog_preview` 为壳，叠加页签/列表/按钮/格/输入/角标；英文占位标签（拼装可读性）；中文文案入库前替换
- 验收：§13 拼装非烤死、PSD 分层、R1 久存角标在 WonStash 可见、R3 已拍领取底栏可见；未做「筛选展开 640×480」与「上架·拍卖」独立预览（见残留）

## 场景文件

| 文件 | 尺寸 | 要素 |
| --- | --- | --- |
| BrowseFixed.png/.psd | 640×420 / 860×440 | 浏览·直售：主 Tab、模式/分类、搜索、表头排序、行态、购买底栏 |
| BrowseAuction.png/.psd | 640×420 / 860×440 | 浏览·拍卖：拍卖模式、BadgeAuction、Countdown warn、BtBid |
| ListForm.png/.psd | 640×420 / 860×440 | 上架·直售：模式、背包格选中、数量/单价/时长、BtList |
| WonStash.png/.psd | 640×420 / 860×440 | 已拍 10×5、占用文案、领取/全部领取、**BadgeStashOld（R1）** |
| Records.png/.psd | 640×420 / 860×440 | 记录子 Tab、只读行、EmptyState 示意 |
| BidDialog.png/.psd | 280×160 / 520×220 | 出价弹层：当前价/最低加价、FieldBid focus、冻结提示、取消/确认 |

## 残留（非阻塞 · 可 v2）

- 浏览·装备筛选展开（640×480）未单独出图
- 上架·拍卖（时长只读 1h）未单独出图（ListForm 仅直售档）
- 文案现为英文占位，待客户端字体/GBK 策略后换中文
- 无客户端路径实测校准（v083 基线）
