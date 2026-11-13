# r03 — 第五功能面「宠物·召唤兽·龙」IMG / UI 排版 / 样式 / 交互层证据

> 来源：第三路只读 subagent `92e3f47c-f46b-4b89-a298-6096bea7e393` 完整清单（2026-11-08 22:09），主 agent 加 §0 复核块。
> 基准 083/v83（`Configuration.h:299 VERSION="0.83"`）；全程只读、未改文件；未引 095/v95 线证据作依据。

## §0 主 agent 复核块（8 条）

1. **与 r01 硬互证**：Angel `CUIPetEquip::OnButtonClicked` 逐字 `if(a2==2000) TogglePetEquip(dword_BED64C); else if(a2>0x7D0 && a2<=0x7D3){ *(this+360)=a2-2001; InvalidateRect(...); }` → **2001/2002/2003 = 三宠物页签 id，页索引存 this+360**；第三路独立从 `evidence/angel-petsummon-decompile-full.json:39` 读到同一结论。当前端 `UIEquipInventory.cpp:181` `pet_tabs[3]={"BtPet1","BtPet2","BtPet3"}` / `:184` `(right+7+34*i, bottom-30)` 严格对应。
2. **「校准 ≠ 改回原生」再证**：Angel 用 `CUIPetEquip`（2 方法）子面板 + `CUIEquip::TogglePetEquip/ToggleDragonEquip`（corewindow 面 `angel-corewindow-decompile-full.json:139-141` 的 `case 3001u→ToggleDragonEquip` / `case 3002u→TogglePetEquip`）由装备窗 Toggle 拉起；当前端把三页签直接做进 `UIEquipInventory`（`set_pet_panel` / `BT_PET_TAB0..2` / `pet_panel_open`），**无独立 UIPet 窗**（glob 全库无 `UIPet.h`，与任务预判一致）。
3. **UIWindow.img 顶层无独立宠物/召唤/龙窗**：112 顶层节点里宠物相关仅 `:4 Equip` / `:12 UtilDlgEx_Pet` / `:69 UtilDlgEx_MultiPetEquip` 三个；**无 Pet/Summon/Dragon/Mount/Taming 顶层窗** → 与 r01「本面 SP_ 主体是 Effect/Item/Sound 而非 UIWindow.img（185 个 SP_ 仅 6 个 UI_UIWINDOWIMG）」结构互证。
4. **真缺口（UI 层）**：`Equip/pet` 13 个槽名烤进贴图、无 `Slots` 子节点 → 当前端 `UIEquipInventory.cpp:622` 逐字「预留：BtPet1/2/3 三页宠物装备槽的穿戴/卸下（Equip/pet 下无 Slots 子节点，槽位数据待协议批次）」；`w7-equip/coords.md:34` 列 13 槽名（自动HP/宠物HP/自动MP/宠物MP/自动拣物/金币拾取/物品过滤/范围提升/项圈/宠物装备/速度提升/宠物戒指×2）。Angel `CUIPetEquip::OnMouseMove` 用 `CharacterData::GetItem(..., a4: -v5)` 与 `a4: -100-v6` 两处负槽位取物 → **原生宠物装备槽 = 装备栏 a3=1 的负槽位**，当前端未接线。
5. **排版待定项（非错误）**：`BtPet1/2/3` 四态 origin **实测 (15,13)**（`r27-b3-equip/README.md:18` 更正 `coords.md:37` 旧值 (15,0)）；`UIEquipInventory.cpp:176-180` 逐字记「按 (15,0) 推出来的现有 y 可能整体高 13px，等一张展开宠物面板的原生帧再定（列入用户取证）」；4 格改色预测命中率 71.27/77.19/34.67/71.08%（`r27-b3-equip/README.md:313-315`）。→ **只记不改，须原生展开帧**，禁止盲调。
6. **locale / Configuration 覆盖稀薄**：locale 宠物/召唤/龙相关 **全库仅 4 个 stub 键 ×2 语言**（`charinfo.pet_wish` + `utildlgexmultipet.{multipet_softkey_name,no_517_tag,rename}`），zh-CN 值全英文未译、与 en-US 完全相同；真文案硬编码在 `UIMiscShells.cpp:6823/6841/6874/6898/6910/6941/6964`（"Pet name"/"pet chat"/"exclude ids"/"slot,itemId"/"MultiPet name"）。Configuration 仅 `ui.autopot.hp_item` / `ui.autopot.mp_item`（`Configuration.h:1055-1064`，默认 0），**无 `pet_auto_pot` 字面量**（任务里名称需纠正），无宠物名/骑宠/排除物品/召唤落盘设置 → 全走运行时封包。
7. **095 污染点（主 agent grep 补充）**：`Gameplay/MapleMap/MapPortals.h:40` 逐字「传送门触发范围（对齐 v95：CPortal::CheckKey 的按键通道 vs ProcessPortal 的碰触通道）」——MEMORY.md 违规面新增 1 处，处置 = 只换注释依据引 Angel 083，不改行为。`docs/features/native-map-object-re/evidence/beidou_v177_decrypted_strings.tsv:2341/4680/4681` 是 V177 明文串，非 v83 基准，仅记存在性不作依据。
8. **IMG 门禁阻塞（六面一致）**：`mcp_connector_status` 仅 `json-photoshop`；`mcp_connector_catalog(keyword=orange)`=0；`C:\Users\11036\.cursor\mcp.json:3-6` 仅 Cursor 侧声明 orange-wz(127.0.0.1:10012) 未注册 Host；`map-object-audit/evidence/codebuddy-transcript-947f8ba0.md:37` 已知 `Index 16 out of bounds for length 4` + `list_children` 空 → 资源层证据以仓库 `.nx` 节点引用 + 截图为准。

