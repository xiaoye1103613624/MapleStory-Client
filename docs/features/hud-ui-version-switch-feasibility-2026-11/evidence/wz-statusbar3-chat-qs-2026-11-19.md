# orange-wz 取证：StatusBar3.img 聊天/快捷键/按钮节点结构（2026-11-19）

## 取证条件

- **工具**：orange-wz MCP `127.0.0.1:10012`（`tools/_mcp_call.py` 同协议脚本），端点 `http://127.0.0.1:10012/mcp`，29 工具。
- **文件**：live `E:\MXD\BeiDou-Client-xy\Data\UI\StatusBar3.img`（1,866,425 B，GMS 密文）。
- **key**：`userKeyBase64` = `tools/_gms_key_128.b64`（128B `WzAESConstant.DEFAULT_KEY`，B64_LEN=172、HEAD=`EwAAAFIAAAAqAAAAWwAAAAgA`）；`ivBase64=TSPHKw==`（GMS 4B）；`exclusive:false`。
- **收口**：`load_files → list_children/get_node_tree_json/get_node_detail/find_node → unload_all`（`{"ok":true,"rootCount":0}`）。
- **脚本**：`tools/_hud_probe/{wz_full,wz_deep,wz_origin,wz_tab,wz_target}.py`，原始输出同目录 `*.out`。
- **编码**：MCP 错误文本 GBK，UTF-8 解码呈乱码，节点名以 ASCII 部分为准。

## ⚠️ 关键翻案：给定证据 1「StatusBar3 无 chat/ingame」不成立

`find_node("mainBar")` 命中，`list_children(root=".../StatusBar3.img", nodePath="chat/ingame")` 返回：

```json
{"children":[{"name":"view","type":"LIST_PROPERTY"},
             {"name":"input","type":"LIST_PROPERTY"},
             {"name":"filter","type":"LIST_PROPERTY"}]}
```

`chat/ingame` **真实存在且结构完整**。`status_bar_root()` 在 modern 下返回 `StatusBar3.img`（`Util/UiAssetResolve.cpp:80-94`，经 `ModernStatusBar3Layout::status_bar_root()` `Util/UiThemeLayout.cpp:78`）。所以 `UIChatBar.cpp:324` 的 `chat/ingame` **能取到节点**。

**真因不是"节点整体缺失"，而是"modern 分支子节点名与 StatusBar3 实际命名不匹配 + 目标标签从未创建"**：

| 代码节点名（UIChatBar.cpp） | StatusBar3 实际节点 | 结论 |
| --- | --- | --- |
| `input["button:chat"]` | `chat/ingame/input/button:chat` ✅ 存在 | OK（但 normal/0 是 1×1，贴图在 normal 其它帧/UOL） |
| `input["button:itemLink"]`（:390） | 无；实际 `input/button:Link` | **错名 → 按钮空**，道具链接不可用 |
| `input["button:chatEmoticon"]`（:391） | 无；emoji 实际在 `chat/chatCommandMenu/button:chatEmoticon` | **错名 → 按钮空** |
| `input["button:help"]` | `input/button:help` ✅ | OK |
| `input["button:outChat"]` | `input/button:outChat` ✅ | OK |
| `input["layer:backgrnd"]` | ✅ 574×28 | OK |
| `input["layer:chatEnter"]` | ✅ 387×20 | OK |
| `view["btMax"]/["btMin"]/["drag"]` | ✅ 存在 | OK（13×13 / 18×10 / 349×6） |
| 聊天目标标签 | 代码 modern 分支**完全没建**；实际在 `chat/common/chatTarget/{base,all,friend,party,guild,association,afctv,...}` | **modern 目标切换缺失**（classic 分支有） |

## 节点路径与尺寸（带 origin 说明）

### chat/ingame/view（历史框，展开/收起）

