# SoftKey 键位几何审计与还原（2026-11-06，接续 0e415681 子卡1）

> 本文是**终版**。初稿（Login.img/PIC 竖版 3 页签框架、"逐槽坐标拿不到、待用户决策 A/B/C"）已被
> 后续证据**推翻并替换**，推翻链与保留的取证记录见文末「证据演进」。以本文的 UIWindow.img/SoftKeyboard
> 结论为准，不要引用初稿的 A/B/C 抉择。

## 结论（决定性更正）

**UI 错位实锤 + 根因**：旧 `IO/UITypes/UISoftKey.cpp:538-548` 的
`keypos(index,row){ x=index*45; y=row*43; if(row==3)x+=90; return Point(27+x,95+y); }`
是 Journey 通用 12 列宽键盘口径（x 最大 `27+11*45=522px`），却把键帽摆到
`Login.img/Common/SoftKey` 的 **140px 宽窄底板**上 → 键帽大面积漂出底板、盖住 Messenger
（实锤帧 `../softkey-tails/evidence/messenger-look/20261106_p7_admin_softkey.png`：
黑色竖版 140×280 底板 + `1..0/abc/def/...` 乱序小块散落全屏压住聊天窗）。

**V17.7 原生两套软键盘并存**（`docs/features/native-map-object-re/evidence/beidou_v177_decrypted_strings.tsv`）：

| 资产 | 用途 | 结构 |
| --- | --- | --- |
| `UI/Login.img/Common/SoftKey/*`（:4115-4126，14 处） | PIC 二级密码**竖版随机键盘** | backgrnd 140×280 / Tab 3 页签 / BtNum,BtHighCase,BtLowCase 0..9 40×35 origin(0,0) |
| `UI/UIWindow.img/SoftKeyboard/*`（:3013-3018 + UIWindow.img 全表 767 处） | **窗口式 37 键 QWERTY 虚拟键盘** | backgrnd/0,1 / funckey / key/0..36 / Bt/0,1；逐键 origin 自带网格 |

用户决策（m00825）：**按 `UIWindow.img/SoftKeyboard` 还原**＝撤销此前"屏蔽虚拟键盘"的临时决定
（m00725），弃用 Login.img 资产与 45px/12 列 keypos，保留 Mode/物理键盘/回调/check_pic。
这符合 UI 硬约束「按原生客户端逻辑布局还原」，直接消除错资产根因。

## 原生真值（UIWindow.img/SoftKeyboard，结构真值见 uiwindow-softkeyboard-structure.json）

来源 `E:\MXD\BeiDou-Client-xy\REA反编译\wz_dump\UIWindow_img.json` + 运行时
`Data/.nxcache/UI.nx`（`tools/nx_inspect.py` 逐节点实证，5 个关键节点全 OK）。

- `backgrnd/0`：back 290×119 origin(145,59)；`1` 114×20 (57,10)；keyboard 278×95 (139,47)
- `backgrnd/1`（本次采用，金色卷轴皮肤）：back **315×127** origin(157,63)；`1`(title) 147×34 (73,17)；keyboard 278×95 (139,47)
- `Bt/0/{BtClose,BtMin,BtMax}` 12×12 origin(0,0)；`Bt/1` 同 14×14
- `funckey`（四态 normal/mouseOver/pressed/disabled）：capslock 32×22 (18,12)、lshift 48×22、rshift 40×22、enter 33×22、backspace 21×45 (5,-7)
- `key/0..36` 共 **37 键**，每键 21×22、四态齐；`normal/0/origin` 即网格坐标（列距 23 / 行距 23）

### 37 键编号→字符（位图导出 `evidence/keys/keys_sheet.png` 视觉识读）

```
KEY_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-"   // k0..k36
```

### 逐键 window-local top-left（`evidence/keys/origins.json` → keycoords.txt）

共享锚点 `ANCHOR_WINDOW=(283,30)`，`T = ANCHOR - raw_origin`；k36 `'-'` 与 k35 `'Z'`
共用 origin(207,-76)，按原生实拍手工错位到数字行第 11 位 (255,37)。托盘 `TRAY_TL=(18,30)`。

