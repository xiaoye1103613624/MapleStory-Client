# B1a 裁定：位图 origin / 锚点只能有一套规则

用户报的第二轮缺陷「键盘设定 内部的按键图片全都偏上」是 G1 类根因的教科书样本：
**同一个窗口里，底图按「左上角」书写坐标，子层按「锚点」绘制**，两套语义叠在一起，
差值正好等于子层位图的 origin。这一篇把规则定死，并记下它是怎么被量出来的。

## 1. 渲染层事实（读码，不是推测）

| 位置 | 事实 |
| --- | --- |
| [Graphics/DrawArgument.h:134](../../../../../Graphics/DrawArgument.h#L134) | `Point<int16_t> rlt = pos - center - origin;` —— 绘制矩形左上 = 绘制点 **回减** origin |
| [Graphics/Texture.cpp:99](../../../../../Graphics/Texture.cpp#L99) | `Texture::draw` 把 `origin` 交给上式 —— 回减是**自动**的，调用点不写也照样发生 |
| [Graphics/Texture.cpp:77](../../../../../Graphics/Texture.cpp#L77) | `origin = sanitize_origin(src["origin"], dimensions)` —— 离谱 origin（实测见过 1156,8531）归零 |
| [Graphics/Sprite.cpp:29](../../../../../Graphics/Sprite.cpp#L29) | `absargs = stateargs + parentpos` —— Sprite 只加窗口位置，origin 仍在 draw 时回减 |
| [Graphics/Texture.cpp:107](../../../../../Graphics/Texture.cpp#L107) | `shift(a)` 的实现是 `origin -= a`，所以 `shift(get_origin())` 把 origin 变成 (0,0) |
| [IO/Components/Icon.cpp:51](../../../../../IO/Components/Icon.cpp#L51) | `Icon::draw(p)` 内部 `texture.draw(DrawArgument(p, ...))` —— 同样自动回减 origin |

⇒ 只有两种等价写法：
- **锚点语义**（原生 WZ 的书写习惯）：绘制点 = origin 在图内所指的那一点，直接 `draw(p)`。
- **左上语义**（本仓坐标表的书写习惯）：绘制点 = 位图左上角，必须先抹掉 origin
  （`tex.shift(tex.get_origin())`，或 `DrawArgument(tex.get_origin())` 把回减抵消掉）。

## 2. 现状普查（2026-11-01 grep）

| 写法 | 站点 | 语义 |
| --- | --- | --- |
| `if (o.x() \|\| o.y()) backgrnd.shift(o);` | **37 处 / 27 文件** | 左上（底图） |
| `DrawArgument(<bg>.get_origin())` | 3 处（仅 UIKeyConfig） | 左上（底图） |
| 裸 `sprites.emplace_back(Node[...])` / 裸 `texture.draw(p)` | 71 处 emplace（IO/UITypes） | 锚点 |

底图几乎一边倒用左上；**子层（图标/键帽/字模）几乎全是锚点**。底图与子层叠在一张窗口里，
差值 = 子层 origin —— 这就是「全都偏上」这类整块平移缺陷的来源，也是为什么它总表现为
「某个常量写错了」而实际上写错的不是常量。

## 3. KeyConfig 的量尺（`r27-oracle/KeyConfig.json`，全部由 `layout.py mark` 机器量出）

- 原生帧 `C:/Users/11036/Desktop/正确客户端截图/键盘设置.png`，629x369。
- 锚点 = `KeyConfig/BtClose/normal/0`（12x12），帧上 agree=1.000，落在帧 (606,5)。
  不能用 `backgrnd`（629x373）当锚：比帧高 4px，`anchor_in` 直接判「模板比帧大」。
- 素材事实（`nx_inspect`，非记忆）：`KeyConfig/backgrnd` 629x373 **origin (314,186)**；
  `KeyConfig/icon/<n>` 32x32 **origin (0,32)**（键帽美术）；
  `KeyConfig/key/<n>` 7~10x11 **origin (0,0)**（键帽上的字模）。
- 键帽行距：深色描边 (51,17,34) tol 0 min_run 20 实测行 y=59,90,92,123,125,156,158,189,191
  ⇒ 上沿 59/92/125/158/191，**行距恒 33、键帽高 32**。

把 4 个 `icon` 位图直接拿去帧上匹配（`kind: tmpl`），得到的是键帽**左上角**：

| 探针 | 窗口相对(BtClose) | 帧坐标 | 命中 `keys_pos` 表项 |
| --- | --- | --- | --- |
| `keycap_icon23` | (-592, 59) | (14, 64) | `GRAVE_ACCENT = (14, 66)` |
| `keycap_icon100` | (-524, 20) | (82, 25) | `F1 = (82, 27)` |
| `keycap_icon16` | (-185, 125) | (421, 130) | `APOSTROPHE = (421, 132)` |
| `keycap_icon9` | (-134, 92) | (472, 97) | `BACKSLASH = (472, 99)` |
| `btn_ok_normal` | (-598, 229) | (8, 234) | 代码里的 OK 按钮 `(8, 234)` |

`backgrnd` 左上角落在帧 (0,-2)（spec note 里 dy=+2、agree 0.815 的那次平移扫描），
按这个偏移换算，**四颗键帽的左上角与 `keys_pos` 表项逐格相等，误差 0**。
同一条表项在代码里被用作 `icon->draw(position + keys_pos[key] - Point(2, 3))`
—— 绘制点回减了 origin (0,32)，于是键帽美术整体落在 `keys_pos` **上方 32px**，
而字模层 `key/<n>` 因为 origin=(0,0) 落在 `keys_pos` 原位。美术与字模分家 ⇒ 「按键图片全都偏上」。

## 4. 裁定

**规则：一个窗口的坐标表只允许一种语义 —— 「位图左上角相对窗口 `position`」。**

- `position` ≡ 该窗口 `backgrnd` 的左上角（KeyConfig 已用 `DrawArgument(bg.get_origin())` 保证这一点）。
- 任何按这张表绘制的位图，绘制前必须抹掉 origin（`shift(get_origin())`），
  否则 [DrawArgument.h:134](../../../../../Graphics/DrawArgument.h#L134) 的回减会让它整体上偏一个 origin。
- **禁止**在同一窗口里让底图走左上、子层走锚点。混用时的表观症状不是「差几像素」，
  而是「一整层平移」，人眼排查会去改常量，改错了根因。

迁移策略（按 R27 批次，不搞一次性的 57 文件系统性改写）：
1. 窗口出了 oracle → 该窗口按规则对齐，子层补 `shift(get_origin())`，并把手写的 `-(2,3)`
   这类「补偿型 nudge」删掉；nudge 只有在表与语义对上之后才允许作为实测残差存在。
2. 没有 oracle 的窗口**不动**：`shift(o)` 那 37 处本身已经是左上语义，与规则一致，改它零收益、有风险。

## 5. 留给 B8 的实测残差（不猜）

上表在 `backgrnd 左上 = 帧 (0,-2)` 下四格全中，但 `btn_ok_normal` 命中代码常量要求 `dy=0`。
差值 2px 来自截图比 backgrnd 矮 4px（裁掉 2 还是 4 无法从这张静态截图判定），
它落在 spec 的 `tol=2` 内，且远小于本次要修的 32px。

⇒ 本轮只修 32px 那一档语义错误；`keys_pos` 是否需要再补 2px，**等 B8 真机抓帧**
（抓帧时窗口 `position` 已知，锚点可直接对 `position` 而非对 BtClose）实测后写回 oracle，
不在这里靠推断定稿。