| 节点 | 尺寸 (W×H) | 说明 |
| --- | --- | --- |
| `chat/ingame/view/max/top` | 574×15 | 展开态顶 |
| `chat/ingame/view/max/center` | 574×1 | 展开态中（纵向拉伸） |
| `chat/ingame/view/max/bottom` | 574×15 | 展开态底 |
| `chat/ingame/view/min/top` | 409×15 | 收起态顶 |
| `chat/ingame/view/min/center` | 409×1 | 收起态中 |
| `chat/ingame/view/min/bottom` | 409×15 | 收起态底 |
| `chat/ingame/view/drag` | 349×6 | 拖动把手 |
| `chat/ingame/view/btMax/normal/0` | 13×13 | 展开钮 |
| `chat/ingame/view/btMin/normal/0` | 18×10 | 收起钮 |
| `chat/ingame/view/tab/normal/0` | 52×18 | 标签页底 |
| `chat/ingame/view/btAddTab/normal/0` | — | 添加标签 |

### chat/ingame/input（输入行）

| 节点 | 尺寸 | 说明 |
| --- | --- | --- |
| `chat/ingame/input/layer:backgrnd` | 574×28 | 输入行底板 |
| `chat/ingame/input/layer:chatEnter` | 387×20 | 输入框内衬 |
| `chat/ingame/input/button:chat/normal/0` | 1×1 | 空占位（贴图在别帧） |
| `chat/ingame/input/button:Link/normal/0` | 1×1 | 道具链接（错名重定位目标） |
| `chat/ingame/input/button:chatCommand/normal/0` | 18×18 | 聊天命令 |
| `chat/ingame/input/button:fontSize/normal/0` | 1×1 | 字号 |
| `chat/ingame/input/button:help/normal/0` | 1×1 | 帮助 |
| `chat/ingame/input/button:outChat/normal/0` | 18×18 | 外部聊天 |

### chat/common（聊天目标标签，modern 分支当前未使用）

`chat/common/chatTarget/{base,all,friend,party,guild,association,afctv}` 各含 `normal/pressed/mouseOver/disabled` 四态（与 IDA StatusBar2 `chat/Tap/*` 9 标签语义一致，本版为 7 标签）。`base` 是底框，`all/friend/...` 是分页钮。
`chat/common/Tap_outside`、`BtChat_outside`、`BtChatEmoticon_outside`、`BtFontSize_outside`、`BtLink_outside` 为外部聊天变体按钮。
`chat/chatCommandMenu/button:chatEmoticon`、`button:chatCommand` 为表情/命令按钮正确归属。

### mainBar 按钮与血条 / 快捷栏

| 节点 | 尺寸 | 说明 |
| --- | --- | --- |
| `mainBar/menu/button:CashShop/normal/0` | 34×37 | 商城 |
| `mainBar/menu/button:Event/normal/0` | 32×36 | 活动 |
| `mainBar/menu/button:Character/normal/0` | 34×37 | 角色 |
| `mainBar/menu/button:Community/normal/0` | 34×37 | 好友 |
| `mainBar/menu/button:Setting/normal/0` | 34×37 | 设置 |
| `mainBar/menu/button:Menu/normal/0` | 34×37 | 菜单 |
| `mainBar/status/backgrnd` | 198×37 | 血条背景 |
| `mainBar/status/layer:cover` | 204×70 | 血条覆盖层 |
| `mainBar/status/layer:Lv` | 26×14 | 等级牌 |
| `mainBar/quickSlot/backgrnd` | 557×67 | 快捷栏底板（377+180，28 槽需要完整宽度） |
| `mainBar/quickSlot/layer:cover` | 562×73 | 快捷栏遮罩 |
| `KeyConfig/backgrnd` | 660×537 | 键位设置底板 |

## 与布局重叠 / 对齐的关联结论