## §1 节点引用（.nx 字面量 + UI 资源定义，逐字 + 行号）

### A. 装备窗宠物区（UIEquipInventory.cpp，主关注）
- `IO/UITypes/UIEquipInventory.cpp:92` — `nl::node Equip = UiAssetResolve::window4("Equip");`（Equip 根节点）
- `:162` — `pet_backgrnd = Texture(Equip["pet"]);` 宠物面板底图节点 `Equip/pet`[177x181]
- `:160-162` — 注释「宠物装备区：以窗体右下角为锚，坐标逐像素模板匹配 正确客户端截图/角色装备.png 量得」
- `:169-171` — 注释 pet[177x181] 压住窗口右边 3px；`pet_panel_pos = (right-3, bottom-pet_backgrnd.height())`
- `:173-174` — `Equip["BtPetEquipShow"]` @ `(right-73, bottom-24)`
- `:181` — `static const char* const pet_tabs[3] = { "BtPet1", "BtPet2", "BtPet3" };`（三页签节点名逐字）
- `:184` — `Equip[pet_tabs[i]]` @ `(right+7+34*i, bottom-30)`，注释间距 34
- `:187-188` — `Equip["BtPetEquipHide"]` @ `(right+152, bottom-22)`
- `:176` — 注释「实测 BtPet1/2/3 四态 origin 都是 (15,13)（不是旧注释 (15,0)）…等一张展开宠物面板的原生帧再定」
- `:60-86` — `try_pet_tab_btn(...)`：`:70` 未选=`src["disabled"]["0"]`、`:71` 选中=`src["normal"]["0"]`、`:82` `TwoSpriteButton(unselected, selected,...)`；注释 `:68-69` 未选=灰底灰数字、当前页=彩底红数字
- `:131-132` — `iconpositions[TAMEDMOB]=(5,230); SADDLE=(38,230)`（行7 骑兽/鞍子）
- `:213-215` — 注释「Equip/pet 槽名已烤进贴图，不再用 Text 重画」+ draw pet_backgrnd
- `:219-221` — 注释不可用槽=同张 backgrnd 逐像素改色
- `:592` — `pet_panel_open = open && pet_backgrnd.is_valid();`；`:606` `dimension = pet_panel_open ? expanded_dimension : base_dimension`
- `:620` — `b->set_state(i==pet_tab ? PRESSED : NORMAL)`（三页只切高亮）
- `:622` — 注释「预留：BtPet1/2/3 三页宠物装备槽的穿戴/卸下（Equip/pet 下无 Slots 子节点，槽位数据待协议批次）」
- `docs/features/ui-v83-window-restore/notes/asset-node-inventory.md:43-44` — `UIWindow.img/Equip` 10 个直接子节点逐字：`backgrnd[175x304] / FullBackgrnd[208x291] / pet[177x181] / BtPet1[30x27] / BtPet2[30x27] / BtPet3[30x27] / BtPetEquipShow[67x18] / BtPetEquipHide[17x16] / BtCashshop[32x11]{4态} / BtDetail[46x19]`
- `:46-52` — 槽位灰底原生=逐像素调色 `R'=snap17(0.75R+68.25)`、`G'=B'=snap17(0.75V)`；origin：backgrnd/FullBackgrnd/pet 均(0,0)、BtPet1/2/3 四态均(15,13)；槽位 x∈{5,38,71,104,137} y=32+33k
- `docs/features/ui-v83-window-restore/evidence/w7-equip/coords.md:23` — 参考截图 `C:\Users\11036\Desktop\正确客户端截图\角色装备.png` = 348×305
- `:34` — Equip/pet 177×181、13 槽名烤进贴图（自动HP/宠物HP/自动MP/宠物MP/自动拣物/金币拾取/物品过滤/范围提升/项圈/宠物装备/速度提升/宠物戒指×2）、无 Slots 子节点
- `:84-101` — §7 四态实测：disabled=灰底灰数字(mae 0.0 为截图态)、normal=浅蓝底红字、pressed=深灰、mouseOver=蓝描边
- `docs/features/ui-v83-window-restore/evidence/r27-b3-equip/README.md:18` — `BtPet1/2/3` 四态 30x27 **origin=(15,13)**（更正 coords.md:37 旧值 (15,0)）
- `:253` — `String.nx/Eqp.img/Eqp` 16 类含 `Taming{1902002} / PetEquip{1802002} / Dragon{1942002}`（原生独立装备类别）
- `:270` — 本仓没有 Pet/PetEquip 类、`EquipSlot::Id` 无 PET_* 槽位
- `:313-315` — 宠物面板 4 格改色预测命中率 71.27/77.19/34.67/71.08%，待原生展开帧
- `docs/features/ui-v83-window-restore/evidence/r27-oracle/Equip.json:7` — Equip 根节点实有子节点清单 + 网格行距恒 33 共 7 行 + 粉色宠物格 bbox (72,234) 31x31 与 (6,234) 64x31

