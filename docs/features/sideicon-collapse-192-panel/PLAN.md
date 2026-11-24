# 192 `ShortCutUI` 横向弹层还原 · 方案（2026-11-21）

## 用户诉求
高版本侧栏折叠：一个入口图标（btMain），点击后弹出**并排功能列表**。拍板按 192 `ShortCutUI` 真·还原。

## IDA 一手（MapleStoryT.i64 / 13339，字段/地址级）
### 关键纠偏（必须遵守，不能凭印象）
- **展开面板屏幕居中**：`l=(screenW-w)/2, t=(screenH-h)/2`（ctor @0x117bdb0），**不是**贴底栏向右弹出。"向右"仅指图标在面板内自左向右排。
- **btMain 是点击 toggle，不是悬浮**：`CUIContentsShortcutOpen::OnButtonClicked@0x117cfd0`，nId=2000，有实例→Destroy（收缩），无→CreateInstance（展开）；**不写回 collapsed 配置**，全库无 collaps 字段。
- **无四态显式代码**：btMain 是 `CCtrlButtonWithAniState`，点亮靠 `Animate(1,0)`（CheckQuestID），点击复位 `Animate(0,0)`。四态资源在 live btMain 节点存在，按资源四态接即可。
- **图标只取 `/normal/0` 单帧**，不走 Appear/Default/Notify 序列（全库 ShortCutUI/.*Appear|Default|Notify = 0 命中）。用户提供的参考包序列帧是"活动通知图标"资源，非本弹层图标。

### 类/字段
- `CUIContentsShortcut` size 3200：`m_lm@0xB64`(CLayoutMan)、`m_aContentsInfo@0xC6C`(ZArray, 每项32B: sName/aQuestID/nQuestIndex/sToolTip/sResourcePath/nDateS/nDateE/nUIIndex)、`m_pTextBox@0xC70`、`m_pFont@0xC74`、`m_nTopHeight@0xC78`、`m_nMidHeight@0xC7C`
- `CUIContentsShortcutOpen` size 444：`m_pAniButton@0xAC`(btMain, ID2000)、`m_lm@0xB4`
- `ContentsInfo`(32B) 字段见上

### 布局公式（一手）
```
nLine = ceil(count / 6)                      // 每行 6 个
w = top_w (353)                              // 面板宽 = top 贴图宽
h = top_h + middle_h*nLine + bottom_h        // = 33 + 92*nLine + 33
面板屏幕居中: l=(screenW-w)/2, t=(screenH-h)/2, z=10
图标 i: x = ((56-iconW)>>1) + 56*(i%6) + 10
        y = 92*(i/6) + 33
        资源 UI/UIWindow4.img/ShortCutUI/contentsIcon/{resourcePath}/normal/0
        nID = 2000+i
```
- 数据源 `Etc/ContentsShortcut.img` 遍历：openUI_idx→nUIIndex、resourcePath→sResourcePath(默认default)、toolTip、questIDs(可"123-125"区间)；保留条件 nUIIndex!=-1 或 questIDs 非空。
- dispatch：`SelectIcon@0x117a460`，nUIIndex>-1→`CWvsContext::UI_Open(nUIIndex)`（特例34=Messenger）；nUIIndex<0→任务型：按 quest map 过滤→CUtilDlgEx→StartQuest。点后 `UI_Close(195)`。
- btMain 宿主窗 @0x117aa90：相对小地图定位（`GetAbsLeft + ...`、`+25*n` 相邻 guide 下移），但本端按"侧栏竖栏"接入，btMain 挂在竖栏上（见下）。

## 资源（probe 实证）
live `UIWindow4.img/ShortCutUI` **完整存在**：top 353×33 / middle 353×92 / bottom 353×33 / textBox 53×14 / btClose 四态 / btMain 四态+ani / newIcon 0..3 17×17 / default 37×37 / contentsIcon{default,hekaton,bossArena,crossHunter}。
live **`Etc/ContentsShortcut.img` 缺失**——需从 192 XML（`/e/MXD/【2023.8.8江浩生日福利】怀旧岛192整合版/MapleServer192/wz/Etc.wz/ContentsShortcut.img.xml`）转 key 导入，或按 XML 内容在本端构造数据（推荐后者：少量静态数据，不引 WZ 跨 key 风险）。

## 本端实现方案（Journey-native，与现有竖栏共存）
1. **不破坏现有竖栏**：`UISideIconBar` 保持常驻竖排；在竖栏**顶部/底部加一个 btMain 入口按钮**（用 live `UIWindow4.img/ShortCutUI/btMain` 四态，MapleButton），替代之前被移除的折叠钮位置——这就是"收缩时的一个入口图标"。
2. **新窗类 `UIContentsShortcut`**（UIDragElement 或纯 UIElement，屏幕居中弹层）：
   - 背景：top/middle/bottom 三段按 nLine 拼（353 宽，高 33+92*nLine+33），屏幕居中
   - 图标：从 `ContentsShortcut` 数据表（本端构造，字段同 IDA ContentsInfo）生成，每项 MapleButton，坐标按 56×92 pitch + 6 列公式（IDA 一手）
   - btClose 四态关闭钮
   - tooltip：hover 图标显示 sToolTip（Locale/本地化数据）
   - 点击图标 dispatch：nUIIndex→本端 UIElement::Type 映射（需一张 ContentsShortcut UI 索引→本端 Type 的表；无对应则提示），questIDs→本端任务逻辑（若无则提示"任务型暂不支持"）
3. **数据源**：本端 C++ 静态表（从 192 XML 抠出：BossArena/CrossHunter/ItemPot/Bits/maplechat 等，含 toolTip/resourcePath/openUI_idx），不导 WZ（避开跨 key）；图标用 live contentsIcon 已存在的 default/hekaton/bossArena/crossHunter，其余缺图标的回退 default。
4. **开关**：复用 `ui.side_icons` 总开关；btMain 只在竖栏可见时显示。
5. **ESC/关闭**：点 btMain toggle、点 btClose 关闭、ESC 关闭（send_key），不开新 opcode（纯客户端 UI + 本端已有 UI 打开机制）。

## 与用户描述的对应
- "收缩时一个入口图标" → 竖栏顶部/底部 btMain 钮 ✓
- "点击向右展开并排功能列表" → 点击弹**屏幕居中的 6 列横向图标面板**（IDA 证实原生居中，"并排"=面板内横向排列）✓
- "悬浮触发" → IDA 证实**不支持悬浮**，仅点击；如实告知用户（不臆造 hover 展开）

## 真机验收点
btMain 显示→点击弹面板（居中/6列/图标/底图三段）→hover 出 tooltip→点图标开对应窗→btClose/ESC/再点 btMain 关闭→与现有竖栏共存不冲突。

## 待实现（一次成型）
按上述落代码，全量 Rebuild，部署（需用户关客户端解除 exe 占用）。
