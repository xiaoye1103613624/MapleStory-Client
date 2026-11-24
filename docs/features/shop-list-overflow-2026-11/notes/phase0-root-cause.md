# 商店列表溢出 UI / 未对齐背景 · Phase0 研判（2026-11-20）

## 现象（本端截图已确证）
用户本端截图 `XiaoYe_2026-11-20_08-24-55.png`（左下竖栏图标+雨滴天气=本端）：NPC 商店双列商品（野狼头巾/橡胶雨鞋…）文字溢出背景板下缘、右列压在场景上、与背景槽位错位。

## 根因（逐行号，点到面）
| 证据 | 位置/值 | 结论 |
|---|---|---|
| 布局常量 | `UIShop.cpp:94 item_y=124`、`:95 item_height=36`、`:102/:112 42*i`、`:124/:137 Slider Range(123,484)`、`:97 buy_x=8 / :107 sell_x=284 / :108 sell_width=200` | 代码按 **~499 高背景**设计：9 行从 y=124 到 y=496；右列到 x=484 |
| 代码来源 | git blame `:94-150` 全 = 2019 Ryan Payton（Journey 原版），`git diff` 本机未改 | **这就是 Angel/v83 原生布局**，非私改 |
| live 资源实际 | orange-wz 实测 `UIWindow.img/Shop/backgrnd` = **463×339**，origin(0,0)；pngFormat ARGB8888 | 背景板比代码预期**矮 160px** → 商品溢出 |
| 需求包资源 | `商店列表加长/backgrnd.webp` = **463×499**，两列各 **11 行**槽位，顶部角色预览+分隔线（已读图确认） | 与代码 42px 行高/484 下界完全吻合，**这是原生加长板** |
| 资源时间线 | live UIWindow.img 时间戳 2026-11-19 14:57（近期被动过）；需求包 webp 2026-07-13 | 疑似近期把加长板换回矮板、代码未同步 |

**结论：不是代码逻辑错，是资源被换成矮板导致原生布局失去依托。**

## 修复方向（二选一，需用户拍板）
- **A（推荐，符合"还原 Angel 原生"）**：把 `商店列表加长/backgrnd.webp`（463×499）作为 PNG 写回 live `UIWindow.img/Shop/backgrnd`，代码零改动。走 orange-wz MCP：`load_files(live UIWindow.img, GMS key)` → `mutate_nodes set_png(nodePath=Shop/backgrnd, filePath=png)` → `save_as` 异路径 staging → probe → 原子替换回 live。全程备份原 img，禁裸拷。
- **B**：若 339 矮板是**故意**换的（适配某新版 UI），则反向把代码改矮：9 行→6 行（或缩行高）、slider Range(123,~330)、sell_width 适配 463——但这偏离"Angel 原生"，且改的是逻辑。

## 待用户确认的关键问题
1. live 14:57 那次 UIWindow.img 改动换矮背景板是**有意**还是**误操作/回滚**？
2. 用户要的"还原 Angel 原生"= 恢复 463×499 加长背景（A）？还是保留矮板、把代码也改矮（B）？

## 修复执行（2026-11-20，用户拍板 A：恢复加长板 + 矮板非有意）
1. 备份 live `UIWindow.img`（11,833,578B）→ `E:\MXD\_shop_fix_staging\UIWindow.img.live-backup-20261120-085822`
2. 需求包 `商店列表加长/backgrnd.webp`（463×499）转 PNG（130,189B）
3. orange-wz MCP：`load_files(staging 工作副本)` → `mutate_nodes(set_png, nodePath=Shop/backgrnd, pngFormat=ARGB8888, base64Png)` → `save_as(异路径 UIWindow.fixed.img)` → `unload_all`（全程未回写 loaded 根，符合 AGENTS.md）
4. 完整性 probe：原版备份与修复产物均 **113 根节点**（save_as 未毁树）；backgrnd 由 463×339 → **463×499**
5. 原子替换回 live：`cp UIWindow.fixed.img → live/UIWindow.img`
   - 替换前 live SHA `ADB57A16…` → 替换后 live SHA **`FEF0A47F6519191619DACE4BDF562F99932BD2F40D9CC59E137E72A7805A3415`**（与产物 SHA 完全一致）
6. live 终验：load isError:false、**113 节点**、Shop/backgrnd=**463×499 ARGB8888**、isError:false

**代码零改动**（原生 9 行布局本就匹配 499 高背景，git blame 证实 2019 Journey 原版）。

## 真机验收（待用户，如实声明未验收）
重启客户端 → NPC 商店：两列商品应完整落在背景板槽内、下缘不再溢出场景、滚动条正常。截图须本端。
回滚：如需撤销，把 `UIWindow.img.live-backup-20261120-085822` 复制回 live 即可。
