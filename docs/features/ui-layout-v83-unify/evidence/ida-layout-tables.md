# IDA / 布局证据摘要

## KeyConfig（`CUIKeyConfig`）

- ctor：`0x832586`，资源 `UI/UIWindow.img/KeyConfig`
- 槽位表：`dword_BE27E0`（X,Y int32 对，32×32 命中）
- `GetShortCutIndexByPos`：`0x833a65`；长键视觉槽 54/89/90 映射到 Shift/Ctrl/Alt

主要坐标（节选）：

| Key | (x,y) |
| --- | --- |
| GRAVE | 14,66 |
| NUM1–EQUAL | 48..422, 66 |
| Q–RBRACK | 64..438, 99 |
| A–APOS | 81..421, 132 |
| Z–PERIOD | 98..370, 165 |
| F1–F10 | 82..404, 27 |
| INS/HOME/PGUP | 514/548/582, 66 |
| DEL/END/PGDN | 514/548/582, 99 |

HeavenClient 枚举 F11/F12 与官方位号 87/88 纹理对齐；本表将 F11/F12 画到 (438,27)/(472,27)。

## Equip（`CUIEquip`）

- ctor / CreateWnd：宽 175（或龙装备 208）× 高 304
- 槽位表：`dword_BE2260`；`sub_7FEC32` 命中（跳过 body 14 与 20–47）

| EquipSlot | (x,y) |
| --- | --- |
| HAT | 38,35 |
| FACE | 38,68 |
| EYEACC | 71,101 |
| EARACC | 104,101 |
| TOP | 38,134 |
| BOTTOM | 38,167 |
| SHOES | 71,200 |
| GLOVES | 5,167 |
| CAPE | 5,134 |
| SHIELD/SUBWEAPON | 137,134 |
| WEAPON | 104,134 |
| RING1–4 | 见实现 |
| PENDANT1 | 71,134 |

## StatusBar

- `StatusBar.img/base/backgrnd` 800×71；`quickSlot` 151×80
- 1280 下 `right = VWIDTH-800`，QS 使用 `648+right`
