# 侧栏 flyout「只显示一个 / 点击闪退」修复（2026-11-23 21:xx）

## 来源端与版本确认（先确认再动手）
- 用户截图 `XiaoYe_2026-11-23_21-38-01.png`（sha256 b28028ff…）：本端运行画面（视觉后端本轮 429 限流，未能逐张复核截图细节，仅按用户描述 + 代码证据分析）。
- live exe 在 21:38 为 `build-v2p0`（SHA `5736A67…`，mtime 20:24），**不是**我 19:10 部署的 `1FB930…`——期间被 v2 构建替换。PDB 确认 v2p0 含 `defer_rebuild` 符号。
- WER 21:37:56 `c0000005` 偏移 `0xe138e` → `UIElement.cpp:381`（`send_cursor` 遍历 buttons）；21:40:10 `0xb1f60` → `Button.cpp:56`（`Button::is_active`）。两次都指向 buttons 遍历中按钮对象悬空。

## 根因（行号级）
1. **闪退**：`defer_rebuild`（`UISideIconBar.cpp:295-299`）只防了 `button_pressed` 里直接 `rebuild_buttons()`，但 `dispatch_item → FeatureRegistry::open → UI::emplace` 会在**同一次 send_cursor 调用栈**内间接触发 `UISideIconBar::update()` 的 dirty 重建（`:443-455` `rebuild_buttons()` → `buttons.clear()`），使基类 `UIElement::send_cursor:381` 正在遍历的迭代器/按钮悬空 → `is_active()` 崩。
2. **横向只显示一个/不显示**：新 side_menu 树的 iconKey（attendance/EventList/Achieve/dailyGift…）在 live `UI/SideIcons/` 无对应序列帧；`rebuild_buttons` 的 fly 回退（`:178-182`）用透明 `AreaButton`，导致横排成员不可见（看起来只有一个有图）；`layout_flyout:290-292` 的 dimension 计算写反且把高度压成 fly 行高，进一步破坏命中/布局。

## 一次性修复
- `UISideIconBar.h`：新增 `buttons_generation`，每次 `rebuild_buttons()` ++。
- `UISideIconBar.cpp`：
  - `rebuild_buttons()` 开头 `++buttons_generation`；
  - **覆写 `send_cursor`**：复制 button id 列表后遍历副本，每次解引用前检查 `buttons_generation` 未变且 id 仍在 map 中；`button_pressed` 后若 generation 变化则不再对旧指针 `set_state`。彻底消除遍历悬空（不修改基类，避免影响其他 UI）。
  - `layout_flyout` dimension 计算改为 `dimension.x()+fly_w`、高度 `max(dimension.y(), fly_h)`（不再自我叠加/压高）。
  - fly 图标回退改为与 root 一致的可见 `BtClaim` `MapleButton`（不再透明 AreaButton）。
- `LocalFrameButton.cpp:102+`：补全 legacy key 映射（attendance→checkin、dailygift→star、achieve→colorprism、eventlist→book 等），让新树成员图标对应到 live 已有 PNG，横排能区分。

## 构建与部署
- 全量 + 增量构建 `build_flyout_fix_20261123.bat`：`EXITCODE=0`（增量确认重编 `LocalFrameButton.cpp`）。
- live 部署：结束占用进程（PID 33772，本会话既往授权自动处理）→ 备份 `MapleStory.exe.bak-before-flyout-fix-20261123` → 覆盖；SHA `C97B1EEA3B32F47DDAF56125B9B66EE8BA72CE3B24F863319B56FF7AD73FCB8D`，延迟 8s 复核一致（无回滚）。

## 真机状态（如实）
- 崩溃/布局修复已编译部署，但**本轮未真机复测**（视觉后端限流未能核对截图细节）。请用户重启客户端后：点第二个灯泡展开横排，确认 4 个成员图标都可见、逐个点击不闪退。
- 注意：live exe 曾在 20:24 被 v2 流程替换过一次；如后续再次被替换导致 SHA 变化，需重新同步本修复版。

