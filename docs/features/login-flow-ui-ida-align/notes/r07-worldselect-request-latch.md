# R07 · 选世界/频道请求门闩

> 日期：2026-10-21  
> 依据：`BeiDou.exe` IDA，`CUIWorldSelect::OnButtonClicked @0x624AE8`、`CUIWorldSelect::IsRequestValid @0x62540A`

## 1. 参考端证据

`sub_624AE8` 在进入世界按钮分支前调用 `sub_62540A`：

```
*(CLogin + 368) == 0 && *(CLogin + 360) == 1
```

- step 必须为 1（选世界）；
- `CLogin+368` 为登录请求门闩，世界请求发出后置位，回包后清零。

因此参考端在世界列表请求进行中不会重复处理世界/频道按钮。

## 2. 当前端问题

`UIWorldSelect::enter_world()` 会创建 `UILoginWait` 并发送 `CharlistRequestPacket`。  
频道按钮“再次点击进入”和 `BtGoWorld` 两条路径都可能连续触发：

- 第一次创建 Wait 并发包；
- 第二次再次 `emplace<UILoginWait>`，可能重置 Wait 并重复发包。

参考端没有对应的重复请求保护。

## 3. 本次实现

`IO/UITypes/UIWorldSelect.cpp`：

- `button_pressed()` 入口检测 `UILoginWait`，存在时直接返回，不处理任何按钮。
- `enter_world()` 同样在 `channelid == 0xFF` 或 `UILoginWait` 已存在时返回。
- 角色列表到达后 `remove_selected()` 会把世界选择置为非激活状态，后续返回时重新允许操作。

## 4. 说明

- 当前端以 `UILoginWait` 作为“请求进行中”的等价标记，而不是移植 `CLogin+368` 字段；
  原因是 Wait UI 的生命周期已覆盖“发包 → 角色列表/错误弹窗”这一区间。
- 若服务器长时间不回包，Wait 会一直存在；这与参考端门闩未复位的表现一致，后续可加超时。