### B. 宠物对话框（UIMiscShells / UINpcTalk）
- `IO/UITypes/UIMiscShells.cpp:156-169` — `load_window_bg(name,Texture&,Point&)`：`:158` `UiAssetResolve::window(name)`、`:161` `root["backgrnd"]?…:root["backgrd"]`、`:162-164` `root["t"]/["Cover"]/["0"]["Cover"]/["backgrnd"]`、`:166` `out=Texture(bg)`
- `:1762` — `load_window_bg("UtilDlgEx_Pet", ...)`（UIAskPet 底图）
- `:1764` — 回退 `UtilDlgEx_Avatar`；`:1777` `UiAssetResolve::window("UtilDlgEx_Avatar")`；`:1789-1793` `avatar["BtPrev"]/["BtNext"]/["BtExit"]` + `nl::nx::UI["Basic.img"]["BtOK2"]/["BtCancel2"]`
- `:1749` — 注释「dlgType=6：UtilDlgEx_Pet 419×297」；`:1758` 注释「IDA sub_9A3CAE/9A3CED：底图 UtilDlgEx_Pet/backgrnd 419×296」；`:1759` position=(190,150)
- `:1901` — `nl::node src = nl::nx::Item["Pet"][strid + ".img"];`（UIAskPet::load_pet_stand）；`:1902` stand0/stand；`:1908` ItemData icon
- `:6805` — `load_window_bg("UtilDlgEx_Pet", ...)`（UIUtilDlgExPet）；`:6803-6805` position=(260,120) dimension=(320,240) dragarea=(320,20)；`:6825` draw 只画 backgrnd
- `:6922` — `load_window_bg("UtilDlgEx_MultiPetEquip", ...)`（UIUtilDlgExMultiPet）；`:6921` position=(240,100) dimension=(360,280) dragarea=(360,20)
- `:6826-6915` — send_key 软键标签硬编码：`"Pet name"`(改名/517*名牌)、`"cmd 0-9"`(PET_COMMAND)、`"pet chat"`(PET_CHAT)、`"exclude ids"`(PET_EXCLUDE_ITEMS)、`"slot,itemId"`(PET_AUTO_POT)、M→UIUtilDlgExMultiPet、X→关闭
- `:6941/:6964` — 硬编码 `"MultiPet name"`；`:6919/:6935/:6939` 用 locale 键 utildlgexmultipet.*
- `IO/UITypes/UIMiscShells.h:271-272` — `WND_W=419; WND_H=297`；`:276-278` `PET_LOOK_X=209; PET_LOOK_Y=218; PET_ICON_X=226`；`:281-288` `BTN_PREV_X/Y=15/191、BTN_NEXT_X/Y=341/191、BTN_OK_X=314、BTN_CANCEL_X=363、BTN_EXIT_X=9、BTN_BOTTOM_Y=272`
- `IO/UITypes/UINpcTalk.cpp:1163` — `node = "UtilDlgEx_Pet";`（speaker==2 宠物说话人皮肤）；`:1165` `UiAssetResolve::window(node)`；`:1171-1174` `root["t"]/["c"]/["s"]/["bar"]`
- `IO/UITypes/UINpcTalk.h:204-212` — AVATAR_BTN_* 同族坐标常量
- `docs/features/ui-v83-window-restore/evidence/r15-newrefs/_p4_nxframes.txt:152` — `UIWindow.img/UtilDlgEx_MultiPetEquip/backgrnd bitmap 419x219`；`_p17_allframes.txt:72` 同尺寸 opaque 413x213

### C. 角色信息 / 技能书 骑宠按钮
- `IO/UITypes/UICharInfo.cpp:56` — `UserInfo = UiAssetResolve::window("UserInfo")`；`:90` `character["BtPet"]`、`:93` `character["BtRide"]`
- `:104-108` — 注释「坐骑外观未暴露：Ride 保持禁用；Pet 在 load_target 后按 has_pet 切换」+ 两钮 `set_state(DISABLED)`
- `:369-370` — `case Buttons::BtRide:` / `case Buttons::BtPet:` → `show_bottom_window(buttonid)`
- `:724` — `Locale::format_all("charinfo.pet_wish", {pet_count, wish_count})`
- `IO/UITypes/UISkillBook.cpp:172` — `try_maple_btn(buttons, Buttons::BT_RIDE, main["BtRide"]);`（骑宠技能按钮节点）
- `docs/features/ui-v83-window-restore/notes/asset-node-inventory.md:132-136` — `UIWindow.img/UserInfo`：`BtPetShow 49x17 / BtPetHide 48x17`、`BtTamingShow/Hide 各 47x17`、`BtPet1/2/3 各 30x27`、`bossPetCrown[18x12 origin(9,6)]`
- `:28` — `bossPetIcon[34x38]`（UIWindow.img/Item 下）