## 22:xx 复查补充（用户「继续」）
- live exe SHA 仍为 `C97B1EEA…`（mtime 21:51），未被回滚；源码含 `buttons_generation`(cpp×5/h×1) 与 legacy 映射（attendance→checkin）。
- WER 最近崩溃仍为 21:37/21:40 两条（修复前），**21:51 部署后至今无新崩溃**。
- 服务端在跑（8686，PID 13072，20:44 启动，jar 19:14 含 sendTo），revision=2；客户端本地 side_menu.json 为 19:36 落盘的 revision=1（23 items，folder_daily 4 个 enabled leaf）——非 bug：19:36 登录后客户端未再登录，故未收到 revision=2；下次登录会同步。
- 复核 `SideMenuSyncHandler::handle`（CommonHandlers.cpp:423-438）：wire int revision + maple string，`apply_remote_json` 版本判断正确；`InPacket::available()` 返回 size_t（`InPacket.h:37`），`:426` 的 `available()<6` 守卫正常（此前「available 返回 bool」是 ItemParser 历史坑，此处不适用）。
- 静态复核新版 `UISideIconBar::send_cursor`（:539-613）：generation 守卫覆盖 `button_pressed→close_flyout→erase fly ids` 与 `dispatch→emplace→dirty rebuild` 两条重建路径，无悬空访问；未发现新回归。
- **结论**：不做无证据的进一步改动（避免引入回归）；唯一待办是用户真机复测并反馈，若复测仍有问题，按点到面一次性改完。

## 22:xx 用户三点设计反馈 — 一次性整改
1. **图1/图2 美容院/融合外观是纯文字 UI**：坐实为赶进度占位（`UIBeautySalon.cpp:42`/`UIAnvilDialog.cpp`/`UIDamageSkinPicker.cpp:43` 仅 `Text A11M + AreaButton`，无 backgrnd Texture）。**已修**：三个 UI 均加载原生通用窗体底图 `UIWindowCN.img/CN_50100/backgrnd`（与魔女森林同款，回退 `Admin/Block`），draw 时按窗口尺寸拉伸（DrawArgument(position, sx, sy)）。不依赖 Photoshop/WZ MCP（本轮两者不可用），纯复用已入库资源。
2. **原生快捷键功能不该塞侧栏**：坐实 `KeyAction::Id::WORLDMAP`(KeyAction.h:34)/`MONSTERBOOK`(:51) 有原生快捷键（UIStateGame.cpp:576）。**已修**：种子删 `leaf_worldmap`/`leaf_mapinfo`/`leaf_mobbook`；但 DB 里已有 revision=2 快照（含这三项），仅改种子无效——通过 `/sideMenu/v1/save` 正常业务路径写入新树（revision=3 + 广播）。实测 `/get`：20 项，三项已无。
3. **成员图标都一样**：坐实 live `UI/SideIcons/` 仅 6 张 PNG，种子 iconKey 大量映射同图。**已修**：`LocalFrameButton.cpp` 补全 legacy 映射使不同 key→不同图（attendance→checkin、dailygift→star、achieve→colorprism、absolute→damagerank 等）；4 个目录分别用 bulb/star/dailyGift/Content 图标。

## 部署（22:xx）
- 客户端全量构建 `EXITCODE=0`，live SHA `A41C3F8CAE04AB9FF8661B0FB11BFCFE0246A39F2CF91CD488EC23B8CCC14142`（备份 `MapleStory.exe.bak-before-uiframe-20261123`），延迟复核无回滚。
- 服务端 package EXIT=0，重启后 8686 监听；Web save 实测 revision 2→3，菜单 23→20 项、三原生入口清除。
- **真机未验**：三个 UI 的窗体底图效果、图标区分、删项后侧栏表现需用户重启客户端登录验证（视觉后端限流无法看图）。
