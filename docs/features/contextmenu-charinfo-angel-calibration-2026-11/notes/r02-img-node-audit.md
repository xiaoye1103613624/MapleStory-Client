# r02 — 运行时 UIWindow.img 节点核验（orange-wz 128B key）

来源：`evidence/img_runtime_nodes.md`（orange-wz get_node_tree_json，部署 `E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow.img`，exclusive=false，同会话 unload_all）。原始树 `evidence/img_ContextMenu_tree.txt` / `img_UserInfo_tree.txt`。

## ContextMenu（当前代码只用了 t/c/s + 8 按钮；运行时实际有 12 按钮）
- t = 100×23, c = 97×15, s = 97×7（origin 均 0,0）——与当前 `tile_t/c/s` 及 Angel `SP_2605/2606/2607` 一致。
- 按钮（四态 normal/pressed/disabled/mouseOver 齐全，首帧 canvas 尺寸）：
  - BtInfo 85×15, BtSave, BtWhisper, BtParty, BtGuild, **BtAlertGM**, **BtBlock**, BtClaim, BtTrade, BtFamily, **BtCashTrade**
  - 当前代码枚举只有：BtInfo/BtSave/BtTrade/BtWhisper/BtParty/BtGuild/BtClaim/BtFamily —— 缺 BtAlertGM / BtBlock / BtCashTrade 三个节点。

## UserInfo（扁平 v83 结构，无 character/farm/personality/collect/damage/item 子容器）
- 背景板：backgrnd 275×199、backgrnd2 275×399、backgrnd3 240×162、backgrnd4 240×162、backgrnd5 275×361、backgrnd6 159×147、backgrnd7 275×404。
- 其它位图：ReqLv 31×10、bossPetCrown 18×12。
- 按钮（各四态）：BtParty, BtTrade, BtItem, BtWish, BtPresent, BtPetHide, BtPetShow, BtTamingHide, BtTamingShow, BtBookHide, BtBookShow, BtExceptionShow, BtExceptionHide, BtMeso, BtRegister, BtDelete, BtSearch, BtFamily, BtCollectionHide, BtCollectionShow, BtPet1, BtPet2, BtPet3, Marry。
- **当前 UICharInfo.cpp 构造的 `UserInfo["character"]`（回退 UserInfo）、`UserInfo["farm"]`、`["personality"]`、`["collect"]`、`["damage"]`、`["item"]` 六个子容器在运行时均不存在**（扁平结构），导致：farm/personality/collect/damage/item 子面板与按钮永不创建；`character/BtPersonality` 等旧路径按钮缺失。
- 当前有的扁平兜底（`UserInfo["BtCollectionShow"]` / `BtPetShow` / `BtTamingShow` / `BtTrade` / `BtBookShow` / `BtFamily`）能创建部分主窗按钮，但大量真实节点（backgrnd3..7、BtWish/BtPresent/BtMeso/BtRegister/BtDelete/BtSearch/BtBookHide/Marry/bossPetCrown、BtCollectionHide/BtException*）从未接线。

## 关键结论
- 主背景 backgrnd=275×199（当前 UICharInfo dimension 由此定，正确）；但 Angel 个人情报窗随状态切换 backgrnd..7 多块板（SetState 1/2/3），当前代码只有单块 backgrnd + 不存在的 backgrnd2。
- 全部尺寸以运行时为准（AGENTS.md R1/R2）；打包源 UI.wz 已漂移，未采用。
- orange-wz 本次解析全部成功（128B GMS key + GMS IV），无 `Index N out of bounds` / `IMG 解析失败` / GBK 乱码，无需回退 .nx dump。
