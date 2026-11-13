# D5–D10 仲裁记录（修复阶段，2026-11-11）

仲裁方法：orange-wz MCP（128B key）取 IMG 位图 → PIL 全窗/期望位 SAD 模板匹配 →
正确客户端截图。SAD≈0 = 像素级坐实；SAD 高且无低落点 = 原生帧根本没画这个件。

| 缺陷 | 仲裁数据 | 结论 | 处置 |
| --- | --- | --- | --- |
| **D5** KeyConfig/BtHelp 未绑 | BtHelp/normal/0 = 12x12 橙「?」（read_image 亲验 `d5_bthelp_tmpl6x.png`）；全窗匹配最优 SAD=175.77 @(150,64)（键帽区误配）；(606,5) 处 BtClose SAD=0.00（`d569_match.json`） | **原生 v83 键盘设定窗根本不绘制 BtHelp**（IMG 有素材、客户端不建；与 IDA sub_832834 析构只释放 6 个 CCtrlButton 互证） | **不绑 = 正确原生行为**，非缺陷。留证不造功能 |
| **D6** Stat BtDetail (153,324) vs IDA (124,324) | 全窗匹配 BtDetail/normal/0 (46x19) 最优 frame=(154,326) SAD=0.62；本地代码坐标 (153,324) | 本地与原生帧差 1~2px，**像素级坐实**；IDA a4:124 是 CreateCtrl 锚点参数语义，与 top-left 记录口径不同（同 CUIStat::OnCreate 只建这一颗钮） | 坐标不改；注释口径从「截图目测」升级为「SAD 实测」 |
| **D7** Stat BtAuto (106,198) 推断 | BtAuto/normal/0 (100x35) 全窗最优 SAD=147.56 @(266,115)（误配区）——**角色能力.png 无任何可信落点** | 原生 v83 能力窗不绘制 BtAuto（IMG 有 BtAuto/BtAuto1/BtAuto2 素材，客户端不建；与 D5 同型） | 本地绑定保留（新手态熄灭 + 转职后 set_active 门控，历史设计），注释补「原生帧无此件」证据；漂移风险降级 |
| **D8** 学院窗 224 vs 227 | orange-wz 实测 Family/backgrnd = **224x392 origin(0,0)**；截帧 227 含 3px 边 | 窗宽 224 是 IMG 真身 | **非缺陷**，撤销登记 |
| **D9** KeyConfig 5 钮 y=234 无背书 | 五颗钮在**期望坐标**逐颗 SAD：BtOK(8,234)=0.22、BtCancel(58,234)=11.58、BtDefault(112,234)=0.20、BtDelete(177,234)=0.20、BtQuickSlot(260,234)=0.00（`d569_match.json`，cov=全点） | 一行坐标全部像素级坐实（BtCancel 11.58 = 该钮常态与截帧瞬时态色差，位置正确）；「截图目测」升级为「SAD 实测」 | 坐标不改；注释升级口径 |
| **D10** MiniMap BT_SMALL/BT_BIG | orange-wz 实测 MiniMap kids = ['BtMap','MaxMap','Min','MinMap','title']，**无 BtSmall/BtBig 素材** | 显式预留属实（与 ui-v83-window-restore/README.md:101 一致）；造滚动逻辑=无素材编排版 | TODO 注释保留 + 补本证据引用 |

## 修复阶段实际代码改动清单（截至本文件）

| 缺陷 | 文件 | 性质 |
| --- | --- | --- |
| D1 | IO/UITypes/UIMedal.h/.cpp（新建）、IO/KeyAction.h（MEDAL=28）、IO/UIElement.h（Type::MEDAL）、Net/KeyMapTranslate.h（双射）、IO/UIStateGame.cpp（include+case）、MapleStory.vcxproj/.filters | 勋章窗整体落地（门禁 18 无素材复用件路径） |
| D2 | IO/UITypes/UIMonsterBook.h/.cpp 重写、locale/{zh-CN,en-US}.ini（4 新键） | 书本双页 15 节点全绑；坐标三方对齐（IDA 立即数 + IMG 尺寸 + 截帧 SAD） |
| D3 | IO/UIElement.h/.cpp（fade_in + draw_sprites/draw_buttons 推进 + makeactive/ctor 归零）、UIMedal.cpp/UIMonsterBook.cpp（自绘件乘 fade） | 窗口开合淡入（音效层本地已有，补透明度层；淡出不做，理由注释内） |
| D4 | IO/UITypes/UIMaker.h/.cpp（BtStart@(82,356)/BtCancel@(163,356) 绑 + button_pressed 接 MakerCreatePacket；ComboBox/Gauge/BtDown 显式预留） | 底部两钮按 IDA 立即数落地 |
| D5–D10 | 无行为改动 | 仲裁为「非缺陷/已坐实/显式预留」，证据本文件 + d569_match.json |

build：tools/build_smoke.bat EXITCODE=0（UIElement/UIMaker/UIMedal/UIMonsterBook 四 TU 重编，build_log.txt:149/1456/1504/1674；exe 18:24 产出）。0 commit / 0 push。
