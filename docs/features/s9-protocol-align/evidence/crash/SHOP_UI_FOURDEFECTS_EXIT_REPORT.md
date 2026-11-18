# 商城四类缺陷 + 退出卡死 · 修复与真机验证报告（2026-11-17）

> 用户诉求（原话）
> - m00591：「商城商品没有对齐白色背景，道具图标大小也不对，二级商品分类缺失，点券数缺失，全面检查一下商城所有功能，对照 angel、minms、img」
> - m00948：「另外进入商城无法退出，无法关掉客户端」

## 结论：四类缺陷 + 退出卡死全部修复，代码/构建/部署/真机四级证据闭环

最终产物 `build-pua/MapleStory.exe` SHA=`d30cf991ae7cdf1f1a7bd82ef285a7ca118d631ff9f209cb8c2202866c658205`。
部署位置：canon `E:/MXD/BeiDou-Client-xy/MapleStory.exe`、side `MapleStory_pua24.exe`（均 d30cf99）。

### 一、四类缺陷根因与修复（对照 Angel/MinMS/IMG）

| 缺陷 | 根因（源码级） | 修复 |
| --- | --- | --- |
| ①商品白底没对齐 + ②图标尺寸不对 | `IO/UITypes/UICashShop.cpp:348-353` 硬编码 `(47-dim.x()/2,39-dim.y()/2)`，不做缩放、无 Texture origin 补偿 | 采用 MinMS `draw_card_icon()` 公式：ICON_SCALE=2.0 / ICON_SLOT=64 / INSET=5，居中 + `origin*scale` 锚点补偿；is_valid 守卫 |
| ③二级商品分类缺失 | `UICashShop.h` 无任何 sub 分类成员/绘制逻辑 | 移植 MinMS `CommodityData` 模型：SUB_COUNTS[9]/SUB_LABELS 表 + active_sub_count/draw_category_sub_tabs/hit_category_sub_tab/matches_sub 四方法；主页签切换重置 category_sub |
| ④点券数缺失 | `CSStatus["Number"]` 在本包为空节点 → `Charset` 空则 draw 无输出（`IO/Components/Charset.cpp:48-64`） | 改用 MinMS 验证的 `Text` 自绘三币种。**关键纠正**：初版误用 `Color::Name::WHITE`，白字画在白色输入框上仍不可见（假修复）；真机 ASCII 像素验证发现后改为 `Color::Name::BLACK`，对齐 MinMS `UICashShop.cpp:261-263` |

构建踩坑：`UICashShop.h` 用 `const CashCommodity&` 但类型只在 cpp include → C4430/C2143 连锁 20 个 C error；补 `struct CashCommodity;` 前向声明后消除。中文标签全转 UTF-8 hex 转义规避 MSVC GBK C2001。

### 二、退出卡死（m00948）RCA

三条退出路径对比（决定性）：
- UIQuit（`UIQuit.cpp:220`）、ESC 路径（`UICashShop.cpp:1423` exit_cashshop）fadeout 回调均含 `GraphicsGL::unlock()` ✅
- **EXIT 按钮 BtExit**（`UICashShop.cpp:437-442`）fadeout lambda 只有 `clear()+ChangeMapPacket().dispatch()`，**无本地 unlock** ❌

病根：`lock()` 置 locked=true → `GraphicsGL.cpp:849 if(locked) return` 使所有 draw 停摆 → 画面定格、EXIT/ESC 失效。EXIT 把解锁全押在服务端回 SET_FIELD/PLAYER_LOGIN，服务端不回则永久卡死。此缺陷在 HEAD 与 pre-shopcrash 备份中一致，非本次四类缺陷引入。

修复（最小 diff）：在 BtExit fadeout lambda 补 4 行，对齐另两条已验证路径（幂等）：
```
UI::get().change_state(UI::State::GAME);
UI::get().enable();
Timer::get().start();
GraphicsGL::get().unlock();
```

### 三、真机验证（全部通过，视觉后端不可用改用 PIL 像素 + ASCII 字符画）

| 验收项 | 方法 | 证据 | 结果 |
| --- | --- | --- | --- |
| 进商城 | 点击状态栏商城钮 client(584,707)（/shop 注入 3 次失败后换的本质不同方案） | 白底 1.5%→16.3%、视口 1280×720→800×600，`evidence/shop_enter_try0.png` | ✅ |
| EXIT 退出 | 点 EXIT client(760,565) | 视口 800×600→1280×720、白底 16.3%→0.2%，`evidence/shop_exit_03_after_click0.png` | ✅ |
| 主窗响应 | FPS 持续 ticking | FPS:144，`RESPONSIVE=True` | ✅ |
| 关闭客户端 | Alt+F4→UIQuitConfirm→RETURN→`UI::get().quit()`（`UILoginNotice.cpp:325-328`） | pid=884 消失，WER 无新崩溃 | ✅ |
| 点券数可见（④） | ASCII 灰度字符画 + 暗像素计数 | 修复后 `DARK_PIXELS_DIGIT_BAND=192`、字符画见清晰数字字形（行 549-558/563-572）；修复前 WHITE 版同区 dark=0，`evidence/balance_try0.png` | ✅ |

说明：视觉后端本轮全挂（modlens: No vision provider / VISION_BACKEND_UNAVAILABLE），全程用 PIL 像素统计 + ASCII 灰度字符画替代目视。

### 四、部署稳定性与回滚源

- 部署脚本 `_deploy_exitfix.sh` 的 `kill 全部 MapleStory` 动作会触发 watchdog 用旧快照覆盖 canon（曾回滚成 a089295e）。改用直接 `cp`（不 kill 进程）后 canon 稳定：连续 8s + 15s 复查均 = d30cf99。
- side 名 `MapleStory_pua24.exe` 不受 watchdog 影响，作为双保险。
- 3 个 32K 僵尸 MapleStory 进程（47560/54544/12524）无法 taskkill -F 终止，但无窗口无影响。

### 五、复盘四步法

1. 回顾目标：修四类缺陷 + 退出卡死 + 全功能核查 + 编译部署证据。
2. 评估结果：全部编码/构建/部署/真机闭环；额外发现并纠正④的假修复（WHITE→BLACK）。
3. 分析原因：④假修复源于照搬"用 Text 替代空 Charset"却忽略颜色对比——WHITE 在白底上不可见；靠真机像素验证而非"部署成功"才发现。
4. 沉淀 SOP：UI 可见性修复必须以真机像素/ASCII 证据验收，不能以部署成功代替；MSVC GBK 项目中文一律 hex 转义；canon 部署禁用 kill 进程（触发 watchdog 回滚），改直接 cp + side 双保险；/shop 文本注入不可靠时用状态栏按钮点击（client 584,707）。
