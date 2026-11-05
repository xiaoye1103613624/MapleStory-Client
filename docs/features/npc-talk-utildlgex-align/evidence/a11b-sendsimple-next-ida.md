# A11b SENDSIMPLE「下项」— IDA / IMG 取证

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）｜ IMG：同目录 `Data/UI/UIWindow.img`

## 1. 类与入口

| 符号（本 IDB 无 demangle） | 地址 | 角色 |
| --- | --- | --- |
| `CUtilDlgEx::CreateUtilDlgEx` | `0x9A3E38` | 居中 CreateWnd（插件 rs.cpp 钩子同址） |
| `CUtilDlgEx` ctor | `0x9A31AD` | |
| `SetUtilDlgEx` | `0x9AC552` | `this+124 = dlgType`；**type==4** 时正文偏移 `+24` |
| 布局分发 | `0x9A6370` | `switch (dlgType)` → type4 调 `0x9A8FEF` |
| LIST 建钮 | `0x9A8FEF` | **BtClose id=2**（左）+ **BtNext id=8193**（右） |
| `OnButtonClicked` | `0x9AB92A` | 见下 |
| OK/校验收口 | `0x9AB50A` | GetText/GetNumber 等 |

## 2. dlgType 映射（与本仓 `TalkType`）

| dlgType | 布局函数 | 本仓 |
| --- | --- | --- |
| 0 | `0x9A7C8B` TEXT | SENDOK / NEXT/PREV 变体 |
| 1 | `0x9A8062` YESNO | SENDYESNO |
| 2/3 | GetText / GetNumber | SENDGETTEXT / SENDGETNUMBER |
| **4** | **`0x9A8FEF` LIST** | **SENDSIMPLE** |
| 5+ | Image / Avatar 等 | 非本切片 |

## 3. type==4「下项」语义（定案）

`sub_9AB92A` case 4：

```text
if (buttonId == 8193) {           // BtNext
  m_nSelect = list[cur].field40;  // 当前高亮项 selection
  Finish(8193);                   // 收口成功
}
```

- **不是**「空操作 / 半成品」。
- **不是** wasm 那套「下项=循环高亮、确定才发包」；原生 LIST 的 **下项本身就是确认当前高亮并发包**。
- 点列表项设 `cur`（本仓：悬停更新 `simple_selected`，点 `#L` 仍可直接 `NpcTalkMorePacket(sel)`）。

TEXT 型按钮 id（对照）：`0x2000`=Prev、`0x2001`/`8193`=Next、`1`=OK、`2`=Close（`0x9A7C8B`）。

## 4. IMG（orange-wz）复核

`UIWindow.img/UtilDlgEx`：

| 节点 | 尺寸 |
| --- | --- |
| `t` / `c` / `s` | 529×28 / … / 529×58 |
| `BtNext/normal/0` | **46×18** |
| `BtClose/normal/0` | **85×19** |

与 R25 常量 `BTN_X_RIGHT=474`、`BTN_DY=49`、`BTN_X_CLOSE=12` 一致；本切片不改几何。

## 5. 字体

名牌 `A11M` 白 / 正文 `A12M` DARKGREY；走 `font.path.*` 思源宋体，**未回 SimHei**。
