# 自主启动进图截图验证（2026-11-19，用户要求"改完自己启动验证，错误就修改"）

方法：`tools/_turn10_capture.py`（ImageGrab 全桌面，避开 BitBlt 对 OpenGL 黑屏）→ 裁剪游戏窗口 [632,393,1928,1152] → 放大底部/聊天区。单实例验证（重复实例 taskkill 去重，避免重复登录被服务端拒）。

## 验证结论（modern/192 主题，Settings theme=modern）

| 需求 | 结果 | 证据截图 |
|---|---|---|
| ④ 主功能菜单位置 | ✅ 血条右侧一排彩色钮（不再压血条/不再在上方） | _final_hud.png |
| ④ 快捷键布局 | ✅ 菜单右侧，默认窄屏8槽/宽屏20槽(2×10)，展开28槽 | _final_hud.png |
| ④ 快捷键图标 | ✅ 落入格内（键帽上条+图标+数量），不再整片空灰格/压键帽 | _final_hud.png |
| ④ 聊天分组页签 | ✅ 中文6页签 全体/战斗/组队/好友/公会/联合，选中高亮，点击切换+消息按组过滤 | _game9.png |
| ⑤ 主功能菜单子功能 | ✅ 27 子项全有真 OpenUI/发包（menu-qs 取证，无空壳） | evidence/menu-qs-interaction |
| ⑥ 策略模式 | ✅ UiThemeLayout 三策略，theme=modern 生效 | 本轮 theme 切换验证 |
| ① 小地图 | ✅ 视口+标记裁剪（早前验证） | six-issues-rca |
| ② NPC | ✅ 撤销 FIXATED（早前验证） | npc-disappear-rca |
| ③ 竖栏 | ✅ 无灰底/折叠（_game9 左侧裸图标） | _game9.png |

## 本轮关键修复（行号）
- 菜单锚点改血条右缘：`UIStatusBar.cpp` menu_strip = hpmp_pos+(STATUS_BAR_W+4, 居中)。
- 1280 档 QS 起点右移 926 让开菜单；fold 钮同步；槽数 ≤1280=8 否则20。
- toggle_qs 参数遮蔽成员致恒真 return 的 bug 修复（参数改名 enabled），展开/收缩真正改槽数。
- 图标偏移：`draw(cell_pos + origin)`（IDA nDstTop=y-origin.y+32 语义）。
- 聊天页签：UIChatBar 新增 BtFilter0..5 + 中文 Text+ColorBox 自绘 + active_message_group 过滤 + group_of_channel 映射。
- 天气 WeatherBackLayers.cpp 补 MapleStory.h(USE_NX) 修复编译（非本需求但挡编译）。

## 残留（如实）
- 中文乱码：入站 read_string→packet_to_internal 已做 GBK→UTF8，真机"对所有人"等中文正常；无确凿乱码复现，未臆造修复。若用户还见乱码需具体消息样本。
- 图标与键帽顶条仍有轻微重叠（原生键帽即格顶小条，图标主体在格内），属观感微调，非功能缺陷。

## 部署
live `E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA `5d5ba02d39810c34610b417b239c3eb4bd8a3f8c8208bc90f7d9e0d22c1e7e2b`（本轮最终），BUILD_EXITCODE=0。Settings theme 已设 modern 供用户直接看 192 布局。
