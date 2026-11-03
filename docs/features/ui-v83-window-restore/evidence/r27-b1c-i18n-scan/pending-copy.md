# NO-NATIVE 文案待取证台账（R27-B1c）

**这张表是什么**：B1c 全量接入按用户拍板走「保持英文原值，只建键」，于是有一批键的**值**是本包自造的、
没有 v83 原生依据。门禁禁止凭记忆编词条（`String.img` / IDA 才算依据），所以这些键**只登记不翻译**：
日后逐条取证后**只改 ini 的值，代码不用动**（站点已在 `locale/*.ini`，绑定已在窗体）。

| 键 | 现值（zh / en） | 站点 | 建议取证来源 | 状态 |
| --- | --- | --- | --- | --- |
| `version.prefix` | `Ver. {}` / `Ver. {}` | `UILogin.cpp:76`、`UICharSelect.cpp:136`、`UIRaceSelect.cpp:156`、`UICommonCreation.cpp:78 / :360` | 原生版本号那行是 `Login.img/Common/version/pos` 定位的**位图数字**（`UIWindow.img/.../Number`）还是自绘文本，决定要不要这个前缀 —— 用 orange-wz 只读查 `pos` + 原生截帧对照 | 待取证（当前值 = 原英文字面量，行为不变） |
| `charselect.rank_unavailable` | `排行不可用` / `排行不可用` | `UICharSelect.cpp:384`（ctor 绑定）、`:1305`（update 无排行分支） | 角色选择窗「排行」行在原生 v83 是 `Login.img/CharSelect` 的烘焙位图还是自绘 `Text`；若自绘，真文案在 `String.img/CharSelect` | **两份同值**：en 不编英文（编一个和编一个是同一种错）。待取证 |
| `charselect.rank_up` | `{} up` / `{} up` | `UICharSelect.cpp:1297` | 原生排行升降是**箭头位图**（`Login.img/CharSelect/.../arrow`）而非文字后缀的可能性大 —— 若是，正确做法不是换文案，是改成贴图 | 待取证 |
| `charselect.rank_down` | `{} down` / `{} down` | `UICharSelect.cpp:1299` | 同上 | 待取证 |

## 为什么模板里带占位符而不是「数字 + 独立后缀」

`Util/Locale.cpp` 的加载器会 `trim` 掉值两端空白，所以单独存 `" up"` 只会得到 `"up"`；
后缀连空格一起待在模板里（`{} up`），占位符两侧的空格由每种语言自己决定 —— 这是
`Locale::format` 的既定约定，不是这次的临时发明。升降档拆成「名次」+「上/下」两条键反而更糟：
中文会拿到「42 上」这种断不成句的东西，且没法表达「上 42」这种语序。

## 判据：哪些键**不进**这张表

只有 `# NO-NATIVE` 整行注释标出来的键才登记在此。已有原生依据的（`equip.pot.*`、
`sysopt.combo.*` 取自截帧等）不列 —— 那张表一旦混进有依据的条目，就没人再信「表里的都值得去取证」。

## 与「站点归零」的关系

`tools/scan_display_text.py` 的清点（`inventory.txt`，402 处）里，**壳窗 333 处**是英文自造提示，
按决策 B 同样「保持英文原值只建键」。那一批键落地时按同一格式追加到这里：一行一键，
值照抄现英文，状态写「待取证」。**所以这张表会随 B1c-5/B1c-6 变长，不是收尾时才写的总结。**