### D. 精灵渲染 .nx 节点（Pet/Summon/Dragon）
- `Character/Look/PetLook.cpp:35` — `namelabel = Text(A13M, CENTER, WHITE, NAMETAG, name)`
- `:38` — `nl::node src = nl::nx::Item["Pet"][strid + ".img"];`；`:40-46` 段名 `move/stand0/jump/alert/prone/fly/hang`；`:48` `nl::nx::Effect["PetEff.img"][strid]`；`:50` `animations[WARP] = effsrc["warp"]`
- `Gameplay/MapleMap/MapSummon.cpp:34-35` — `nl::nx::Skill[skill_namespace(skill_id)+".img"]["skill"][strid]["summon"]`；`:38-39` 回退 `["summoned"]`；`:41-52` 只收容器段、跳过 info；`:59/:61-64` `set_state("summoned")`；`:28` 注释段名 `stand/hit/die/summoned`
- `Gameplay/MapleMap/MapDragon.cpp:25` — `nl::node src = nl::nx::Mob["8840000.img"];`；`:27` 注释段名 stand/move/hit(非hit1)/die1；`:39` `if(name=="info"||name=="summon") continue;`；`:52` `flip=(stance%2)==0`；`:20-23` 注释 Evan 1001..1005 只部署 8840000 一只
- `Gameplay/MapleMap/FieldMarker.cpp:230/:232` — `src["summon"]["0"]` / `src["summoned"]["0"]`
- `Graphics/GraphicsGL.cpp:1634-1635` — `else if (root=="TamingMob") base = nl::nx::TamingMob;`；`Util/WzRuntime.cpp:780` `nx::TamingMob = add_file("TamingMob.wz");`、`:787` 根名表含 "TamingMob"；`includes/NoLifeNx/nlnx/nx.cpp:143` `TamingMob = Base["TamingMob"];`；`Util/Resource/RootTable.cpp:66` `&nl::nx::TamingMob`
- `IO/Components/NameTag.cpp:126` — `nl::node root = nl::nx::UI["NameTag.img"];`；`:133-134` `case Group::PET: root = root["pet"];`（UI/NameTag.img/pet/<id>）
- `_tools/summon_probe/main.cpp:267` — `f2->root()["311.img"]["skill"]["3111002"]["summon"]`
- 全库结论：grep `nl::nx::(UI|Character|Item|Skill|Etc|Map|Mob|Effect|TamingMob)` 含 Pet/Summon/Dragon 的 .nx 字面量仅 4 处（PetLook.cpp:38/48、MapDragon.cpp:25、UIMiscShells.cpp:1901）；`Character/Pet*` `Item/Pet*` `Skill/*Summon*` `Etc/*` 形式无额外命中

### E. UIWindow.img 顶层节点表（文档）
- `docs/features/v83-hud-quest/evidence/UIWindow.img.top-level.txt:4` `Equip`、`:12` `UtilDlgEx_Pet`、`:69` `UtilDlgEx_MultiPetEquip`（全文 112 行）
- 全表无独立 Pet/Summon/Dragon/骑宠 顶层节点，宠物仅这两个 UtilDlgEx_* 对话框 + Equip 根节点
- ⚠ 更正：任务所写 `docs/features/map-object-audit/ 的 UIWindow.img.top-level.txt` 实际不存在（glob 确认该目录仅 codebuddy-baseline.txt / codebuddy-transcript-*.md）；真实文件在 `docs/features/v83-hud-quest/evidence/` 下
- `docs/features/ui-v83-window-restore/notes/raw-nx-dumps/Equip.txt:58/:67` — 旧 dump 把 `BtPetEquipHide` / `BtPetEquipShow` 记为 `[null]`
- `docs/features/ui-v83-window-restore/evidence/w4-chatbar/utildlgex-tree.txt:101/:281`、`_whisp_node.txt:105` — `UtilDlgEx_MultiPetEquip` 节点条目

