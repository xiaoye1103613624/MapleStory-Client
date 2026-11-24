# HUD 对照还原笔记 — 2026-11-18

对照源：IDA `MapleStoryT.i64`（pid=31908 / port=13339 会话）、orange-wz `StatusBar3.img`（GMS IV）、Angel 源码。

## 1. quickSlot 空格（无图标/键位字）

**结论：代码缺绘制，不是缺节点、不是 origin 错位。**

- orange-wz：`mainBar/quickSlot` 仅有 `backgrnd`(557×67) / `layer:cover` / Fold·Extend；**无**槽内图标子树。键帽在 `StatusBar3.img/KeyConfig/key/*`（齐全）。
- IDA：`CUIStatusBar::CQuickSlot::ChangeScreenResolution` → 800/1024 显示 8 槽，其它 28 槽；`s_ptShortKeyPos@0x279c390` 为 28 点（实读 7/15、40/15…447/48）；槽命中 32×32。
- Angel：`init_modern` 只铺底板；`draw()` modern 路径**不**调 KeyConfig 映射画图标。classic 才有 `refresh_classic_qs_icons` + 键帽。

**已修：** modern 路径按 `s_ptShortKeyPos` 画图标+键帽；前 8 槽走 `Keyboard::quickslots`，扩槽用固定 `MODERN_QS_KEYS`；键帽优先 `StatusBar3/KeyConfig/key`。

## 2. BT_THEME（ThemePack）看不见

**多因叠加：**

1. live `BeiDou-Client-xy\MapleStory.exe` 时间戳 **2026-11-17**，晚于含 Theme/SideIcon 的 `hud_sidedock_out`（11-18）→ 运行中的可能是旧构建。
2. 竖长小地图使标题栏过窄，Theme 被挤到负 x / 文字下（`W_min` 原先未计入 `bt_theme_width`）。
3. 贴图原用 `BtHide`，观感像「隐藏」而非主题切换。

**已修：** 标题栏最小宽度计入 Theme；贴图改优先 `BtClaim`；需同步新 exe。

## 3. UISideIconBar 不显示

**配置正常：** `Settings` 有 `side_icons: true`，工作目录有 `side_icons.ini`，`UIStateGame` 已 `emplace`。

**真因倾向：**

1. 同上 — live exe 过旧可能无 SIDEICONBAR。
2. `MARGIN_Y=80` 落在通高小地图覆盖区内；虽后画，但贴图若失败/旧 exe 无此类型则「完全不见」。

**已修：** 布局时若小地图右缘 >40，竖栏挪到小地图右侧；每秒重锚。需新 exe。

## 4. Angel 小地图宽高上限

| 来源 | 上限 |
|---|---|
| Angel 旧源码 | **无**硬上限；`body = max(min, canvas)` → 竖图通高 |
| 192 IDA `CUIMiniMap::SetCtrl` | `m_nMini_W > screen_w-50` 或 `m_nMini_H > screen_h-150` 时禁用 Big；另有 `≤280×150` 小图禁用 ToggleSize |
| 192 `CalculateScr` | 用 `m_nPane_W/H << m_nMag` 做视口滚动（窗体仍按 canvas） |

**已修：** Angel 内容区硬夹到 `vw-50` / `vh-150`（对齐 SetCtrl 禁用阈值）；超限 canvas 裁剪绘制；标题栏保底宽度装下 Theme/±/大地图。

## 阻塞

- Release|x64 链接时 live `MapleStory.exe` 被进程占用（本机多个 MapleStory）。需关客户端后再覆盖，或先用 `hud_sidedock_out\MapleStory.exe` 验收。
