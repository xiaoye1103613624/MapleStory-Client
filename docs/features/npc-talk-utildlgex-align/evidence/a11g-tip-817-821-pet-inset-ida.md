# A11g 原生 tip 817–821 + Pet `speaker&6` inset=25 — IDA 取证

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）

## 1. tip 弹出路径 `sub_9929DD`（0x9929DD）

`sub_9AB50A` 校验失败时：

1. `sub_406455(id)` 取 StringPool 文案（817–821）
2. 带 `%d` 的用 `sub_445B4B` 格式化（818/819/820/821）
3. `sub_9929DD(...)` 建 tip 窗（对象 size `0xD8`，ctor `sub_8D51D4`）
4. `sub_992EDA(..., duration=1000, …)` → `a1[37]=1000`
5. `sub_4EDAB7(0,0,1,0)` 居中创建窗；`sub_4EDBA1` 模态等待
6. **不关** UtilDlgEx（对话窗保持）

### Draw `sub_99311B`（vtable+0x2C @ `off_B3BB24`）

当 `this[37]==1000`（9929DD 路径）：

| 项 | 证据 |
| --- | --- |
| 正文字号 | `sub_98A707` **style 19** → `push 0Ch` = **12** |
| 正文字色/α | style 19 → `push 0FFFFFFFFh` = **白 α=1.0** |
| 描边字号 | style **20** → `push 0Ch` = 12 |
| 描边色/α | style 20 → `push 0FF355A84h` = **RGB(53,90,132) α=1.0** |
| 字体名 | StringPool **5527** = `Arialdf`（本仓仍走思源宋体路径） |
| 行距 | Draw 内 `v137 += 14` |
| 窗宽 | OnCreate `sub_991803` → width **266**（Notice 族） |

### StringPool 文案（`beidou_stringpool.py` 解密）

| id | 文案 |
| --- | --- |
| 817 | `Only numbers are allowed.` |
| 818 | `You may only enter a number \r\nequal to or higher than %d.` |
| 819 | `You may only enter a number \r\nequal to or lower than %d.` |
| 820 | `Please enter more than %d digits.`（AskNumber `+0xDC` 常为 0，少触发） |
| 821 | `Please enter at least %d characters.` |

校验映射（`sub_9AB50A`）：非数字→817；`<min`→818；`>max`/越界负→819；文本 `len<min`→821。

## 2. Pet `speaker&6` inset=25

| 证据点 | 地址/结论 |
| --- | --- |
| 包装写标志 | `sub_746A8B` / `sub_746DFF`：`this[441] = (speaker & 6) != 0`；`this[440]=speaker` |
| 布局读标志 | `sub_9ACC20`：`content_left = this[441] ? 25 : 157`（非 Avatar 输入槽） |
| Pet | `speaker=2` → `(2&6)!=0` → **inset=25** |
| Avatar | `speaker=1` → `(1&6)==0` → inset 仍 157；且走 `sub_9A8716` 固定槽（与本字段无关） |

## 3. 本仓接线

- `UINotice` OK tip：`OutlinedText` A12M 白 + `NOTICEOUTLINE(#355A84)`；α=1.0；宋体路径不变
- `UINpcTalk::dispatch_input_field` 失败 → `UIOk` 弹 tip 817–821，对话窗不关
- 非 Avatar GetText/Number：`(dlg_speaker&6)!=0` 时输入槽 `INPUT_LEFT_PET=25`

## 4. 局限

- ~~tip 820（位数下限）~~ → **A11h 已接**（线路径仍置 0；见 a11h）
- Draw 原生 duration=1000 为多偏移描边；本仓用四向 `OutlinedText` 近似
- Pet Say 正文 `TEXT_X` 未改（无独立 inset 证据；仅输入槽 9ACC20）
