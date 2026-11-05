# IDA · CUIMessenger::SetCtrl 关闭钮

- IDB：`E:\MXD\Maplestory95.i64`
- 函数：`?SetCtrl@CUIMessenger@@QAEXXZ` @ `0x7F34C0`
- 取证：`idapro.open_database` + Hex-Rays（2026-11-06）

## 关键立即数（展开态 `m_nState != 2`）

```text
v9     = 276          // x
nCloseY = 6           // y
StringPool → "UI/Basic.img/BtClose3"
CCtrlButton::Create(this, ctrlId=1000, x=v9, y=nCloseY, …)
```

最小化态（`m_nState == 2`）：`(191, 3)` —— 本端未实现 Min/Max，仅归档。
