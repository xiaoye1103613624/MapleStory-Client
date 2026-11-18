# 商城四项深挖 · 环境与约束底稿（2026-11-17）

## 部署槽位（真机验证地基）
- 构建产物：`build-pua/MapleStory.exe`，当前基线 SHA=`c0de2fd2...`（卡片对齐+余额修复版）。
- 稳定验证槽：`MapleStory_pua25.exe` = c0de2fd2（watchdog 不碰 side 别名，推荐后续验证都用它）。
- canon `MapleStory.exe` / pua24：10:48 起被另一路部署成 `902c38b7...`，10:58 有活跃实例 pid=23088 占用。不主动 kill canon 实例（会触发 watchdog 回滚），需要真机时先优雅退出它释放 admin 账号。
- 3 个 32K 僵尸进程 47560/54544/12524 无法终止、无窗口、无影响。

## 真机验证固定套路（已验证有效）
1. 启动：`subprocess.Popen(['cmd','/c','start','','MapleStory_pua25.exe'], cwd='E:/MXD/BeiDou-Client-xy', shell=True)`。
2. 锁定新进程窗口：EnumWindows 找 title 含 `XiaoYE` 且 `Admin` 的可见窗口；GBK 标题中文乱码但 ASCII `Admin`/`1-` 可用。
3. 登录冲突：若卡"账户或正在检查服务器，请再确认"确认框 = admin 单点冲突，先优雅退出旧实例（Alt+F4→RETURN 走 UIQuitConfirm），或点确认框"确定"重连。
4. 前台化：AttachThreadInput(mt,gt/ft) + SetForegroundWindow（直接 Set 常失败）。
5. 坐标：DPI 缩放下 GetClientRect=逻辑 1280×720、GetWindowRect=物理 1296×759；用 `ClientToScreen` 取 client origin（实测 (640,424)），点击用 client 坐标 + origin。
6. 进商城：点状态栏商城钮 client(584,707)（/shop 文本注入 3 次全失败，点击一次命中）。判据：视口 1280×720→800×600、白底比例 1.5%→17%。
7. 截图：ImageGrab.grab(bbox=(origin, origin+client_size))。
8. 视觉后端全挂（modlens VISION_BACKEND_UNAVAILABLE），只能用 PIL 像素 + ASCII 灰度字符画判读。

## 关键脚本
- `tools/_cardalign_verify2.py <pid>`：进商城 + 商品卡片放大 + NAME/PRICE 暗像素采样（THE 卡片验证器）。
- `tools/_category_verify.py <pid>`：分类页签切换 + 子分类栏验证。
- `tools/_balance_verify2.py`：余额区验证（抓中文字符画）。
- `tools/_shop_enter_click.py` / `tools/_shop_exit_verify.py` / `tools/_shop_confirm_quit.py`：进/出商城 + 正常退出。
- 构建：`tools/_build_uifour.sh`（MSBuild Release x64，输出到 build-pua/）。**必须 rm build-pua-int/UICashShop.obj 强制重编**，核对 obj mtime > cpp mtime + exe SHA 变化 + error C=0。

## 源码要点（UICashShop.cpp / .h）
- 卡片三要素修复见 `tools/_apply_cardalign.py`：图标 fit_scale clamp（ICON_SLOT=64）、名称/价格 LEFT+A13B+BLACK+NAME_X=76、ellipsis=116。
- 余额区：数字锚点 `WND_STATUS_X+216`（:308，标签在左数字在右，MinMS BALANCE_VALUE_X=462 思路）。
- 商品数据：`UIStateCashShop::cashshop_commodities`（目录）、`cash_best_sellers`（热销 sn 序列）、`cash_inventory`（购物车/已购，0x4B showCashInventory 落地）。
- 预览：`preview_actor`(Player)/`preview_physics`(Physics)/`preview_sprites[3]`/`avatar_look`(CharLook)/`avatar_baseline`；`apply_try_on_item`(:1340) 只处理脸(2)/发(3)/装备，宠物/特效 0 命中。
- 翻页：`list_slider`(:232 构造/:409 draw/:898 send_cursor)、`list_offset` 成员。

## orange-wz MCP
- AGENTS.md 规定 WZ/IMG/XML 读取优先 orange-wz MCP（128B key base64，exclusive:false），但本 harness（DSH）当前未暴露 mcp__orange_wz__* 工具（ops_discover_tools 无匹配）。涉及 WZ 节点存在性（如 Cash item effect img、CSInventory.img）的前置验证暂不可做，报告中标注为待验前置。
