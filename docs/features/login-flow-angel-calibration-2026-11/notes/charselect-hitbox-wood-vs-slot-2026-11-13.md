# CharSelect 木牌 vs 人偶命中框（2026-11-13）

## 现象

- 点右侧木牌「开始游戏」会选中第 3 个角色。
- 选中角色后「删除角色」异常（木牌点击被槽位命中搅乱）。

## Angel（权威）

| 窗 | EA | 几何 | 职责 |
| --- | --- | --- | --- |
| `CUICharSelect` | ctor `0x60135B` / OnCreate `0x60148D` / OnButtonClicked `0x601667` | `CreateWnd(200,-1359,101,131)`；钮相对 `(4,-2)/(4,36)/(4,85)` | 仅木牌三钮 |
| `CUIAvatar` | ctor `0x603FF0` / SelectCharacter `0x60599B` | `CreateWnd(-290,-1218,600,80)` | 列表/选中 `%3` |
| `CUICharDetail` | ctor `0x60292F` | `CreateFadeWnd(183×151/115)`，x=`130*(i%3)-220` | 详情 Fade |

木牌 canvas x≈200–301 与槽2 detail x≈40–223 **不重叠**；WndMan 分窗 → 点木牌不改选中槽。

## 本端真因（单窗）

帧内（BeiDou signboard 锚点 + `CS_FRAME_ORIGIN`）：

| 对象 | TL | BR |
| --- | --- | --- |
| 槽2 旧 AreaButton `(-25,-220)/(233,250)` @ foot `(370,-1086)` | `(345,-1306)` | `(578,-1056)` |
| BtSelect | `(440,-1308)` | `(541,-1278)` |
| BtNew | `(440,-1268)` | `(541,-1233)` |
| BtDelete | `(440,-1217)` | `(541,-1174)` |
| signboard/0/0 origin `(66,119)` → TL | `(424,-1323)` | `(557,-1085)` |

旧槽2 框吞掉三钮；`send_cursor` 对所有命中依次 `button_pressed` → 木牌 + `CHARACTER_SLOT2` 双触发。

## 修复（`UICharSelect.cpp`）

1. 人偶框：相对脚点 `(-40,-200)`、`(80,210)` → 槽2 右缘 ≈410 &lt; 木牌左缘 424。
2. `send_cursor`：非槽位 MapleButton 优先，单击只处理一个命中。
3. `doubleclick` body_rect 与 AreaButton 同尺寸（`Point` 构造，避免 `Rectangle(left,right,top,bottom)` 参数序误用）。

## 删除 / 门闩回归口径

- `BtDelete`：`require_pic==0` → `CHAR_DEL_FAIL_NO_PIC`；`1/2` → 确认 → SoftKey/发包。本轮不改协议。
- `request_pending`：`clear_pending()`；`UIStateLogin::update` 在 `UILoginWait` 失活时清门闩（既有 r06）。命中修复后木牌点击不再误触槽位选中。

## 构建

- `MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-13 11:39，exit 0）。

## 手工验收

- ≥3 角色：点开始/创建/删除木牌 → 选中槽不变。
- 仅点人偶身体才选中；双击人偶仍可走开始游戏。
- 选中后点删除 → 确认或无 PIC 提示，不切到第 3 槽。
- （需重启客户端加载新 exe 后点测。）
