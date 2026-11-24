# 192 原生布局二次校准（2026-11-19 16:xx，用户四条反馈）

## 用户四条反馈与本轮修复
1. 聊天框距底/分栏样式非192原生 → 根因 modern `position` 从未赋值(恒0,0)，`update_screen` modern 直接 return。修：构造尾部锚定 position=(2, vh-15-input_bg_y-2) 贴底；分栏页签改用 192 原生 `chat/common/chatTarget/{all,afctv,party,friend,guild,association}` 四态位图（顺序按 MessageGroup），烤字与语义不符处用同色内框盖住再叠中文 全体/战斗/组队/好友/公会/联合。
2. 菜单应与血条同高、都在经验条上 → menu_strip.y 从 hpmp_pos.y 起（顶对齐），x=血条右缘+4。
3. 快捷键长条（非小键盘）、从菜单到右缘、图标偏移、切换UI → 实测 live StatusBar3 `quickSlot/backgrnd` 557×67：列分隔 x=3,38…528 → **pitch=35**；行 y0-31/y34-65 → pitch=34；16列×2行。旧表 (7,15)/33 是 IDA StatusBar2 契约，错位根因。改 `MODERN_QS_POS[32]` 为 (0,0)(35,0)…(525,34)；MODERN_QS_SLOTS 28→32；默认16槽(2行8列)/展开32；QS 起点=血条右缘+4+6*35=菜单之后；图标 draw(center+origin) 居中；Fold/Extend(切换UI)贴 QS 左缘。
4. 血条上方名字背景居中 → namelabel 改 CENTER 对齐，x=hpmp_pos.x()+99（198宽背景中心）。

## 实测证据
- orange-wz 导出 `mainBar/quickSlot/backgrnd` PNG（tools/_qs_backgrnd.png），numpy 量出列分隔 pitch=35、行 pitch=34。
- 原生页签位图导出 tools/_tabs_native.png，烤字=全体/好友/组队/家族/联盟/시청자（与图2语义顺序不同，故覆盖+语义字）。

## 验证（自建单实例 + ImageGrab 置前截图）
- BUILD_EXITCODE=0；live SHA `811e0b783f63ba6a9545bf9812bbd41ec711e8174a65b8453ad649a806c4771d`（实时核对）。
- 截图 tools/_v6_hud.png：血条→菜单6钮(同高)→长条快捷栏(2行8列,图标在格内,键帽顶条)依次排开，无重叠。
- 截图 tools/_v6_chat.png：聊天贴底、6中文页签、对所有人目标条。

## 残留/待用户确认
- 名字"Admin"显示在等级350右侧同一行（x=背景中心、CENTER对齐）——与图2"等级200 xxx天堂2"同排观感一致；若用户所指"背景居中"是别的含义需具体说明。
- 展开态32槽未逐格真机截图（默认16槽已验）；切换UI 钮点击行为已修 toggle 恒真 bug 但真机点击未验证。
- 未提交 git（用户未要求）。
