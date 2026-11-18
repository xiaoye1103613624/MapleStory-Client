# UserInfo.img 按钮节点 → 语义映射（2026-11-16，orange-wz 128B key 实测）

## 方法
从 `evidence/img_userinfo_button_origins.json`（orange-wz get_node_detail 导出的各 Bt*/normal/0 的 PNG base64）解码，拼成 `evidence/img_userinfo_buttons_sheet.png` 联系表，直接读按钮上烘焙的中文文字确定语义。这是运行时部署数据（E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow.img），非陈旧打包源。

## 映射表（权威）

| 节点 | 烘焙文字 | 尺寸 | 参照图位置/用途 |
|---|---|---|---|
| BtParty | 组队邀请 | 59×17 | 顶部左行(148,22)，无队伍时灰禁用 |
| BtTrade | 交易邀请 | 59×17 | 顶部左行下(148,44)，非可交易态灰禁用 |
| BtItem | 道具 ▶ | 60×19 | 顶部右行(209,22)，粉红，打开右侧道具子窗 |
| BtWish | 想买的 ▶ | 60×19 | 顶部右行下(209,44)，粉红 |
| BtPresent | GIFT ▶ | 45×13 | 礼物入口 |
| BtPetHide | 宠物 ▲ | 48×17 | 底部(93,173)，宠物子窗收起 |
| BtPetShow | 宠物 ▼ | 49×17 | 底部展开态 |
| BtTamingHide | 骑宠 ▲ | 47×17 | 底部(5,173) |
| BtTamingShow | 骑宠 ▼ | 47×17 | 底部(5,173)展开态（参照图是▼） |
| BtBookHide | BOOK ▲ | 48×17 | 手册子窗收起 |
| BtBookShow | BOOK ▼ | 48×17 | 手册子窗展开 |
| BtExceptionShow | 排除道具 ▶ | 71×17 | 底部(198,173)蓝，排除道具子窗 |
| BtExceptionHide | + (12×12) | 12×12 | 添加按钮（人气行箭头候选） |
| BtMeso | 金币 | 31×17 | 金币相关 |
| BtRegister | 登录 | 31×17 | 登录相关 |
| BtDelete | 删除 | 31×17 | 删除 |
| BtSearch | 搜索 | 38×17 | 搜索 |
| **BtFamily** | **学院(蓝房子)** | **40×37** | **顶部左(100,20) 学院按钮！当前代码误当家族功能且坐标未设** |
| BtCollectionHide | 收藏 ▲ | 49×17 | 底部(147,173) |
| BtCollectionShow | 收藏 ▼ | 49×17 | 底部(147,173)橙选中态（参照图收藏子窗打开） |
| BtPet1/2/3 | 宠物槽(带1/2/3角标) | 30×27 | 宠物子窗内3个槽 |
| Marry / BtClose / BtPopUp / BtPopDown | 节点不存在 | - | 不可引用，须 guard/替代 |

## 关键结论

1. **学院按钮 = BtFamily**（烘焙文字"学院"+蓝房子，40×37）。当前 UICharInfo 把 BtFamily 坐标设为默认(0,0) 且接线到家族逻辑，是错的根本原因之一。正确：放在顶部左 x=100,y=20，语义=学院(新手引导)。
2. **人气 ↑↓ 箭头**：IMG 无 BtPopUp/BtPopDown；参照图 x=232..263,y=113..127 是两个 ~14×14 灰箭头按钮。候选是 backgrnd 烘焙 + 小节点，需实机/进一步取证；暂按小按钮贴图或占位处理（详见 r04）。
3. **底部 4 按钮**尺寸与 IMG 完美对应：骑宠(47×17)/宠物(49×17)/收藏(49×17)/排除道具(71×17)。参照图里"收藏"橙色=Show 态，其余灰=各自态。
4. **顶部 2×2 网格**：组队邀请/交易邀请(BtParty/BtTrade 59×17) + 道具/想买的(BtItem/BtWish 60×19)，尺寸与 IMG 一致。
5. 当前代码引用的 BtPopUp/BtPopDown/BtDamage/BtPersonality/BtArrayGet/BtArrayName/BtFAQ/character[BtXxx] 等节点在运行时大多不存在（扁平 UserInfo），须移除或 guard，不能臆造贴图（R6）。