### F. 解析层（判定节点根归属）
- `Util/UiAssetResolve.cpp:42-55` — `window(name)`：先 `nl::nx::UI["UIWindow2.img"][name]` 再 `nl::nx::UI["UIWindow.img"][name]`
- `:57-75` — `window4(name)`：`UIWindow4.img` → `UIWindow2.img` → `UIWindow.img`；`:133-145` `usable(n)=n.size()>0 或 bitmap`
- `:115-131` — `close_btn()`：Basic.img BtClose→BtClose2→BtClose3
- ⇒ Equip/pet 走 window4（UIWindow4/2/img），UtilDlgEx_Pet/MultiPetEquip 走 window（UIWindow2/img）[推断]
- `docs/features/map-object-audit/evidence/codebuddy-transcript-947f8ba0.md:49/:54/:56` — 四窗脚本驱动动态拼 `UI/UIWindow.img/<name>`（故 .c 里看不到路径字面量）；`:103` `UiAssetResolve::window(name)` 返回 `nl::nx::UI["UIWindow.img"][name]`
- `docs/features/map-object-audit/README.md:18/:29/:40` — Summon 真状态机对象；Char/Pet/Summon 真实渲染（宠物挂 Char.cpp:66-119）；Dragon→Mob/8840000.img 真对象

### G. Angel(13339) 083/v83 带符号 IDB 校准（UI 交互层）
- `docs/features/ida-v83-angel-instance/README.md:198` — `| 宠物 | CPet / CPetTemplate / CVecCtrlPet / CUIPetEquip | 28/2/6/2 |`
- `docs/features/feature-gap-inventory-2026-11/README.md:627` — `CPet / CVecCtrlPet / CUIPetEquip / CSummoned / CDragon` EA `0x703314../0x9c406b../0x800f7b../0x7a3366../0x4feb23..` 函数 28/6/2/17/10
- `docs/features/corewindow-angel-calibration-2026-11/evidence/angel-corewindow-decompile-full.json:139` — `CUIEquip::OnButtonClicked`：`case 3001u: ToggleDragonEquip(...)`、`case 3002u: TogglePetEquip(this)`
- `:141` — `CUIEquip::TogglePetEquip` 反编译（Alloc(1456)、`v5=...+123`、`sub_7FE299(...)`、`*(this+361)=1`）
- `docs/features/corewindow-angel-calibration-2026-11/notes/r03-corewindow-img-style-evidence.md:182-183` — Angel `ToggleDragonEquip/TogglePetEquip` 正好解释「175 与 208 两套宽」「BtPet1/2/3 三页」；当前端 UIEquipInventory.cpp:567,622 记三页显式预留
- `evidence/angel-petsummon-decompile-full.json:39` — `CUIPetEquip::OnButtonClicked`（2001/2002/2003 三页签 id）
- `:38` — `CUIPetEquip::OnMouseMove` 反编译（`CharacterData::GetItem(...,a4:-v5)` 与 `a4:-100-v6` 两处宠物装备槽取物、`sub_8011FA` 命中格换算、`SetCursorState` 手势）
- `evidence/angel-petsummon-facts.md:734-748` — `### CUIPetEquip（2 函数）`：OnButtonClicked 345B / OnMouseMove 4826B
- `evidence/angel-petsummon-ea.json:3312-3323` — `CUIPetEquip: 0x800f7b OnMouseMove / 0x8019f1 OnButtonClicked`
- ⇒ 与主 agent r01 结论一致：本面 SP_ 主体是 Effect/Item/Sound 而非 UIWindow.img；UI 排版层证据集中在 Equip 宠物区 + 两个 UtilDlgEx_* 对话框[推断]

### H. 非 083/v83 基准旁证（仅记存在性，不作依据）
- `docs/features/native-map-object-re/evidence/beidou_v177_decrypted_strings.tsv:2341` `Character/PetEquip/%08d.imgL`、`:4680` `UI/UIWindow.img/Equip/BtPetEquipShow`、`:4681` `.../BtPetEquipHide9` — V177 客户端明文串，非 v83 Angel 基准，不作依据

## §2 IO/Components 通用组件定义 + 关键常量

