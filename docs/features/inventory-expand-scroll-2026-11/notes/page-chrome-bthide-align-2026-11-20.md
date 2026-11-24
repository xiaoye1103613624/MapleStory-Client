# 翻页三件套改用 BtHide2/BtHide3 + 中线对齐

> 日期：2026-11-20 ｜ 范围：仅展开态翻页 chrome 视觉与坐标

## 变更

| 项 | 旧 | 新 |
| --- | --- | --- |
| 上一页 | `MonsterBook/arrowLeft`（估宽 14） | `Basic.img/BtHide3`（12×12，左箭头） |
| 下一页 | `MonsterBook/arrowRight` | `Basic.img/BtHide2`（12×12，右箭头） |
| 禁用态 | MapleButton 缺态回退 normal | 各自 `disabled/0`（含 `BtHide3/disabled`） |
| 垂直对齐 | 箭头 y=266；输入框 y=267 | 相对 `BtCashshop` 中线：箭头 y=269、输入框 y=267 |

## 对齐公式

- `BtCashshop` 顶左 `(W-113, 266)`，高 18 → `center_y = 275`
- 箭头 top = `275 - 6 = 269`
- 页码框 top = `275 - 8 = 267`
- 横向仍从 cashshop 左缘往左：`下一页 → 输入框 → 上一页`，`gap_edge=8`、`gap_inner=3`

## 证据

- orange-wz live：`E:/MXD/BeiDou-Client-xy/Data/UI/Basic.img`、`UIWindow.img/Item/BtCashshop`
- 方向：像素重心 `BtHide3` 偏左、`BtHide2` 偏右（见 `evidence/BtHide*_normal.png`）
- 实现：`IO/UITypes/UIItemInventory.cpp` 构造块「展开态翻页三件套」

## 构建

见 [builds/index.md](../builds/index.md)：`Release|x64` → `x64/bin-bthide/MapleStory.exe`，exit 0，SHA16 `6DE3BC10E804B5DE`。未部署 live。

## 真机验收

1. 展开背包：上一页 / 页码框 / 下一页 / 「扩充道具栏」同一水平中线
2. 第 1 页：上一页灰（`BtHide3/disabled`）；末页：下一页灰（`BtHide2/disabled`）
3. 点翻页、输入跳页、滚轮翻页仍联动页码；整理/切页签正交行为不变

