# R06 · 选角/删除请求门闩

> 日期：2026-10-21  
> 依据：`BeiDou.exe` IDA，`CUICharSelect::OnButtonClicked @0x601667`、`CUICharSelect::IsRequestValid @0x601861`

## 1. 参考端证据

`IsRequestValid` 返回：

```
*(CLogin + 368) == 0 && *(CLogin + 360) == 2
```

- `CLogin+360`：当前 step，必须为 2（频道/角色列表）；
- `CLogin+368`：请求门闩，发包后置位、回包后清零。

`OnButtonClicked` 在门闩通过后才进入按钮分支，因此选角、新建、删除都不会在回调前重复触发。

## 2. 本次实现

`IO/UITypes/UICharSelect.{h,cpp}`：

- 新增 `bool request_pending`，`button_pressed()` 入口统一拦截。
- `SelectCharPacket` / `SelectCharPicPacket` 发出前置位。
- `DeleteCharPacket` / `DeleteCharPicPacket` 发出前置位。
- 新增 `clear_pending()`，由服务器回包复位：
  - `SelectCharacterHandler`：`reason != 0`（选角失败）时复位；
  - `DeleteCharResponseHandler`：读到回包后立即复位。

## 3. 行为变化

- 快速双击“开始游戏”或连续按回车，只会发出第一个选角包。
- 删除确认/PIC 弹窗未点确定前不置位，玩家仍可取消。
- 选角成功后由 `SetField` 销毁选角窗口，门闩无需手动复位。

## 4. 尚未处理

1. **超时**：若服务器不回包，门闩会一直保持；当前依赖断线/重连销毁 UI。
2. **成功回包**：`SELECT_CHAR` 成功路径没有显式复位，依赖 `SetField`；若服务端只回成功不回 `SetField`，需要补超时。
3. **世界选择**：参考端 `CUIWorldSelect` 同样有 `IsRequestValid`（step 1 + 门闩）；当前端尚未加。
