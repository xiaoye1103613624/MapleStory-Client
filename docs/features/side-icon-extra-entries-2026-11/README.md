# 魔女森林 / 突破石 —— 侧栏入口接入指引（2026-11-21）

## 现状（点到面已查证）
两功能**全链路已实现，只缺入口**：
- UI 类：`IO/UITypes/UIWitchForest.cpp/.h`（192 魔女森林抽奖，UIWindowCN.img/CN_50100）、`UIBreakthroughStone.cpp/.h`（CN_Limitbreakthrough）
- 协议：魔女 C 0x3719/S 0x371A；突破石 C 0x371B/S 0x371C（OutPacket.h:316-318，PacketSwitch 已注册收包）
- 服务端：s9 `RecvOpcode:214/215`、`SendOpcode:379/380`、`PacketProcessor:147/148` 注册 handler，构造即发 OPEN 包（UIWitchForest.cpp:127 / UIBreakthroughStone.cpp:73）
- 侧栏分支：`UISideIconBar.cpp:440-442`（key=witchforest/forest/witch → emplace UIWitchForest）、`:445-447`（key=breakthroughstone/breakthrough/stone → emplace UIBreakthroughStone）；`open_ui(arg)` 把 arg 转小写匹配（:392-456）
- 聊天命令：`UIChatBar` 无对应 verb（可选入口，用户未选）

## 缺的三样
1. `side_icons.ini` 无这两功能槽位（现只有 slot.0~5：book/bulb/star/checkin/damagerank/colorprism）
2. `E:\MXD\BeiDou-Client-xy\UI\SideIcons\` 无 witchforest/breakthroughstone 图标素材（现有 6 组约 36-39px RGBA）
3. （可选）聊天命令入口 —— 用户本轮未选，不做

## 用户拍板的接入方式
侧栏图标按钮，图标由用户提供。

## 图标规格（从现有 6 组反推，请按此提供）
- 文件放 `E:\MXD\BeiDou-Client-xy\UI\SideIcons\`
- 命名（4 个文件，_over 为悬停态）：
  - `witchforest.png` + `witchforest_over.png`
  - `breakthroughstone.png` + `breakthroughstone_over.png`
- 尺寸：约 **37×39 px**（现有 colorprism 37×39 / checkin 38×40 / damagerank 36×36），RGBA 带透明
- 风格：与现有 6 组图标一致（左侧竖栏小图标）

## 图标到位后的接入步骤（秒接，无需重编 exe）
1. 复制 4 个 png 到 SideIcons 目录
2. `side_icons.ini` 追加（slot.6/7）：
```
slot.6.id=witchforest
slot.6.enabled=1
slot.6.icon=local:witchforest
slot.6.action=OpenUI
slot.6.arg=witchforest
slot.7.id=breakthroughstone
slot.7.enabled=1
slot.7.icon=local:breakthroughstone
slot.7.action=OpenUI
slot.7.arg=breakthroughstone
```
3. `UISideIconBar` 每 ~5s 热重载 ini（:297），无需重启即可看到新图标
4. 真机验收：点两个图标应分别弹出魔女森林/突破石窗（服务端已注册 handler，真机发包待验）

## 注意
- 若用户提供的图标名不同，arg 必须保持 `witchforest` / `breakthroughstone`（与 :440/:445 的 key 匹配），只有 `icon=local:xxx` 里的 xxx 随文件名变。
- 服务端 handler 已就绪但真机发包未验收；如真机点开无响应，需抓包核 opcode（0x3719/0x371B）。
- 本轮未改任何代码/资源，等用户提供图标素材。
