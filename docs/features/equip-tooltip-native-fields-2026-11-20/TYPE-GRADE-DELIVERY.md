# 类型细分 + 品级标识 + potrank细分 交付（2026-11-20）

> 个人学习/技术研究，单机离线。三张小册子截图为公开图鉴参考，非本端验收证据（USER.md）。

## 0. 需求与证据归纳（三路只读取证）
用户确认实现三块：①高版本装备类型细分；②品级标识（原要求"边框"，取证后翻案）；③potrank 细分到 SS/A。三路子代理只读取证（probe-type-map / probe-grade-border / probe-potrank）。

## 1. 功能1 类型细分（已完成，构建通过）
- 证据：v214 源码 `WeaponType.java:7-50` + `ItemConstants.java:1507-1528`（val=id/1000%1000）给出 ~23 个高版本武器号段实证；本机高版本 Character/Weapon 全是扁平数字 img（无类型子目录，翻案）。
- 实现：`Data/EquipData.cpp` 新增 `high_version_weapon_type(id)` mid3 查表（24 个 val→type），在武器分支与 `islot=Wp` 落空档分支调用；槽位仍 WEAPON，不动 islot/协议/背包。
- locale：`zh-CN.ini`/`en-US.ini` 各加 24 条 `equip.type.*`（魔道书/能量剑/ESP限制器/远古弓/机甲手枪/双弩枪…），UTF-8 校验通过。
- 边界（如实）：副武器细分（135 段调谐器/如意宝珠/念珠/护符/棋子/灵符/瞄准器/短剑剑鞘/箭羽/索魂器/装药/遗物/铁链）本机无号段实证，**未补、不编造**（门禁5）。中文名由职业译名推断，待用户复核。

## 2. 功能2 品级标识（翻案 + 已完成，2026-11-21 补完）
- 翻案：095/192 里**没有"装备品级边框"**——`Rank`(62-82px)是怪物击杀成绩徽章、`UniqueBox`(69x62)是组队奖励按钮、`Border`是世界地图九宫格，语义全部错位。原生 GMS v83 用 `UIToolTip.img/Item/ItemIcon/0..4` 小图标 + 彩色文字。
- **此前"192 打不开"是低级错误纠正**：用户当面质疑「192 是新版本客户端怎么会打不开」——实测发现 192 客户端 `Data/UI/UI.wz` 只是 **63 字节 PKG1 索引占位**，真正资源在分卷 `UI_000.wz`~`UI_008.wz`（30-98MB）。用 **零 IV(base64 AAAAAA==) + 128B key** 加载分卷全部成功（rootCount:1），`search_node UIWindow` 命中 155-1652，找到了 `UI_003.wz` 里的 `UIWindow.img/Item/Grade/0..5`。
- **像素已导出**：orange-wz `get_node_tree_json(includePng=true)` 导出 6 张 Grade canvas PNG（0-2=7×7 实心绿/蓝/紫，3-5=34-36px 灰/蓝/紫光晕），落 `resources/grade192/`。
- **实现**：`tools/gen_grade_pixels.py` 转 BGRA → `Graphics/EquipGradePixels.h`（84KB，6档 atlas id 0x4752440-5）；`EquipTooltip.h` 加 `MemoryTexture grade_tex[6]` + `draw_grade_badge()` 声明；`EquipTooltip.cpp` ctor SET_GRADE_TEX 6 档、draw/draw_preview 在图标框右上角按 `prank`/`prank_preview` 绘制。复用 EquipStarPixels.h 既有范式（MemoryTexture raw atlas，零 WZ 改动）。
- 结论：品级角标（文字副标题 + 图形角标）均已完成；角标资源为 192 原生 Grade，非编造。

## 3. 功能3 potrank 细分（已完成，构建通过）
- 证据：095 `getState()` 字节码（土制壳破译）从 5 潜能 id 数值段位推导等级（零服务端字段）；BWIKI 规则「第一条=当前等级，数值随潜能等级变化」；s9 协议只发 5 short、无 rank 字段（S10 有列也不下发）。
- 实现：`Character/Inventory/Equip.cpp` 新增 `potential_rank_of(p1)`（namespace 静态函数），按第一条 id 分段：0→NONE / 1-12→B(RARE) / 13-24→A(EPIC) / 25-36→S(UNIQUE) / 37-50→SS(LEGENDARY)，ctor 调用；零服务端/DB/协议改动。
- 文案联动：`tools/gen_equip_option_locale.py` 按等级分段重写 50 条 potline（1-12 固定值 / 13-24 +3% / 25-36 +6% / 37-50 +9%/+BOSS20/30/40 等），英文通用转换，重新应用（80条/语言，UTF-8 校验通过）。这样 SS 级装备配 +9% 潜能自洽。
- potflag（equip.pot.* 五档颜色文字）与副标题（equip.rank.* 五档）现有代码已消费 get_potrank()，现在能正确显示 B/A/S/SS 各档。

## 4. 构建/部署证据
- 最终构建（含品级角标）MSBuild EXITCODE=0，error C=0；产物 `typegrade_int/MapleStory.exe` SHA `edff40aa59e9e1e23705072b3ab525106f12a8e5f9989fd3e7750f873cf72f25`。
- **已部署 live 正式 exe**：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`（SHA 实时核对 = 产物 `edff40aa…`），按用户硬要求部署到登录器启动路径。
- locale 已同步 live（zh-CN/en-US）：24 新类型 `equip.type.*` + `equip.rank.*` + 50 条分段 potline + potline/spark/soul 80 条，UTF-8。

## 5. 如实声明
- 三功能为源码层 + 构建 + 资源导出证据，**未真机验收**（4 个旧客户端进程 11-17~19 启动、taskkill 权限不足需用户手动关闭重启）。
- 功能1 中文类型名由职业译名推断，待用户复核；副武器细分（135 段）无号段实证未补、未编造。
- 三张小册子图为公开图鉴参考；095 客户端 VMP 壳不可反编译，等级/类型逻辑来自服务端源码 + 明文 XML + 192 分卷 WZ 导出 + BWIKI。
- 品级角标源自 192 `UI_003.wz` 的 Grade/0..5（真实像素），非想象填充。
