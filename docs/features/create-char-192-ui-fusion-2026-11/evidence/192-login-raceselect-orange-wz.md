# 192 Login.img / RaceSelect_new 取证（orange-wz）

> 日期：2026-11-18  
> 密钥：`新版本客户端` / `ivBase64=AAAAAA==` / `userKeyBase64`=128B `DEFAULT_KEY`（`tools/_gms_key_128.b64`）  
> 源：`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\Data\UI\UI_001.wz`

## 定位

| 包 | Login.img | RaceSelect* |
| --- | --- | --- |
| UI_000.wz | 无 | 无 |
| **UI_001.wz** | **有** | **`RaceSelect_new` only** |
| UI_002 / UI_003 | 无 Login.img | search RaceSelect 空 |

192 `Login.img` 顶层（与创角相关）：`RaceSelect_new`、`CustomizeChar`、`NewChar`、`CharSelect`、`WorldSelect`、`Common`…  
**无**经典 `RaceSelect`（无 knight/normal/aran 三卡体系）。

## RaceSelect_new 结构（已 list）

子节点含：`order` / `button` / `buttonDisabled` / `make` / `leftArrow` / `rightArrow` / `Back`/`Back0`–`Back3` / `BackAni` / `confirm` / `hotList` / `newList` / `ToolTip` 等。

- `button`：索引 `0–27` + `1000–1003`（各含 normal/mouseOver/pressed/keyFocused/disabled）
- **龙神独立入口**：`button/4`（Classes::EVAN=4）齐全五态
- **双刀独立入口**：`button/8`（Classes::DUALBLADE=8）齐全五态
- `confirm/race`：含 `0–27` 与 `1000–1003` 画布（含 race/4、race/8）

## order 列表（版本键）

`order` 子键：`0–36` + `178/183/186/190/192`。

本端 modern 代码硬读 `order[35]`（`SELECTED_LIST=35`）。192 另有 **`order/192`**（版本专用序）。

### order/192 值（30 项，含龙神/双刀）

```
1,8,13,15,27,25,1000,24,23,22,21,20,19,3,4,5,7,10,16,2,9,0,6,14,11,12,18,1001,1002,1003
```

映射（对齐本端 `UIRaceSelect::Classes`）：

| 值 | 职业 |
| --- | --- |
| 0 | RESISTANCE |
| 1 | EXPLORERS |
| 2 | CYGNUSKNIGHTS |
| 3 | ARAN |
| **4** | **EVAN（龙神）** |
| 5 | MERCEDES |
| 6 | DEMON |
| 7 | PHANTOM |
| **8** | **DUALBLADE（双刀）** |
| … | 其余高版本职业 / 1000–1003 海外等 |

### order/35 值（27 项）

同样含 **4=EVAN**、**8=DUALBLADE**（与 192 序略有差异；无 1000 系列靠前项）。

**结论**：192 职业团选择 UI **具备全职业团书架**；龙神/双刀是 **独立 button 槽**，不是复用战神/冒险家。

## CustomizeChar（创建页 2 / 外观板）

含大量 classType，至少：`000`、`000_1`（双刀常用）、`1000`、`2000`、`2001`（龙神）、`2002+`、`3000+`、`5000`、`6000+`、`10112`、`13100`…  

已核实：

- `CustomizeChar/2001`：avatarSel / BtYes|No|Left|Right / charName / charSet / board / genderSelect / info（龙神专属板）
- `CustomizeChar/000_1`：同上 + hairSelect（双刀专属板形态）

## NewChar

192 仍有 `NewChar`（含 createLimit / avatarSel / Bt*），但主路径是 **RaceSelect_new → CustomizeChar**，不是 v83 `NewCharKnight/Aran` 三板。
