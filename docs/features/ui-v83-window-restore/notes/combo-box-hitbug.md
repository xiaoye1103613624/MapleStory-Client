# MapleComboBox 命中区缺陷（B5-2 取证时发现，未修）

## 结论

`MapleComboBox` 的选项 `AreaButton` 命中矩形用的是**还没赋值的 `position`**，
构造完之后才 `position = pos`，所以每一条选项的可点区都少了控件自身的偏移。

## 代码证据

[IO/Components/MapleComboBox.cpp:76](file:///E:/project/MapleStory-Client/IO/Components/MapleComboBox.cpp#L76) 起：

```cpp
Point<int16_t> option_pos = Point<int16_t>(position.x(), position.y() + textures[state][0].get_dimensions().y()) + parentpos;   // L76
for (size_t i = 0; i < option_text.size(); i++)
    buttons[i] = std::make_unique<AreaButton>(...);   // L79  ← 这里已经把命中区定死
...
position = pos;                                       // L85  ← 偏移在这之后才写进成员
```

`draw()` 一侧的行位置也不是同一把尺：底板 `pos.y()+2`、内区 `+3`、正文
`i*HEIGHT - 4`（[L115-L124](file:///E:/project/MapleStory-Client/IO/Components/MapleComboBox.cpp#L115)），
而命中矩形是 `option_pos.y() + i*HEIGHT + 1` ⇒ 绘制与命中相差 3~5px（R27 的
「绘制/命中不同源」这一类 durable 根因，见 [project-ui-v83-window-restore](../../../../memory/project-ui-v83-window-restore.md)）。

## 为什么本轮没修

`grep -rn "MapleComboBox(" IO/UITypes/*.cpp` **零命中** —— 本包没有任何窗体在用这个
组件，缺陷还在潜伏期，不在用户报的缺陷路径上。按「最小改动 / 不借新功能改无关代码」
的门禁，本批不复用它，`UIWhisperTargetDlg` 的下拉直接用 `Basic.img/ComboBox` 三件
自己画 + 自己的行命中区（与绘制同一处常量），这样版式取证与命中是同一来源。

后续谁要用 MapleComboBox（比如 B6/B7 的下拉），先修这两处：
1. 把 `position = pos;` 提到 `option_pos` 计算之前；
2. 行偏移抽成一个常量，`draw()` 与 `AreaButton` 共用。