- `IO/Components/Icon.h:105-106` — `static constexpr int16_t cell_width = 32; cell_height = 32;`（图标 32×32）；`:108-114` set_cell/rect/contains/draw_at
- `IO/Components/Icon.cpp:35-40` — 注释「相对格顶整体下移一格（32px = Icon::cell_height）」+ `texture.shift(Point<int16_t>(0,32))`
- `IO/Components/MapleButton.h:28` — `// A standard MapleStory button with 4 states and a texture for each state`；`:44-45` `Texture textures[NUM_STATES] / Animation animations[NUM_STATES]`
- `IO/Components/MapleButton.cpp:55-71` — 四态节点逐字：`src["normal"]`、`:64` `src["pressed"]["0"]`、`:65` `src["mouseOver"]["0"]`、`:66` `src["disabled"]["0"]`；`:59-62` normal.size()>1 走 Animation
- `IO/Components/Button.h:28-36` — `enum State { NORMAL, DISABLED, MOUSEOVER, PRESSED, IDENTITY, NUM_STATES }`
- `IO/Components/NameTag.h:29-38` — 注释 UI/NameTag.img 三切片 w(origin.x==width)/e(横向重复)/c + clr ARGB；`:37` `Same layout under NameTag.img/medal/<id> and NameTag.img/pet/<id>`；`:41-46` `enum class Group { NONE, MEDAL, PET }`；`:49-50` `MIN_ID=3; MAX_ID=254`
- `IO/Components/NameTag.cpp:205-207` — `frame.left=Texture(src["w"]); frame.mid=Texture(src["e"]); frame.right=Texture(src["c"]);`；`:219` `read_argb(src["clr"])`；`:139/:148` id 约束 + `root[std::to_string(id)]`
- `IO/Components/Charset.h:29-34` — `enum Alignment { LEFT, CENTER, RIGHT }`；`Charset.cpp:30-43` charset_char() Lbracket→'['/Rbracket→']'/slash→'/'/percent→'%'；`:48-63` 构造只收 bitmap 节点
- `IO/Components/Gauge.h:27-34` — `enum Type { DEFAULT, CASHSHOP, WORLDSELECT, CLIP }`（CLIP 注释「自纹理左侧裁出 percentage*maximum 宽，用于 v83 StatusBar HP/MP」）；`:36` 默认 `maximum(1), percentage(0), target(0), step(0), clip_left(0)`
- `IO/Components/Slider.h:29` — 构造签名 `Slider(int32_t type, Range<int16_t> vertical, int16_t x, int16_t unitrows, int16_t rowmax, std::function<void(bool)> onmoved)`；`:44-75` Type 枚举；`:96-102` dbase/dnext/dprev + 三个 TwoSpriteButton
- `IO/Components/Slider.cpp:39/:43/:46` — `nl::nx::UI["Basic.img"]["VScr5"]` / `VScr`；`:75-87` `dsrc["prev"]/["next"]`、`esrc["prev0"]/["prev1"]/["next0"]/["next1"]/["thumb0"]/["thumb1"]`；`:89-94` 注释「VScr5 的 thumb 是 15x25」
- `IO/Components/Tooltip.h:24-42` — 接口类 `class Tooltip`；`enum Parent { NONE, EQUIPINVENTORY, ITEMINVENTORY, SKILLBOOK, SHOP, EVENT, TEXT, KEYCONFIG, WORLDMAP, MINIMAP }`；`:46` `virtual void draw(Point<int16_t>) const = 0`
- `IO/Components/EquipTooltip.cpp:53` — `nl::nx::UI["UIWindow.img"]["ToolTip"]["Equip"]`；`:62` `nl::nx::UI["UIWindow.img"]["IconBase"]["0"]`（注释 32x32 灰底板）；`:46` HEIGHT_BASE=540+15
- `IO/Components/SkillTooltip.cpp:35/:137` — IconBase[0] 节点、width=292；`ItemTooltip.cpp:67-68` — fillwidth=264、fillheight=83+name.height()；`TextTooltip.cpp:41-42` — `if(fillheight<18) fillheight=18`
- 组件文件清单（glob IO/Components/**/*.{h,cpp}）：AreaButton/Button/Charset/IconCover/MapleComboBox/MapleFrame/NpcText/ScrollingNotice/StatefulIcon/Tooltip/TwoSpriteButton/LocalTwoSpriteButton/MapleButton/TextTooltip/ItemTooltip/MapTooltip/NameTag/Gauge/Slider/Icon/SkillTooltip/Textfield/ChatBalloon/EquipTooltip

## §3 已有宠物/骑宠/召唤截图 / layout JSON / Photoshop 产物

