# B7-1 UIChannel 版式 oracle（原生 频道变更.png ↔ UIWindow.img/Channel 逐格比对）

原生帧：`C:/Users/11036/Desktop/正确客户端截图/频道变更.png` **428×122**
资产：`UI.nx / UIWindow.img / Channel`（本包**没有** `UIWindow2.img/Channel`，`UiAssetResolve::window("Channel")`
一定回落到 `UIWindow.img`）

## 0. 根因（先说这条，因为它就是「点了没反应」）

`Channel` 根的直接子节点实测只有：

```
BtCancel  BtChange  c  ch  channel0  channel1  channel2  s  t  world
```

**没有 `backgrnd`。** 而 [UIChannel.cpp:50](../../../../../IO/UITypes/UIChannel.cpp#L50) 的门槛是

```cpp
if (!UiAssetResolve::usable(Channel) || !Channel["backgrnd"]) { active = false; dimension = (0,0); return; }
```

⇒ 构造即自杀：窗口 0×0、不活动。`UIGameMenu::button_pressed(BT_CHANNEL)` 确实 `emplace<UIChannel>()` 了，
也确实 `deactivate()` 了目录气球 —— 用户看到的就是「点「频道更换」，目录关了，什么都没发生」。

原生这扇脸不是一张底板，是**三条横画布拼的**：`t`（标题 + 世界名条）+ 频道槽行 + `s`（按钮条）。

## 1. 坐标系换算（先钉死，后面所有数都靠它）

`nx_inspect.py image` 出的 `t`/`s` 是 436 宽，帧只有 428 宽 —— 不是缩放，是**截图裁掉了左右各 3~4 px**。
用列亮度剖面（[_fit_strips.py](_fit_strips.py) 输出 [_strip_fit.txt](_strip_fit.txt)）对齐：

| 条带 | 画布列 | 帧列 |
| --- | --- | --- |
| `t` 左边界 | x4=122 x5=181 x6=158 x7=245 | x1=122 x2=181 x3=158 x4=245 |
| `t` 右边界 | x428=245 x429=158 x430=180 | x425=245 x426=158 x427=180 |

⇒ **帧坐标 + 3 = 画布坐标**（横）。
行剖面同法：`t` 的 y27/y51/y52/y53 分别落在帧的 y26/y50/y51/y52 ⇒ **帧坐标 + 1 = 画布坐标**（竖）。

下面「窗坐标」= 画布坐标，即窗口左上角 = `t` 的左上角 = (0,0)。

## 2. 三条画布（整幅 SAD 最小，逐 oy 求最优 ox）

| 件 | 尺寸 | 窗坐标 | 帧实测 | 依据 |
| --- | --- | --- | --- | --- |
| `t` | 436×67 | **(0, 0)** | 帧 (-3, -1) | 行剖面 4 个拐点逐行等值 |
| `s` | 436×36 | **(0, 87)** | 帧 (-3, 86) | 同上 + 槽行闭合 |
| `c` | 436×10 | 未用 | — | 纯白细条，`t` 内部已含同色行，SAD≈1.8 到处都低，**不构成定位证据**，不接 |

竖向闭合：`t` 67 + 槽行 20 + `s` 36 = **123**；帧 122 = 123 − 1（顶部裁掉 `t` 的第 0 行黑边）。

## 3. 频道槽（68×20 板 + 20~25×10 标签）

板 = `channel0/1/2`，逐列扫 y=66（[_fit_strips.py](_fit_strips.py)）：

| 帧 x | `channel0` SAD | `channel1` SAD | `channel2` SAD | 帧里是什么 |
| ---: | ---: | ---: | ---: | --- |
| 6 | 79.58 | 63.28 | **6.76** | 蓝底 + ▶ + 白字「CH.1」 |
| 76 | **7.27** | 76.60 | 77.39 | 灰底 + 白字「CH.2」 |
| 146 | **7.47** | 76.64 | 77.48 | 灰底 + 白字「CH.3」 |
| 216 | **0.00** | 81.10 | 77.99 | 纯灰底，无字 |
| 286 | **0.00** | 81.10 | 77.99 | 纯灰底，无字 |
| 356 | **0.00** | 81.10 | 77.99 | 纯灰底，无字 |

`channel0` 在 216/286/356 **逐像素全等（SAD=0.00）**，间距 70、板宽 68 ⇒ 槽列就是 `x = 6 + 70i`（帧）
= **`x = 9 + 70i`（窗）**，`y = 66`（帧）= **`y = 67`（窗）**，**一排 6 个**。

三块板的语义（按平均色 + 上面这张表，不是猜的）：

| 节点 | 尺寸 | 平均不透明色 | 实测角色 |
| --- | --- | --- | --- |
| `channel0` | 68×20 | (224,222,212) 浅灰 | 普通/空槽底板 |
| `channel1` | 68×20 | (139,139,139) 中灰 | 本帧未出现 ⇒ 只能当「已点选、待变更」态（见 §6） |
| `channel2` | 68×20 | (57,168,219) 蓝 + 左侧白色 ▶ | **当前所在频道** |

> ⚠ 旧代码把「选中态」取 `channel1`、「未选中」取 `channel0`，于是**当前频道永远画成浅灰，蓝色 ▶ 板 `channel2` 一次都没用过** ——
> 就算窗口画出来了，第 1 格也不会是原生那个蓝底 ▶。

标签 `ch[i]`（白字「CH.n」，`ch/0`=20×10 … `ch/29`=25×10，共 **30** 个）：
`ch_0` 在帧 (14, 71)、`ch_2` 在帧 (154, 71) ⇒ 相对板左上角 **(8, 5)**，即窗坐标 **(17 + 70i, 72 + 20r)**。
（标签是白字、背景透明，整块 SAD 必然偏高，这里只取它的**位置**，不取 SAD 作判据。）

## 4. 世界名条 + 两颗橙钮

| 件 | 尺寸 | 帧实测 | 窗坐标 | SAD |
| --- | --- | --- | --- | --- |
| `world/0`（蓝蜗牛） | 48×18 | (13, 30) | **(16, 31)** | 4.75（`world/1`、`world/2` 分别 80.2、85.6 ⇒ 世界 0 就是蓝蜗牛，不是撞运气） |
| `BtChange/normal/0` | 60×19 | (283, 98) | **(286, 99)** | **0.07** |
| `BtCancel/normal/0` | 47×19 | (377, 98) | **(380, 99)** | **0.07** |

两颗钮 `origin` 实测都是 (0,0) ⇒ 窗坐标即左上角，可直接喂 `try_maple_btn`。
`BtChange` 在 `BtCancel` 左边，两钮底边 y=99+19=118，`s` 条带 87..122 ⇒ 钮相对 `s` 顶 = **12**。

> 旧代码：`MapleButton(Channel["BtCancel"])`（默认 (0,0)）和 `MapleButton(Channel["BtChange"], (-20,0))`
> —— 两颗钮叠在窗口左上角，还都盖住标题「频道变更」。

## 5. 版式常数（写进代码的那份）

```
SLOTS_PER_ROW = 6      板 68×20，列距 70，行距 20
GRID_TOP      = 67     t 高 67，槽行紧贴其下
S_Y(i)        = 67 + 20 * rows      s 条带顶（rows = ceil(槽数 / 6)）
槽 i 的板     = (9 + 70*(i%6), 67 + 20*(i/6))
槽 i 的标签   = 板 + (8, 5)
BtChange      = (286, S_Y + 12)
BtCancel      = (380, S_Y + 12)
world[w]      = (16, 31)
dimension     = (436, S_Y + 36)
```

## 6. 只能用户取证的两点（不假装量到了）

1. **`channel1` 到底是哪一态。** 本帧 selected == current，画面里没有第三种板出现的机会。
   要定它，需要在原生客户端里**点另一格但先不确认**截一帧。实装先按「待变更 = `channel1`（中灰）」接，
   这一格若与原生不符，只影响选中高亮的颜色，不影响其余 15 个格位。
2. ~~**本帧只有 3 个带字槽 + 3 个空槽，而登录页是 CH.1–CH.20。**~~ **已闭合（本机包取证，不需要用户）。**
   登录期 `SERVERLIST` 的字节就在本仓运行目录的 `packet_dump.log` 里，`[SEND] 0b 00` 之后那条 RECV：

   ```
   0a 00 | 00 | 06 00 "Scania" | 00 | 06 00 "Scania" | 64 00 64 00 00 | 03 |
         | 08 00 "Scania-1" 00 00 00 00 01 00 00 |
         | 08 00 "Scania-2" 00 00 00 00 01 01 00 |
         | 08 00 "Scania-3" 00 00 00 00 01 02 00 | 00 00
   ```

   按 [LoginParser::parse_world](../../../../../Net/Handlers/Helpers/LoginParser.cpp#L54) 的字段顺序逐步对齐：
   `wid=0` / name / flag / event_message / `skip(5)=64 00 64 00 00` / **`channelload_size = 03`** /
   三条 channel（名字+负载 int+wid+cwid+adult）/ `skip_short`。
   ⇒ **本世界（Scania, id 0）真实频道数 = 3**，`slot_total()` 向上取整到整排 = 6 格 1 行 = 3 带字 + 3 空板
   = 原生 `频道变更.png` 的画面。登录页的 CH.1–CH.20 是另一个世界的列表，两者不矛盾。

   落点：`Configuration::set_channel_total(world.id, world.channel_count)` 由
   [ServerlistHandler](../../../../../Net/Handlers/LoginHandlers.cpp#L269) 在解包时就地登记（内存表，不落 Settings），
   `UIChannel` 构造时按 `get_channel_total(get_worldid())` 取；取不到（本轮没解过 SERVERLIST）
   退成 `channelid + 1`，不再用猜的 20。
   > 没有改 `UIWorldSelect::enter_world`（那里本来最顺手），因为该文件正被并行会话改动中。

## 7. 复跑

```
python _fit_strips.py     # t/s/c 整幅 SAD 落位 + channel0 全行扫描 + 两颗钮
python _probe_grid2.py    # ch[i] 标签、world[i] 世界名、channel1 灰板
```

两个脚本都只读本目录 `_dump/` 与桌面原生帧，不起客户端。`_dump/` 是 `tools/nx_inspect.py image` 出的原始位图。
