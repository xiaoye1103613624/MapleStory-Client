# r06 — 个人情报窗全功能/全子窗补齐取证表

## 1. 服务端 CHAR_INFO 字段顺序（权威：s9 PacketCreator.charInfo，PacketCreator.java:2863-2943）

```
writeInt   chr.id
writeShort chr.level
writeShort chr.job.id
writeShort chr.fame
writeByte  marriageRing!=null ? 1 : 0
writeString guildName
writeString allianceName
writeByte  0                       // pMedalInfo
// 宠物循环：最多3只
loop i in 0..2:
  if pets[i]!=null:
    writeByte 1 (true)
    writeInt   pet.itemid
    writeString pet.name
    writeByte  pet.level
    writeShort pet.tameness        // 亲密度
    writeByte  pet.fullness        // 饱清感
    writeShort 0
    writeInt   chr.petEquipItemId(i)
writeByte 0                        // 宠物结束哨兵
// 坐骑
if mount valid:
  writeByte mount.id
  writeInt  mount.level
  writeInt  mount.exp
  writeInt  mount.tiredness        // 疲劳度
else writeByte 0
writeByte wishlist.size
loop: writeInt wishItemId
writeInt book.bookLevel
writeInt book.normalCard
writeInt book.specialCard
writeInt book.totalCards
writeInt book.coverMobId
writeInt medal.itemId (or 0)
writeShort medalQuests.size
loop: writeShort questId
```

**装备明细：服务端不下发**（v83 协议本身）。他人装备列表无法在客户端单侧补齐——协议边界，不臆造。角色外观立绘由本地 CharLook 已有数据渲染。

**排除道具**：`CharInfoRequestHandler:41-44` 对他人额外调 `exportExcludedItems` → `PET_EXCEPTION_LIST`(客户端 opcode 173, 已有 `PetExceptionListHandler` 但当前只 LOG 丢弃)。结构是 cid(int)+petIdx(byte)+petId(long)+excluded item ids。

## 2. 运行时 UserInfo.img 子窗背景板语义（evidence/img_backgrnd*.png，已导出，坐标为原图像素）

| 节点 | 尺寸 | 语义 | 用途 |
|---|---|---|---|
| backgrnd | 275×199 | 默认主窗 | 已用（上一轮对齐） |
| backgrnd2 | 275×399 | 宠物子窗 | 整窗展开：宠物立绘框 + 种类/等级 + 亲密度/饱清感 |
| backgrnd3 | 240×162 | **想买的（心愿单）** | 右侧浮层：标题"想买的" + 6 行 + 滚动条 |
| backgrnd4 | 240×162 | **道具** | 右侧浮层：标题"道具" + 6 行 + 滚动条 |
| backgrnd5 | 275×361 | 坐骑子窗 | 整窗展开：种类/等级 + 经验值/疲劳度 |
| backgrnd6 | 159×147 | 排除道具列表 | 右侧浮层（159 宽） |
| backgrnd7 | 275×404 | 勋章/图鉴子窗 | 整窗展开：等级/手册名/总卡片 + 一般/特殊 + 获得过的勋章 + 滚动条 |

### 子窗数据行坐标（PIL 检测浅蓝行带，原图像素）
- backgrnd3/4（240×162）：数据行 y = 31,49,71,89,111,129（每行 16px，行高带 [31-47]..[129-145]）；左侧图标槽 x≈15-45，文字区 x≈52 起；滚动条在最右 x≈225-238。
- backgrnd2/5/7：整窗展开板，字段标签已烘焙在背景里，数据填在标签右侧的行框（执行阶段用 PIL 网格标注精确 x）。
- backgrnd6（159×147）：列表区 y≈18-120，滚动条 x≈145-156。

## 3. 按钮 IMG 节点（UserInfo.img 顶层，尺寸）
- 宠物槽 BtPet1/2/3 30×27 origin(15,13)
- 心愿 BtWish 60×19；想买的显示板 backgrnd3
- 道具 BtItem 60×19；道具显示板 backgrnd4
- 排除道具 BtExceptionShow 71×17 / BtExceptionHide 12×12
- 坐骑 BtTamingShow/Hide 47×17；宠物 BtPetShow/Hide 49×17
- 收藏 BtCollectionShow/Hide 49×17
- 勋章/图鉴 BtBookShow/Hide 48×17
- BtMeso/BtRegister/BtDelete 31×17；BtSearch 38×17
- Marry married/unmarried 20×17；bossPetCrown 18×12；ReqLv 31×10
- 人气箭头：IMG 顶层**无**对应节点 → 用 ColorBox+Text 程序化绘制（参照图人气行右侧 ~14×14 灰箭头）

## 4. 当前实现空壳（code-explorer 审计，完整见工具结果）
- `button_pressed` UICharInfo.cpp:343-529：
  - no-op：BtWish(388-394 LOG)、BtException(395-401 LOG)
  - 误实现：BtRide/BtPet(402-411) 传 show_bottom_window 无分支 → 折叠所有子窗
  - 不可达：BtBook(382-387) 按钮不加载不绘制
  - 已接线：BtClose/Family/Party/Trad/Friend/Visit/PopUp/PopDown/FAQ/Regist/ArrayGet/ArrayName
- `show_bottom_window`/`show_right_window` (:776-806)：只认 Personality/Collect/Damage/Item 四 bool，无 Ride/Pet/Wish/Exception 分支，无 toggle
- `update_extras` (:766-774)：只把计数拼成一行文本 "Pet N Wish N Medal#N Book N"，明细全丢
- `CharInfoHandler` PlayerInteractionHandlers.cpp:30-109：宠物/坐骑/心愿/图鉴/勋章明细全 skip，尾部 while(available) skip_byte

## 5. 可复用 UI 与发包
- `UIExceptionItemSearch`(UIMiscShells.cpp:7469)：独立搜索窗，构造读 UICharInfo::get_target_name() 并发 FindPlayerPacket —— 语义是"搜索他人排除道具"，非"展示已排除列表"，BtException 接它需斟酌
- `UIMonsterBook`(可 emplace)、`UIUtilDlgExPet`/`UIUtilDlgExMultiPet`、`UIMedal`、`UIFamilyTree`
- `UIEquipInventory` 只接受本地 Inventory，不能显示他人装备
- 发包：GiveFamePacket(cid,up)、PartyInvitePacket(name)、dispatch_trade_invite(cid)、BuddyAddPacket(name,group)、FamilyAddPacket(name)、WhisperPacket(to,text)、FindPlayerPacket(name)、OpenFamilyPacket()

## 6. 未决/边界
- 他人装备列表：服务端不下发，协议边界，不臆造
- 排除道具列表展示：PET_EXCEPTION_LIST(173) 数据当前被 PetExceptionListHandler 丢弃，可收集但需按 petIdx 组织；作为子窗数据源
- 人气箭头 UI：IMG 无节点，ColorBox+Text 程序化绘制，位置按参照图人气行右端
- 快捷键 T/P/B 绕过"看自己"禁用：send_key 分发前检查按钮 state
