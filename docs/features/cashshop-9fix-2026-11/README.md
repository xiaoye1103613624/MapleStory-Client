# 商城九症状一次性修复（2026-11-18）

> 本端截图确认后一次性闭环。带可见 PUA（🔴 华为味 / RCA），不记评分。
> 全程先证据后改动：orange-wz 实测 + MinMS 源码对照 + live IMG 像素测量。

## 1. 目标与证据

用户列 9 项商城问题，要求全部改完。根因猜测"angel/img/minms 缺失"经证据**证伪**：

| 证据源 | 结论 |
| --- | --- |
| orange-wz MCP（10012，128B key，`iv=TSPHKw==`） | `CashShop.img` load OK；`Base/Tab{Tab}`、`CSTab/Tab/1..9`、`CSList/{Base,BtBuy,BtGift}`、`CSStatus/BtCharge..BtExit`、`CSChar/BtBuyAvatar..`、`Base/Tab2/Enable\|Disable/0..4`、`PicturePlate/*` 全在；仅 `Base/BestNew` 不存在（代码已按可选处理） |
| MinMS `E:/project/MinMS/src/client/IO/UITypes/UICashShop.cpp`（3440 行） | 完整移植源在位 |
| Angel IDA `docs/features/cashshop-angel-calibration-2026-11/notes/r01-...md` | 938/938 EA 一致，七子窗 OnCreate 坐标已钉死 |

**真因**：`UICashShop.cpp` 移植时砍掉/错位了若干展示层，不是上游资产缺失。
IMG 几何实测：`backgrnd` 800×600、`CSTab/Tab/N` 508×78、`CSList/Base` 200×80（白图标井 x5–68、浅色文字面板 x72–195，分隔线 y≈24）。

## 2. 修改清单（`IO/UITypes/UICashShop.{h,cpp}`）

1. **已购道具空**：补左侧「已购道具」2×6 格绘制，数据取服务端权威 `cash_inventory`（0x4B/0x57/0x89）；格子 x=21+/y=349+，dx=dy=35（backgrnd 像素实测）。
2. **商品名称不在浅色背景 / 售价偏下**：名称/价格基线由 y=12/32 调到 y=17/38（A13B 13px 落在 y1–19 / y25–71 两条浅色带）；NAME_X 76→74。
3. **商品信息字体不对**：折扣/百分比由 A11M 升 A13B，与 MinMS `cpp:1279-1280` 一致。
4. **二级分类字体**：已为 A13B/CENTER/BLACK；标签落入烘焙色带并加统一背景框（见 #7）。
5. **分页 + 切换 UI**：移除与页码/箭头并存的 `THIN_MINESHAFT` 滑条（构造 + draw + 命中 + 成员全删）；页码 A11B/WHITE→A13B/BLACK，位置对齐列表底边空白条。
6. **热销榜无数据**：补右侧 5 行图标绘制（数字/底纹烘焙在 backgrnd，只填 x=742–773 浅色槽，行距 69）；`refresh_best_icons()` 按 `cash_best_sellers` 回查目录，构造 + `rebuild_items` 双刷新以应对异步数据。
7. **背包类型切换 UI**：读 `Base/Tab2/Enable|Disable/0..4`，补 4×3 格 + 竖排 5 个类型 tab 绘制与命中（`backpack_tab`）。
8. **二级分类偏上/不在背景中**：条带改为 508 宽（原 412）、局部 y=42/h=24（原 y=62/h=22），对齐 `CSTab/Tab/N` 贴图下半部烘焙彩色带；命中区上下扩 4px。
9. **余额偏下**：三行数值右对齐到 x=484、y=546/562/578（原 (470,538) 起，压在蓝字标签行）。

## 3. 验证门禁

- `build_cashshop9fix.bat` → **EXITCODE=0**，`error C/LNK` 计数 **0**（`cashshop9fix_build.log`）
- 产物 exe mtime 07:53:13 > 源码 07:52:41（确认重新编过）
- 部署 SHA（2026-11-18 实时）：产物 = live = `e4fd1860f0f30a4c553d3aec600aab1578eca5babcb9987b2f8df78b29f62094`
- 部署前确认无客户端进程占用；旧的 `MapleStory_pua24.exe`（另一功能快照）未覆盖
- 临时探针脚本全部清理；IMG 证据 PNG 留存 `evidence/`

## 4. 声明与剩余风险

- **真机未验收**：本 agent 无 GUI 操作权。请按原路径复核九项；登录器可能回滚 exe，复核前实时核对 live SHA。
- 已购/背包复用 `cash_inventory` 数据（服务端权威）；若期望"背包"读角色实际现金背包而非已购未取，需服务端补发包，属后续范围。
- 热销榜图标依赖服务端 `cash_best_sellers` 有值；新服无榜单时空槽为正确空态。

## 5. 产物

- `evidence/backgrnd.png`、`evidence/cslist_base.png`、`evidence/tabs/tab1..9.png` 及放大测量图 —— live IMG 原生几何取证
- 源码改动：`IO/UITypes/UICashShop.h`、`IO/UITypes/UICashShop.cpp`
- 构建：`build_cashshop9fix.bat`、`cashshop9fix_build.log`
