# R27-B4 技能窗 r16 —— 真机复测 + 列表顺序取证

oracle：`evidence/r27-oracle/frames/native_skill_macro.png`（用户 2026-11 提供的
「技能展开宏指令.png」，379x291，含技能窗 175 + 技能组合栏）。
本轮出包：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`，4,454,400 B，
md5 `8f94636cbcec77a3ffefe233864bcf09`，2026-11-02 13:15。

## 1. 坐标口径（先把尺子钉死）

抓帧用 `_cap2.py`（PrintWindow），它抓的是**整窗**（含标题栏），而 `SetCursorPos`
要**客户区**坐标。本机实测 winrect=(632,385)、client=(640,416) ⇒ 固定偏移 **(8, 31)**。
`_ms_probe/_macrotap.py` 的 `tap_frame()` 就是先减这个偏移再打点；不扣的话所有
游戏内点击整体右下偏 (8,31)，表现为「点了没反应」——本轮前四次宏钮点击失败的真实
原因，不是按钮命中区坏了。

窗口定位：`layout.template_anchor(_r27_sk_open.png, _skill_backgrnd.png)`
= `(569, 130, rmse 96.4, agree 0.576)`。下文所有数字都是**帧坐标**（相对 backgrnd 左上）。

## 2. 逐格复测（本轮已改的常量）

| 件 | 帧目标（oracle） | 真机 ours | 判定 |
| --- | --- | --- | --- |
| tab 底板黑色外框 | x3..36 / y23..41 | x3..36 / y23..41 | 逐像素一致 |
| 页码圆点白墨 | x16..23 / y30..37（中心 19.5,33.5） | x16..23 / y30..37 | 逐像素一致 |
| 书页图标轮廓 | y57..84，右缘 x34 | y57..84，右缘 x34 | 逐像素一致 |
| 书名墨迹中心 | 104.5 | 104 | ≤0.5px |
| 书名墨迹顶 | 67 | 68 | 低 1px（见 §4） |

原始读数见 `live-measure-r16.txt`（oracle）与 `live-measure-r16b.txt`（真机）。

`BOOK_TEXT_INK_TOP` 暂**不**从 67 调到 66：1px 差来自字体度量（原生字形宽 58px，
我们 53px），而 R27-F 要把 SimHei 换成开源 CJK 字体，换完这个值必然重标。现在调
等于给下一个变量埋雷。

## 3. 宏指令组合栏：贴合 + 收缩（已闭环）

真机两连点（同一帧坐标 712,403 = `MACRO_BTN_TOPLEFT (121,265)` + 48x18 钮中心）：

- 第一点 → `live-macro-expanded-2x.png`：技能组合栏紧贴技能窗右缘（间距 −3），
  标题「技能组合栏」、3 个宏格 + 技能图标 + 「取个性技能名」「显示个性技能名」齐全。
- 第二点 → `live-macro-collapsed-2x.png`：组合栏消失，技能窗回到 175 原宽。

修复点在 `UISkillBook::set_macro()`：`macro_enabled=false` 时显式
`UI::get().get_element<UISkillMacroEx>()->deactivate()`。组合栏是独立窗，旧代码只
改 `macro_enabled`（那只管技能窗自己的宽度和 BT_MACRO_OK 的激活态），收不掉独立窗
——就是用户报的「展开后收缩不了」。两条关闭路径（再点宏钮 / 按「确定」）都过
`set_macro(false)`，所以收在 `set_macro` 里而不是调用点。

## 4. 技能列表顺序：根因已定，改法待用户拍板

**现象**：原生 4 行 = 团队治疗 / 疾风步 / 蜗牛投掷术 / 精灵的祝福；
真机 ours = 精灵的祝福 / 蜗牛投掷术 / 团队治疗 / 疾风步。见
`native-skill-list-order-4x.png`。

**id↔名称**（`raw-wz-order-String-Skill.txt`，按 String/Skill.img 的 name 字数反查）：

```
0000012 = 精灵的祝福 (5字)   0001001 = 团队治疗 (4字)
0001000 = 蜗牛投掷术 (5字)   0001002 = 疾风步   (3字)
```

**原生顺序 = WZ 文件原始顺序**（`raw-wz-order-Skill-000-skill.txt`，
`tools/_wz.py` 解析 `Data/Skill/000.img/skill`，不做任何排序）：

```
… 0001001, 0001002, 0001000, …, 0000012 …
= 团队治疗, 疾风步, 蜗牛投掷术, …, 精灵的祝福
```

过滤掉不可见/无 masterlevel 的（0009xxx 等）之后，正好等于 oracle 的 4 行顺序。

**我们的顺序 = 按子节点名字典序**：0000012 < 0001000 < 0001001 < 0001002，
`JobData` 直接 `for (nl::node sub : src["skill"])` 顺序入表
（[Data/JobData.cpp:38-46](../../../../../Data/JobData.cpp)），`UISkillBook::change_tab`
再原样 `emplace_back`（[IO/UITypes/UISkillBook.cpp:978](../../../../../IO/UITypes/UISkillBook.cpp)），
中间没有任何排序，所以显示顺序完全等于资源解析器的出表顺序。

**为什么是解析器出的表顺序**：客户端不是读预转换的 .nx，而是在进程内用
`wztonx` 直接解析 `.img`（[Util/WzRuntime.cpp:183](../../../../../Util/WzRuntime.cpp)），
`finalize` 之后子节点块已经被
`tools/vendor/NoLifeWzToNx-master/NoLifeWzToNx.cpp:970` 的 `sort_nodes()`
按名字重排过了。

**不能简单删掉 `sort_nodes`**：`node::get_child()` 在 WZ 后端下走
`wz_file::find_child` → `wztonx::get_child_full`
（[includes/NoLifeNx/nlnx/node.cpp:262](../../../../../includes/NoLifeNx/nlnx/node.cpp)、
[Util/WzRuntime.cpp:295](../../../../../Util/WzRuntime.cpp)），而 `get_child_full` 用的是
`std::lower_bound`（NoLifeWzToNx.cpp:571）。删掉排序 ⇒ 每一次
`node["name"]` 导航都失效，全客户端崩版式。

三个可选改法（**属共享资源解析器，按门禁 4 须用户确认后才动**）：

| 方案 | 做法 | 代价 |
| --- | --- | --- |
| A | 去掉 `sort_nodes`，`get_child_full` 改线性扫描 | 查找从 O(log n) 变 O(n)；宽节点很吃紧（String/Skill.img 根就有 613 个子节点，Item.img 更多），热路径（Icon/Tooltip 逐帧取图）可能明显掉帧 |
| B（建议） | 子节点块保持**文件顺序**，另建一张全局「按名字排序的下标数组」，`get_child_full` 在下标数组上二分 | 查找仍是 O(log n)，迭代恢复原生顺序，`node::data` 二进制布局不动；代价是每条父子边多 4 B（按已装载档案计），以及解析器多一个构建步骤 |
| C | 暂不改，文档记明「技能列表顺序与原生不同」 | 用户看到的顺序仍是错的；影响所有依赖 `img` 子节点顺序的列表（技能窗、可能的 Job/Quest 列表） |

本轮**未**触碰解析器，提交只含已实测闭环的版式 + 书名 i18n + 图标 origin 补偿 +
组合栏收缩。
