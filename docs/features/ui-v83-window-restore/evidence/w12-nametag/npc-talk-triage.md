# NPC 对话「乱码」只读归因（W12 附带，不改代码）

> 结论先行：**不是 (a) GBK↔UTF-8 解码，也不是 (b) T12b 度量残留，而是 (c) 版式/文本处理层 —— 具体是 NPC 对话 markup 的解析与逐字节打字机。**
> 归属：**T13（文本/IME 线）**，不是本 UI 资产线。落点文件 `IO/UITypes/UINpcTalk.cpp` 不在 W12 授权范围内，本批**一行未动**。

## 1. 排除 (a)：解码链路是通的

| 环节 | 证据 |
| --- | --- |
| 线上包体 → 内部表示 | 唯一入口 `InPacket::read_string` → `read_padded_string` → **`packet_to_internal(raw)`**：[Net/InPacket.cpp:81-86](../../../../Net/InPacket.cpp#L81)、[:105](../../../../Net/InPacket.cpp#L105)；实现 CP936→UTF-8：[Net/PacketCharset.cpp:18](../../../../Net/PacketCharset.cpp#L18)、[:43-51](../../../../Net/PacketCharset.cpp#L43)（T11 已定盘，头注释明确「转换只允许发生在 InPacket::read_string / OutPacket::write_string 这一层」） |
| NPC 对话正文入口 | `std::string text = recv.read_string();` → `npctalk->change_text(...)`：[Net/Handlers/NpcInteractionHandlers.cpp:46](../../../../Net/Handlers/NpcInteractionHandlers.cpp#L46)、[:66-67](../../../../Net/Handlers/NpcInteractionHandlers.cpp#L66) ⇒ 走的就是上面那条已转码的路 |
| 资源侧文本 | 取样节点 **`String.nx/Npc.img/1012100`**（只读取证 `python tools/nx_inspect.py list .../String.nx 2 "Npc.img/1012100"`）：`name = 赫丽娜`、`func = 弓箭手转职官`、`d0/n0 = 想成为弓箭手的人来找我。。。 / 你想成为弓箭手吗?` —— **合法 UTF-8，无 mojibake**。客户端按原始字节取用：[Gameplay/MapleMap/Npc.cpp:62](../../../../Gameplay/MapleMap/Npc.cpp#L62)、[:65-66](../../../../Gameplay/MapleMap/Npc.cpp#L65) |

⇒ 两条来源（服务器包 / String.nx 资产）到渲染前都已经是 UTF-8，解码层没有可归因的缺口。

## 2. 排除 (b)：度量层不产生「乱码」

度量已按码点走：`LayoutBuilder::measure` → `utf8_next` → `wide_advance`（[Graphics/GraphicsGL.cpp:1196-1234](../../../../Graphics/GraphicsGL.cpp#L1196)、[:1224-1234](../../../../Graphics/GraphicsGL.cpp#L1224)），绘制路径与布局同源（[:1359](../../../../Graphics/GraphicsGL.cpp#L1359)）。度量类 bug 的表现是**错位/重叠/换行点不对**，不会把字变成看不懂的符号。且下面 §3.2 的缺陷发生在「字符串还没按码点切分」之前，度量无从背锅。

## 3. 命中 (c)：两处文本处理缺陷（可复算，不依赖抓帧）

### 3.1 `format_text` 把闭合 `#` 留在正文里，且每种标记只替换一次

[IO/UITypes/UINpcTalk.cpp:467-519](../../../../IO/UITypes/UINpcTalk.cpp#L467)：

```cpp
size_t begin = formatted_text.find("#p");            // 只 find 一次，没有 while 循环
size_t end   = formatted_text.find("#", begin + 1);  // 闭合 #
formatted_text.replace(begin, end - begin, namestr); // 覆盖 [begin, end) —— 闭合 # 没被吃掉
```

- `#p1012100#` → `赫丽娜#` ：**留下一个游离 `#`**。
- 这个 `#` 进了渲染层的 markup 状态机：`case '#'` 看**后一个字符**决定语义（[Graphics/GraphicsGL.cpp:915-1123](../../../../Graphics/GraphicsGL.cpp#L915)）。
  - 后面正好是已登记字母（`b/r/k/l/L/i/h/…`，见 :922-1113）→ **下一个真标记被当成这个游离 `#` 的参数吃掉**（`skip += 2`），表现为颜色/加粗/换行错位；
  - 后面是中文（首字节 0xE4-0xE9）→ `default:` 打 `Unknown format` 且 **`skip` 不加**（[:1119-1122](../../../../Graphics/GraphicsGL.cpp#L1119)）→ 屏幕上留下一个**字面 `#`**。
- `#p/#h/#t` 各自只替换**首次**出现 ⇒ 第二次及以后的 `#p<id>#` 落到渲染层的 `case 'p': skip += 2`（[:1067-1071](../../../../Graphics/GraphicsGL.cpp#L1067)，注释「TODO: Needs implemented」）⇒ **`#p` 消失、`1012100#` 原样画出来**。用户看到的正是「汉字里夹一串数字和 #」这种「乱码」。

### 3.2 打字机按**字节**推进，UTF-8 汉字被 1/3 个地喂给排版

[IO/UITypes/UINpcTalk.cpp:157-165](../../../../IO/UITypes/UINpcTalk.cpp#L157)：

```cpp
char c = formatted_text[formatted_text_pos];   // 一个字节
text.change_text(t + c);
formatted_text_pos++;                          // 每 ~5 tick 一个字节
```

- 一个汉字 3 字节 ⇒ 前 2 帧字符串尾部是**残缺 UTF-8 序列**；`utf8_next` 对越界尾序列返回 `0`（[Graphics/Utf8.h:48-52](../../../../Graphics/Utf8.h#L48)）→ `measure` 记 0 宽、绘制什么都不画 → 逐字「蹦」出来，中间态是半截空格。
- 同理，`#b`/`#L0#` 这类标记在只喂到 `#` 的那一帧会走 `case '#'` 的 `default` → 闪现字面 `#` + 往 `_ui_asset_diag`/控制台刷 `Unknown format`。
- 附带成本：每帧 `change_text` 触发一次全量重排（长中文对话明显）。

## 4. 建议的归属与修法（供主线/T13 决策，本批未实施）

1. **归 T13**：修法全在「对话文本的切分/解析」层（`UINpcTalk::format_text` + `UINpcTalk::update` 的打字机 + `LayoutBuilder` 的 `#` 状态机 default 分支），与 UI 资产代次无关；W12 只授权 `NameTag.{h,cpp}`，故不动。
2. 最小改动面（三处，都不碰协议）：
   - `replace(begin, end - begin + 1, namestr)` 覆盖到闭合 `#`，并把 `#p/#h/#t` 改成 `while` 循环全量替换；
   - 打字机改为按码点前进：`size_t used = 0; uint32_t cp = utf8_next(formatted_text.c_str(), formatted_text_pos, formatted_text.size(), used);` 一次追加 `used` 字节；
   - `LayoutBuilder::add` 的 `case '#': default:` 也 `skip += 2`（或至少 `skip += 1`），让未知标记不再把字面 `#` 画到屏幕上。
3. 若线上复验发现「汉字变方块」而非「夹数字/#」，再回查 `wide_advance` 的字形覆盖（`font.wide` 图集是否被 `Graphics/Utf8.h` 之外的路径绕过）—— 目前证据不支持这条。
