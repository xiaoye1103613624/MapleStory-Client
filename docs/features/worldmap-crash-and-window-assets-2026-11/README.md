# 竖栏三窗口专用 UI 素材 + 闪退修复 · 最终交付（2026-11-21）

## 用户诉求与上轮纠错
用户看本端截图指出：ColorPrism/DailyCheckin 背景是**商城蓝色格子**（我上轮错用 `Shop/backgrnd` 当通用板），要求用需求包**专用素材**；并强调"以后部署必须全部到 live exe"（不再旁挂变体）。本轮一次改全。

## 三套专用素材（全部经 orange-wz 导入 live WZ/IMG，probe 验证）
| 窗口 | 专用素材 | 落位 | probe 结果 |
|---|---|---|---|
| ColorPrism | 需求包 `coloring-prism/wz/client/UI/UIWindow/ColorPrism.img`（76KB GMS头）转 key | live `Data/UI/ColorPrism.img` | 6 节点：backgrnd 301×406 / trackTone/Chroma/Bright 176×8 / backgrndLook / LayerBt |
| DailyCheckin | docs `daily-checkin/ui/window/v4/normal.png`（513×346 跟图重绘）set_png | live `UIWindow.img/DailyCheckin/backgrnd` | 根节点 113→114（未毁树），backgrnd 513×346 ARGB8888 |
| DamageRank | 需求包 `个人伤害统计/UI_custom2.wz` 内 9 个 canvas（includePng 导出→set_png） | live `UIWindow.img/DamageRank/{backgrndmax,backgrndcenter,backgrndbottom,title1,title2,iconCommonAtk,iconUnknownSkill,...}` | 根节点 114→115，9 节点尺寸一致（223 宽分段） |

导入脚本与 staging 备份在 `E:\MXD\_ext_import_staging\2026-11-20\`。

## 代码指向专用素材（去掉商城蓝格）
- `UIColorPrism.cpp`：读 `nl::nx::UI["ColorPrism.img"]["backgrnd"]`（+ backgrndLook 供 Hair/Eyes/Skin 页），删 shop_root 回退；补 `<nlnx/nx.hpp>` include。
- `UIDailyCheckin.cpp`：只读 `window("DailyCheckin")/backgrnd`（专用），删 Shop 回退。
- `UIDamageRank.cpp`：读 `window("DamageRank")` 分段 backgrndmax/center/bottom，draw 拼背景（max 52 + center 21 循环铺 + bottom 3），删 Shop 回退；h 加 3 个 Texture 字段。

## 闪退修复（上轮，本轮产物含）
`mapid<=0` 三处守卫（Misc.cpp get_map_node_name/get_life_on_map、MapTooltip.cpp set_mapid），根因 `get_map_node_name(0)→ensure_map(0)` 堆损坏 0xc0000374；file 垫片定位、垫片已清。

## 构建与部署（三证 + 铁律）
全量 Rebuild `BUILD_EXITCODE=0`、链接行、error=0，产物 `build-final-assets/MapleStory.exe` 5123584B。
**按用户铁律部署**：备份 live 原 `MapleStory.exe` → 覆盖为正式名 → 实时核对 live SHA = 产物 SHA = **`873CC6437B27E98B2A01AD211DE4D8C3931C44FD7CAD87ED81A1B3BA9C7064DC`**（00:33:11）。无旁挂变体交付。

## 真机验收（待用户，如实声明未验收）
当前 live 正式 exe 即含全部修复：①侧栏 ColorPrism/DailyCheckin/DamageRank 应显示**专用素材**（染色窗 301×406 专用板+滑条轨、签到 513×346 跟图窗、伤害统计 223 宽分段板），不再是商城蓝格；②滑入大地图任意点位不闪退。截图须本端。

## 回滚
- exe：`MapleStory.exe.bak-before-assets-20261121`
- UIWindow.img：`UIWindow.img.bak-before-checkin` / staging 副本