（PNG 尺寸由 powershell `[System.IO.File]::ReadAllBytes` 读 IHDR 实测）
- `docs/features/beidou-v17-parity/notes/ui_layout_dump/equip_pet.png` — 177x181（Equip/pet 底图）；无配套 layout JSON
- `docs/features/ui-v83-window-restore/evidence/w7-equip/pngs/pet.png` — 177x181；`BtPet1_normal.png` 30x27；`BtPet1_disabled.png` 30x27；`BtPet1_states4_vs_ref.png` 512x260（四态对照）；均无配套 layout JSON
- `docs/features/ui-v83-window-restore/evidence/w5-tooltip/pngs/cbpet0_c.png` — 12x14；`cbpet0_nw.png` — 6x6（宠物 tooltip 小片）；无 layout JSON
- `docs/features/ui-v83-window-restore/evidence/r15-newrefs/_nx_all/UIWindow.img_UtilDlgEx_Pet_backgrnd.png` — 419x296；`UIWindow.img_UtilDlgEx_MultiPetEquip_backgrnd.png` — 419x219；均无 layout JSON
- `docs/features/ui-v83-window-restore/evidence/r27-oracle/tmpl/Equip_pet.png` — 177x181；`Equip_BtPet1_normal.png` — 30x27；`r27-b3-equip/Equip_pet.png` — 177x181（模板匹配用）；均无 layout JSON
- `docs/features/ui-v83-window-restore/evidence/r27-b4-userlist-equip-live/live-a53de1c8-equip-pet.png` — 1296x759（整屏实机帧，宠物面板未展开，r27-b3 README:598-601 记 3 条 FAIL 全在宠物面板）；无 layout JSON
- `**/layout-*.json` 全库无命中；`**/layout*.json` 仅 5 个且均非宠物面：`char-stats-ui-ps/ui/detail/v4/layout_cn.json`、`map-search-ui/resources/layout.json`、`map-search-ui/ui/search_panel_backgrnd/v3_pipeline/layout.json`、`map-info-panel/resources/layout.json`、`UI/MapInfo/layout.json` ⇒ 上述 14 张 pet PNG 全部无配套 layout JSON
- glob `**/*[Ss]ummon*.png` / `**/*[Dd]ragon*.png` / `**/*[Mm]ount*.png` 均 No files found（召唤/龙/坐骑无独立截图）
- `docs/features/_native-samples/` 仅 hotkey/ 三张，无宠物样板
- `evidence/screens/` 空目录
- `C:\Users\11036\Desktop\正确客户端截图\` 共约 78 个文件；grep 宠/骑/召唤/龙/装备/坐骑 命中 4：创建骑士团2.png、创建骑士团页.png、角色装备.png、角色装备栏显示物品信息.png；无任何文件名直接含宠物/骑宠/召唤/龙；无 json/layout 配套。`角色装备.png`=348×305 是 `UIEquipInventory.cpp:160` 量宠物装备区的基准帧
- docs 下全部 .psd 产物无 pet/summon/dragon/mount 命名 ⇒ 无 Photoshop 出图产物
- `ui/*/vN/` 目录：docs/features 下 356 项未见宠物/召唤/龙功能目录
- `docs/features/ui-v83-window-restore/README.md:76`、`CHECKLIST.md:84`、`PLAN.md:98` — 均记宠物区 pet[177x181]+BtPet1/2/3+BtPetEquipShow/Hide 按右下锚算位、展开改 dimension

## §4 locale 文案 key 全集（宠物/召唤/骑宠/龙）

- `locale/zh-CN.ini:349` — `charinfo.pet_wish=Pet {} Wish {}`
- `:610` — `utildlgexmultipet.multipet_softkey_name=MultiPet — SoftKey name`
- `:611` — `utildlgexmultipet.no_517_tag=No 517* tag`
- `:612` — `utildlgexmultipet.rename=Rename: {}`
- `locale/en-US.ini:349` — `charinfo.pet_wish=Pet {} Wish {}`；`:607-609` — 同 610-612 三键
- 全集结论：grep `(?i)pet|summon|dragon|mount|taming|骑|宠|龙|坐骑` 于 locale/ 共 8 命中 = 4 键×2 语言，无第五键；无任何 summon/dragon/mount/骑宠 独立文案键
- stub 判定：上述 4 键在 zh-CN 里值全为英文未译（"Pet {} Wish {}" / "MultiPet — SoftKey name" / "No 517* tag" / "Rename: {}"），与 en-US 完全相同 ⇒ 均为占位/stub 文案[推断]
- 补充：真正面向玩家的宠物标签多硬编码于代码而非 locale：`UIMiscShells.cpp:6823/:6841` "Pet name"、`:6874` "pet chat"、`:6898` "exclude ids"、`:6910` "slot,itemId"、`:6941/:6964` "MultiPet name"、`:6819` "UIUtilDlgExPet: no 517* name tag"

## §5 Configuration 设置项（宠物/召唤/骑宠）

- `Configuration.h:1055` — 注释「宠物自动药：s9 AUTO_HP_POT(336)/AUTO_MP_POT(337) 下发的 itemId」
- `:1056-1059` — `struct AutoHpPotItem : Configuration::IntEntry { AutoHpPotItem() : IntEntry("ui.autopot.hp_item","0") {} };` 默认 "0"
- `:1061-1064` — `struct AutoMpPotItem : ... IntEntry("ui.autopot.mp_item","0")` 默认 "0"
- `Configuration.cpp:136-137` — 分组「自动喝药」：`ui.autopot.hp_item`=HP 自动喝药绑定道具 ID、`ui.autopot.mp_item`=MP
- `:249-250` — `settings.emplace<AutoHpPotItem>(); settings.emplace<AutoMpPotItem>();`
- `:550` — 注释「ui.autopot.* 等 39 项曾因三级键解析 bug（反着 prepend 'a.b.c'→'c.a.b'）在 Settings 改了不生效、退回代码默认值」
- 键名差异：任务所称 `pet_auto_pot` 在本仓无此字面量；实际落盘键为 `ui.autopot.hp_item` / `ui.autopot.mp_item`（宠物自动药间接相关），由 AUTO_HP_POT/MP_POT 封包（`Net/Handlers/PlayerHandlers.cpp:387-393/:405-411`）下发 itemId 写入
- 骑宠 / 宠物名 / 宠物排除物品：`Configuration.h` 全文 1088 行 grep `(?i)pet|summon|dragon|mount|taming` 仅 `:1055` 一条注释；`Configuration.cpp` 无独立项 ⇒ **无独立落盘设置项**，这些走运行时封包（`UIMiscShells.cpp` 软键改名/排除列表、`MapObjectHandlers.cpp:1233` PetNameChange / `:1776` PetExceptionList；协议 opcode PET_NAMECHANGE(172)/PET_EXCEPTION_LIST(173) 见 `docs/features/map-object-audit/evidence/codebuddy-baseline.txt:95-118`）[观察+推断]
- `UIKeyConfig.cpp:942-943/:959` — 键位设置 H/M 软键 itemId 读写 `Setting<AutoHpPotItem|AutoMpPotItem>`

## §6 orange-wz(IMG) MCP 门禁结论

- `mcp_connector_status` — 本会话仅 1 条连接 `json-photoshop`(photoshop) json/none/stdio 状态未知 host-tools-pending；**无 orange-wz 已连接**
- `mcp_connector_catalog(keyword=orange)` — 共 0 个已上架连接器
- `C:\Users\11036\.cursor\mcp.json` — 文件存在：`:3-6` `orange-wz`(type=http, url=http://127.0.0.1:10012/mcp)、`:7-10` `ida-pro-mcp`(127.0.0.1:13337/mcp)、`:11-18` `photoshop`(npx)。这是 Cursor 侧配置，DSH Host 未注册 orange-wz 连接
- `docs/features/map-object-audit/evidence/codebuddy-transcript-947f8ba0.md:37` — 历史记录 orange-wz 对 E:\MXD 私服改版 .img 解析 bug「Index 16 out of bounds for length 4」、内部 list_children 空
- **门禁结论：IMG MCP 未接入，资源层证据以仓库内 .nx 节点引用 + 截图为准**[推断]

## §7 宠物召唤龙 UI 资源证据覆盖度小结

【有节点证据（.nx 字面量 + 尺寸/origin/四态实测）】
- Equip 宠物面板：`Equip/pet[177x181]`、`BtPet1/2/3[30x27 四态 origin(15,13)]`、`BtPetEquipShow[67x18]`/`BtPetEquipHide[17x16]`、`FullBackgrnd[208x291]`（`asset-node-inventory.md:43-52` + `UIEquipInventory.cpp:162-188` + 14 张实测 PNG）
- 对话框底图：`UtilDlgEx_Pet/backgrnd[419x296]`、`UtilDlgEx_MultiPetEquip/backgrnd[419x219]`（v83-hud-quest 节点表 + `_p4_nxframes.txt` + 两张 419 宽 PNG）
- UserInfo 骑宠/宠物按钮：BtPetShow/Hide、BtTamingShow/Hide、bossPetCrown/bossPetIcon（`asset-node-inventory.md:28,132-136`）
- 宠物精灵：Item/Pet/<id>.img 段 move/stand0/jump/alert/prone/fly/hang + Effect/PetEff.img/warp；宠物名牌 UI/NameTag.img/pet/<id>
- 召唤兽：Skill/<ns>.img/skill/<id>/summon{stand,hit,die,summoned}；龙：Mob/8840000.img
- 交互逻辑：Angel CUIPetEquip::OnButtonClicked 2001/2002/2003 三页 id、CUIEquip::TogglePetEquip/ToggleDragonEquip（v83 合规基准）

【只有代码坐标 / 无独立节点】
- 宠物装备 13 个槽名（烤进 Equip/pet 贴图，无 Slots 子节点，`UIEquipInventory.cpp:622` 显式预留「槽位数据待协议批次」）
- 宠物面板 4 格不可用态改色（同张 backgrnd 逐像素调色公式，r27-b3 命中率仅 34~77%，待原生展开帧）
- UtilDlgEx_Pet / MultiPetEquip 内部控件（Prev/Next/OK2/Cancel2/Exit 取 UtilDlgEx_Avatar 同族 + Basic.img 共享件，节点表未逐颗列宠物专属）
- 骑宠 Skill/BtRide（`UISkillBook.cpp:172`）、UserInfo BtPet/BtRide（`UICharInfo.cpp:90/93`）仅代码引用，节点表有尺寸但本面未做像素级校准

【完全无 UI 资源证据】
- 召唤兽 / 龙 / 骑宠：无任何独立截图 PNG、无 layout JSON、无 Photoshop .psd 产物、无 UIWindow.img 顶层节点（全表仅 2 个 UtilDlgEx_*）；只有渲染层 .nx 精灵节点（Skill/Mob/TamingMob）与 Angel 反编译
- locale：仅 4 个 stub 键（zh-CN 全英文未译），无 summon/dragon/mount/骑宠 文案
- Configuration：仅 ui.autopot.hp_item/mp_item 两项（默认 0），无宠物名/骑宠/排除物品/召唤落盘设置
- 结论：UI 排版/样式/交互层证据 **集中在「装备窗宠物区」一个面板**；召唤兽、龙、骑宠三块在「UI 资源层」几乎空白（只有精灵渲染 + 反编译 + 软键硬编码字符串），若要还原这三块的原生 UI 需补 orange-wz(IMG) 实解析或原生展开帧。

（全程只读，未修改任何文件；基准 083/v83，未引 095/v95 线证据作依据。）