| 行 | 键 @ x |
| --- | --- |
| y=37（数字） | `1@25 2@48 3@71 4@94 5@117 6@140 7@163 8@186 9@209 0@232 -@255` |
| y=60（QWERTY） | `Q@48 W@71 E@94 R@117 T@140 Y@163 U@186 I@209 O@232 P@255` |
| y=83（ASDF） | `A@60 S@83 D@106 F@129 G@152 H@175 J@198 K@221 L@244` |
| y=106（ZXCV） | `Z@76 X@99 C@122 V@145 B@168 N@191 M@214` |

funckey window-local：capslock(20,83) lshift(20,106) rshift(249,106) enter(263,83) backspace(274,36)；BtClose(299,2)。
像素范围 x25..295 / y37..127 **全部装进 315×127 底板**（`evidence/keys/preview_full.png` 逐像素对拍）。

## 现实现偏差清单 → 已修

| # | 偏差 | 旧值 | 修复 |
| --- | --- | --- | --- |
| 1 | 用错资产 | `Login.img/Common/SoftKey` 窄底板 | `UIWindow.img/SoftKeyboard` 315×127 宽键盘 |
| 2 | 键位网格 | `keypos` 12 列×45px，x≤522 漂出 | 逐键 `KEY_X/KEY_Y`（23px 网格）锁进底板 |
| 3 | 字符映射 | 随机四行 + Blank 槽 | `KEY_CHARS` 37 键静态 QWERTY（位图实证） |
| 4 | 大小写 | `highCase` 切两套随机键帽 | CapsLock 持久 / 左右 Shift 一次性，附点字母转小写 |
| 5 | 输入框/提示 | textfield 漂到 x≈419、tooltip x419+ | textfield `position+(10,130)` 200×18；tooltip `position+(10,150)` |
| 6 | 背景绘制 | 单 `sprites.emplace_back(backgrnd)` 吃 origin 飘走 | 三层 Sprite 各传 `DrawArgument(Texture(node).get_origin()[+(18,30)])` |
| 7 | 功能键 | 无 Caps/Enter/Shift/Backspace 还原 | 5 funckey + BtClose 全 try_maple_btn |

## 引擎绘制语义（为何传 top-left 恒等、对 sanitize_origin 免疫）

- `IO/Components/MapleButton.cpp:30-53` `try_maple_btn`：
  `origin=Texture(src["normal"]["0"]).get_origin()`（已 sanitize）→
  `buttons[id]=make_unique<MapleButton>(src, position+origin)`；
  `:76-83` draw=`textures[state].draw(position+parentpos)`；
  `:91-108` bounds=`parentpos+position-textures[state].get_origin()`。
  draw 与 bounds 减的是同一个 sanitized origin ⇒ **传入 `position`＝期望 top-left，净效果恒等**。
- `Graphics/Texture.cpp:31-45` `sanitize_origin`：键帽 21×22 → lim_x=106，
  origin.x>106（如 k01 (258,-7)）会被清零；但上条恒等性使实现不受影响。
- `Graphics/DrawArgument.h:122-148` `get_rectangle`：`rlt=pos-center-origin` ⇒
  `Texture::draw(DrawArgument(P))` 时 `left=P.x-tex_origin.x`；取 `P=origin+offset`
  位图画在 `offset`。故背景 Sprite 必须传 `DrawArgument(origin+offset)`（同
  `UIKeyConfig.cpp:85-95` classic 支），否则大 origin 位图整体飘走。
- `IO/Components/Textfield.cpp:338-384` `send_cursor` 用 `bounds.contains(cursorpos)` ⇒
  bounds 必须绝对坐标；`draw(position)` 用 `bounds.get_left_top()+position` ⇒
  本实现 bounds 已含 `position`，draw 传 `Point(0,0)`。

## IDA / 反编译取证记录（为何静态拿不到字符映射）

1. **BeiDou.exe 为 VMProtect 保护**：PE 节区异常（4 个重名 `.text`、伪 `.export`、入口 0x663FF3），
   `config.ini: UseVirtuProtect=true`；家族旁证 `...浩浩岛095(9.1测试端)\开启游戏.vmp.exe.i64`。