1. **血条重叠**：`hpmp_pos=(416+pos_adj,40)`，`status/layer:cover` 高 70 → 血条覆盖 Y∈[40,110]；menu 六钮 `menu_strip.y = quickslot_pos.y()+36`，1024 档 `quickslot_pos.y=37` → 钮顶 y=73，钮高 37 → Y∈[73,110]，X 从 `quickslot_pos.x - 35*6 - 8`，1024 档 x=628-218=410 → X∈[410,620]。血条 X∈[416,620]、Y∈[40,110]，**两者 X/Y 全区间重叠** → 用户报"主功能按钮压血条"。修法：menu 六钮移到血条上/下方空闲带（背景 `status/backgrnd` 仅 198 宽，血条实际只占 x 416~620 的左侧；把钮整体右移到血条右缘之后或改到血条上方安全 y）。
2. **快捷键槽数/宽度**：`backgrnd` 557px 宽；28 槽 `s_ptShortKeyPos` 最大 x=447 + 32 宽 = 479 < 557，槽位全在背景内。当前 `modern_qs_count=28`（1024+）与背景宽度匹配；8 槽模式裁到 148px 也正确。**但** `layer:cover` 562×73 与 `backgrnd` 557×67 尺寸不同，两者 `shift(origin)` 后若 origin 不同会错位 → 已按前人证据以同一个 `qs_draw` 绘制，需真机复核键帽落点。
3. **聊天目标标签**：modern 分支 `UIChatBar` 构造中未创建任何目标分页按钮/底框，切到 modern 后没有"全部/好友/组队..."切换入口——必须补建（用 `chat/common/chatTarget/*`）。

## 补充取证：输入按钮 1×1 占位 → 真实纹理回退（tools/_hud_probe/wz_btnsize.py）

输入行的 5 个按钮在 `chat/ingame/input/button:*` 下 `normal/0` 多为 **1×1 占位**（WZ 用 UOL/其它帧承载真实图，本端 nx 解析器读成空帧）。`UIChatBar.cpp:387` 用 `Texture(input["button:chat"]...).width()` 算按钮间距 `input_btns_x = 1+3 = 4`，导致 5 个按钮以 4px 间距**全部叠在一起** —— 这才是"聊天功能不完善"的直接观感来源。所有占位按钮在 `chat/common/Bt*_outside/normal/0` 都有 **18×18 真实纹理**，可作可见回退：

| 占位节点（1×1） | 回退节点 | 尺寸 |
| --- | --- | --- |
| `input/button:chat` | `chat/common/BtChat_outside` | 18×18 |
| `input/button:Link`（代码错名 itemLink） | `chat/common/BtLink_outside` | 18×18 |
| `input/button:fontSize` | `chat/common/BtFontSize_outside` | 18×18 |
| `input/button:help` | `chat/common/BtHelp_outside` | 18×18 |
| `input/button:outChat`（本身已有图） | 保持 | 18×18 |
| `chat/chatCommandMenu/button:chatEmoticon`（代码错名，且原节点 47×12） | `chat/common/BtChatEmoticon_outside` | 18×18 |

**约束**：`toggle_input()`/`update_view()` 里 modern 分支对 `buttons[BtChat/...]` 无空指针检查（`UIChatBar.cpp:2493-2497/2511-2515`），注释明确 operator[] 插空指针会 `0xc0000005`。因此修法必须**保留全部按钮对象**，只把贴图源切到真实纹理节点，不能"跳过创建 1×1 按钮"。

## 未确证 / 局限

- 部分 canvas 的 `origin` 属性 `get_node_detail` 返回 None（WZ 中未显式写 origin，运行时默认 (0,0)）；按钮/贴图精确像素落点需真机抓帧复核。
- `chat/ingame/input/button:chat` 等 1×1 normal/0 是占位帧，真实贴图可能在 UOL 指向的其它帧；本证据不臆测其可见态，改法需给可见兜底。
- 本取证只确认节点结构与尺寸，**未做真机渲染验证**；所有布局修复结论标注"真机待验"。
