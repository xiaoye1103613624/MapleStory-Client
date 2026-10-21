# R05 · 选角入口索引与请求守卫

> 日期：2026-10-21  
> 依据：`BeiDou.exe` IDA，`CUICharSelect::OnButtonClicked @0x601667`、`CUICharSelect::IsRequestValid @0x601861`

## 1. 参考端行为

`CUICharSelect::OnButtonClicked` 在任何按钮分支前先调用 `IsRequestValid()`：

```
if (CUICharSelect::IsRequestValid(this) != 0)
{
    SetKeyFocus(-1);
    *(this + 29) = buttonId - 1000;   // 记录最后点击的按钮
    switch (buttonId)
    {
        case 1000: CLogin::SendSelectCharPacket(...); break;
        case 1001: CLogin::OnNewCharStep(...);        break;
        case 1002: CLogin::SendDeleteCharPacket(...); break;
    }
    CWndMan::SetFocus(this);
}
```

`IsRequestValid` 同时检查：

- `CLogin + 368 == 0`：没有尚未完成的登录请求；
- `CLogin + 360 == 2`：当前处于 step 2（频道/角色列表）。

因此参考端不会在无效状态或请求进行中重复发包。

## 2. 当前端问题

1. `UICharSelect` 构造时 `uint16_t world;` 未初始化。若直接进入选角（自动登录、或 `UIWorldSelect` 已销毁），
   `selectedWorld["icon"][world]` 会使用未定义索引，表现为世界图标/名称空白或越界访问。
   本项在构建时以 `C4701` 暴露。
2. `selected_character` 直接来自 `Setting<DefaultCharacter>`。该角色被删除、或服务器槽位数变化后，
   索引可能 `>= characters_count`，而 `BtSelect` / `BtDelete` 直接以它索引 `characters`。
3. `BtDelete` 没有检查是否真的有角色；在空列表/空槽上触发会越界。
4. 当前端没有“请求进行中”标记，快速双击/回车可能重复发送 `SelectCharPacket`。

## 3. 本次修复

`IO/UITypes/UICharSelect.cpp`：

- `world` 初始化为 `0`；若映射结果在 `selectedWorld["icon"]` 中不存在，回退到 `0`。
- 构造时先把 `selected_character` 夹到 `[0, characters_count)`，再计算 `selected_page`，
  并保证 `selected_page < page_count`。
- `BtSelect`：仅当 `selected_character < characters_count` 且 `< characters.size()` 时才保存/发包。
- `BtDelete`：同样的索引校验，不满足时直接返回。

## 4. 尚未对齐

1. **请求进行中防重**：参考端 `CLogin+368` 的门闩尚未移植。当前端仍可在服务器回包前重复点击；
   需要增加 `select_pending` 并在 `SelectCharacterHandler` 失败/`SetField` 时复位。
2. **频道翻页**：参考端 `CUIChannelSelect` 有 1000/1001 翻页按钮（60 个频道）；当前端固定 30 个频道按钮，
   超出部分不显示。
3. **View All / View Choice**：`UIWorldSelect` 中两个按钮仍为 stub，点击无行为。
4. **选角窗口结构**：参考端是 `CUICharSelect` 101x131 小窗 + 多个对话框拼合；
   当前端是单个 800x600 `UIElement`。本次只修索引安全，不改变版式。

## 5. 验证

- 构建：MSBuild Release / x64 / v142，`EXITCODE=0`，无 `C4701`。
- 实机：删除默认角色后重进选角、或把 `DefaultCharacter` 改成越界值，确认不再空白/崩溃。