2. live IDA（ida-pro-mcp@127.0.0.1:13337，GUI PID 挂 BeiDou.exe.i64）strings=3889：
   无 `SoftKey/BtNum/BtHighCase/KeyWindow` 字面量；UI 路径串存在 ⇒ 运行时拼串或 VMP 段内引用。
3. 解密串 VA `0xb21e10/0xb21db0/0xb21d80/0xb21d54/0xb21d30/0xb21d20/0xb21d10`
   处 `get_bytes` 全是密文（VMP 运行时解密），`XrefsTo` 各=1（数据段自引用）、无代码函数
   ⇒ **静态反查字符映射死路，已判死不再重试**。教训：VMP 加密串 VA 处是密文，xrefs 不指向使用函数。
4. 083 系 `misc/Equip_PIC.c`（sub_5CAC3D，3373 行 PIC 流程）无显式像素网格算术。
5. **版本纪律**：BeiDou.exe.i64(V17.7/083系)=复刻目标；Maplestory95.i64(095系) 仅供 095 侧参考，
   UI 布局证据严禁跨系互引（用户 m00372）。

**替代取证路径（已闭环）**：VMP 阻断静态 → 转运行时资产：
`Data/.nxcache/UI.nx` 位图可达 → `tools/nx_inspect.py` 导 37 键 PNG →
视觉识读字符映射 + `origins.json` 逐键 origin → `_preview_layout.py` 三图逐像素对拍 →
用户附原生实拍（卷轴 SOFT KEYBOARD + `1..0 -` + QWERTY 三行 + Caps/Shift/Enter/Backspace）
与推导网格逐键吻合。

## 验证状态

| 层 | 结果 | 证据 |
| --- | --- | --- |
| 运行时资产可达 | ✅ | nx_inspect 5 节点全 OK（UI.nx） |
| 字符映射 + 网格 | ✅ | keys_sheet.png / origins.json / keycoords.txt / 预览三图 |
| 引擎绘制语义 | ✅ | MapleButton/Texture/DrawArgument/Textfield 源码逐行 |
| 构建门禁 | ✅ | `tools/build_smoke.bat` GATE OK `s9 9dcd23e331` + EXITCODE=0，新 exe 16:45 / 4878336 字节；UISoftKey.obj 已重编、零 error |
| **真机对拍（键帽不漂出底板/不盖 Messenger）** | ⏳ **待用户截图验证** | 用户 2026-11-06 17:19 决定「先别真机测试，直接下一步开发」，本轮真机驱动卡在登录页未进图（`evidence/live-20261106/v1_verify.txt`），未产生对拍帧；不视为通过也不视为失败 |

> 构建期间另修 3 处**并行会话未跟踪半成品**的客观编译笔误（非本功能、不进本功能提交）：
> `IO/UITypes/UISpecial.h:4-5` include 路径 `../Graphics/`→`../../Graphics/`；
> `Net/Packets/SpecialPackets.h:3` `"OutPacket.h"`→`"../OutPacket.h"`；
> `IO/UITypes/UISpecial.cpp` 补 Constants.h / nlnx/nx.hpp include、`backgrnd.draw(position)`→`(position, inter)`。

## 证据演进（初稿为何被推翻，保留以防回退）

- 初稿只查了 `Login.img/Common/SoftKey`，判「PIC 竖版 3 页签 + 逐槽坐标拿不到」，提 A/B/C 抉择。
- 用户 4 次追问（m00819~m00825）「UIWindow.img/SoftKeyboard 是你需要的软键盘嘛？能按资源还原吗？」
  → 解密串表证实 V17.7 **两套键盘并存**，且 UIWindow.img 这套自带逐键 origin（网格就在资产里），
  不需要 IDA 猜坐标 → 初稿的「证据不足」前提不成立。
- 中间曾误判「v83 原生未启用软键盘」（VMP 加密导致静态 0 命中），用户令屏蔽虚拟键盘（m00725），
  误写 `UISoftKey.h` 为 `UIInputDialog` 物理键盘弹层；发现完整资产后 `git checkout --` 回滚，
  用户改选按原生还原。**当前头文件/实现均为 UISoftKey 原类，无 UIInputDialog 残留。**
