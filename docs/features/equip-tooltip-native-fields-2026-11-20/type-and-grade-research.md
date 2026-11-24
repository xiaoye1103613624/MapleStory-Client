# 装备类型映射与品级边框调查（2026-11-20）

> 个人学习/技术研究，单机离线。三张小册子截图与图鉴站数据只作类型名称参考，非本端验收证据（USER.md）。

## Q1：装备 tooltip 显示的类型有哪些？三端 id→类型表？缺的能否补？

### 1.1 本端tooltip显示的类型（行号级）
- 类型字符串由 `Data/EquipData.cpp` 硬编码号段表产生：
  - 非武器 15 类：`:151-168` `HAT/FACE ACCESSORY/EYE ACCESSORY/EARRINGS/TOP/OVERALL/BOTTOM/SHOES/GLOVES/SHIELD/CAPE/RING/PENDANT/BELT/MEDAL`（`index=id/10000-100`，`:149`）。
  - 武器 19 类：`:194-213` `ONE-HANDED SWORD/AXE/MACE, DAGGER, WAND, STAFF, TWO-HANDED SWORD/AXE/MACE, SPEAR, POLEARM, BOW, CROSSBOW, CLAW, KNUCKLE, GUN`（`WEAPON_OFFSET=30`）。
  - 高版本扩展 6 类：`:121-144` `SHOULDER(115)/BADGE(118)/EMBLEM(119)/TOTEM(120)/ANDROID(166)/HEART(167)`。
  - 兜底 `:216-219` `CASH`。
- 这些 type 字符串走 `Locale::enum_text("equip.type.", type)` 本地化，中文表 `locale/zh-CN.ini:48-79` 共 **32 条**（15非武器 + 17武器（其中3个空串未显示）+ CASH + 扩展6类部分缺）。
- **本端无独立 id→类型名数据表**，纯靠号段 + WZ `islot` 推导（`:84-108` islot 优先，号段兜底）。

### 1.2 三端 id→类型对应关系
- **s9 服务端**：`ItemInformationProvider.java:659-666 getWeaponType` 有 20 项 `WeaponType[]`（`cat=(id/10000)%100`，30-49），只是**武器**大类；槽位由 `getEquipmentSlot` prefix override（S10 `:597-610`）。**不是完整 id→类型名对应表**。
- **095/192 端**：明文 `wz/Item.wz/*.img.xml` 里是装备属性（islot 等），名称在 `String.wz/Eqp.img.xml`。**两端都没有"id→类型名"的独立映射表**——类型名是 WZ 结构（Character/ 子目录 + islot 字段）隐式表达的，不是一张表。
- 结论：**083/095/192 三端都没有现成的 id→类型名对应表**；本端用号段表已是等价实现。

### 1.3 图鉴站补缺可行性
- 用户三张图 = 小册子（mxd.dvg.cn）分类树：防具/饰品（帽子5019/套服4053/鞋子2579/披风1884/上装911/手套911/下装759/盾牌141）、武器（双手武器3179：双手剑289/矛287/拳套282/弓箭278/弩266/枪238/长枪237/拳甲229/双手钝器208/双手斧199/手炮162/双弩枪161/武士刀133/机甲手枪60/拳封53/远古弓50/环刃47…；单手武器2974：单手剑368/短剑291/单手钝器282/长杖272/杖249/单手斧224/手杖159/能量剑159/双头杖138/灵魂手铳132/亡命剑128/记忆长杖123/阴阳扇81/ESP限制器65/魔力手套53/调谐器52/锁链52/呼吸手铳50/扇子50/长剑46…；辅助武器615：刀121/军团盾35/魔道书30/小太刀16/卡片15/灵魂盾牌15/魔法箭矢15/麦林弹13/叶片12/宝珠12/控制器12/火药桶12/灵魂戒指12/诺巴精髓12/护理宝珠11/武器传送装置11/吊坠10/如意宝珠10/弓箭护指10/念珠10/扇坠10/手腕护带10/手链10/护符10/棋子10/武器腰带10/灵符10/瞄准器10/短剑剑鞘10/箭羽10/索魂器10/装药10/遗物10/铁链10/饰品10…）。
- 这些是**现代/高版本类型细分**（能量剑/ESP限制器/呼吸手铳/灵魂手铳/魔道书/小太刀/亡命剑/机甲手枪等），v083 原生号段表**没有**对应分类；对应 id 多为 13xxxx/12xxxx 高版本号段。
- **补法（若用户要）**：不是补 locale（locale 只翻译已有 type 字符串），而是扩 `EquipData.cpp` 号段表 + 服务端 `getEquipmentSlot` 的 prefix override，把高版本号段映射到这些新 type 名；再在 `locale/*.ini` 加 `equip.type.*` 中文名。**图鉴站只提供"类型名清单"，不提供 id→类型映射**——id 对应关系仍要从各版本 WZ 的 islot/号段实证（orange-wz 查 live Character/*.img）。
- 当前状态：**未补**（用户本轮只问"有没有/能不能"，未要求实现）。

## Q2：副标题是否显示？品级边框是否用上？

### 2.1 副标题代码状态
- 代码已实现：`EquipTooltip.cpp:1093-1097`（构建 potrank→equip.rank.*）、`:680-684`（对比窗侧）、绘制侧 rank_line 插在名称区（品级副标题行）。
- 文案 `equip.rank.hidden/rare/epic/unique/legendary` 已写入 live locale（具名 exe strings 全含 rank key 2 命中）。
- 触发条件：`Equip.cpp:25` potrank 由 `p1/p2/p3` 是否非0推导 → 有潜能即 POT_RARE → 显示"B级（稀有物品）"；SS级需更高 potrank（本端 potrank 只由 p1/p2/p3 推导，**无 SS 级区分**）。
- **本轮未真机验收**：live 目录当前无 `MapleStory.exe` 主 exe（5 个具名 exe 都含 rank key），用户截图来源端/具体 exe 未确认；按 USER.md 不能拿不确定来源截图当本端验收凭据。

### 2.2 品级边框资源（三端有、本端无）
- 095/192 明文 `wz/UI.wz/UIWindow.img.xml` 有专门节点：
  - `Rank`：s/a/b/c/d… 各 canvas（62x82/59x73/55x74…，origin -23,-13 等）= 品级徽章图；
  - `UniqueBox`：normal 69x62 等 = 稀有物品专属框；
  - `Grade`、`Border`：品级色块/边框条。
- 本端 v83 `Data/UI/UIWindow.img`（orange-wz 128B 实测）：`search_node "Rank"` 只命中 `ShortCut/BtRanking`（排行榜按钮）、`"Grade"` 只命中 `UserList/Guild/GuildGrade`（公会等级）、`"UniqueBox"` **0 命中**——**没有装备品级边框资源**。
- 本端代码 `EquipTooltip.cpp/.h` grep `grade|border|rank.*frame`：只有 rank **文字**行（equip.rank.*），**没有任何品级边框绘制**。
- 结论：**副标题（文字）代码已实现；品级边框未实现**——因为本端 WZ 没有边框资源，095/192 有但跨版本 key 不同、需经 orange-wz batch_convert_key 迁移（AGENTS.md §3.1 硬门禁），属于独立移植任务。

## 3. 如实声明
- 三张小册子截图为公开图鉴站分类树，只作类型名参考，非本端证据。
- 本轮为源码 + WZ 节点 + 图鉴站的只读取证，**未改任何代码、未真机验收**。
- 095 客户端 VMP 壳不可反编译；类型/边框结论来自明文服务端 XML + WZ 节点，非客户端逐像素。
