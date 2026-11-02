# R27-B3b 装备 tooltip / 怪物手册 非位图文案 i18n

> 目标（用户 2026-11-01 定调）：客户端中**不是位图画出来**的文案一律走国际化，默认中文、
> 配置驱动、后期可运行时切换。位图烘焙文字（原生 `ReqLevel` 那种精灵字）不在此列。

## 1. 改了什么

| 文件 | 改动 |
| --- | --- |
| `Util/Locale.h` | 新增 `enum_key` / `enum_text` / `format` 三个静态助手（+37 行，无成员变更） |
| `IO/Components/EquipTooltip.cpp` | 装备 tooltip 全部英文字面量 → Locale 键（预览分支与真实分支同源改） |
| `IO/UITypes/UIMonsterBook.cpp` | 怪物手册标题 / 封面 / 卡片数 / `Lv.` 行 / `Mob #` 兜底 → Locale 键 |
| `locale/zh-CN.ini`、`locale/en-US.ini` | 5 键 → **79 键**，两份键集严格对齐 |
| `MapleStory.vcxproj` | 4 处 `PostBuildEvent` 追加 `mkdir locale` + `xcopy /y /d`，把 ini 同步到 OutDir |

文案命名空间：`equip.label_*` `equip.gender.*` `equip.pot.*` `equip.stat.*`(15)
`equip.type.*`(32) `equip.speed.*`(9) `monsterbook.*`。

## 2. 两个非显然的坑（本轮真正的产出）

### 2.1 部署侧：改完 ini 不重新出包 = 全部键回显成键名

`Locale::load_file("locale/zh-CN.ini")` 按**客户端工作目录**解析，OutDir 里那份是
10 月 22 日的旧档（404 B / 5 键），仓库里已是 2,952 B / 79 键。缺键时 `tr()` 静默回显
键名，于是 tooltip 上会出现 `类型：equip.label_type`。这不是「忘了拷文件」的偶发问题，
而是**构建流程里根本没有这一步**，所以补进 post-build，而不是靠人手拷贝的习惯。

本次出包后已核对：OutDir 两份与仓库 `md5` 一致（`SYNCED`）。

### 2.2 键名折叠：连字符不折，6 条武器类型必然脏文案

`enum_key` 把数据表英文枚举折成 ini 键后缀。武器类型表里有 **6 条带连字符**：

```
ONE-HANDED SWORD / ONE-HANDED AXE / ONE-HANDED MACE
TWO-HANDED SWORD / TWO-HANDED AXE / TWO-HANDED MACE
```

ini 按 `_` 建档（`equip.type.ONE_HANDED_SWORD`），而修复前的 `enum_key` **只折空格、
不折连字符**，折出 `equip.type.ONE-HANDED_SWORD` → 查不到 → 回显键名。这 6 条恰好是
单手剑/斧/锤、双手剑/斧/锤，是玩家最常看的武器类型。

`Util/Locale.h` 上那段注释本来就写的是「`ONE-HANDED SWORD` → `ONE_HANDED_SWORD`」，
即**注释描述了意图、代码没实现**，所以光读代码看不出问题。

**为什么真机悬停抓不到它**：测试角色是 Lv1 新手，身上无武器、背包 `装备` 页全空，
带连字符的类型在这台号上永远 hover 不到。这类「数据表封闭可枚举、运行期才拼键」的
缺陷，逐格真机验证既贵又漏，必须用穷举门禁。

## 3. 门禁：`tools/check_locale_keys.py`

```
python docs/features/ui-v83-window-restore/tools/check_locale_keys.py
```

从**真实数据表**（`Data/EquipData.cpp` 两张 `types[]` + else 兜底 `"CASH"`、
`Data/WeaponData.cpp::getspeedstring`、`Character/EquipStat.cpp::names`）抠出全部枚举值，
按与 C++ 侧一致的 fold 规则折成键，要求在 zh-CN **和** en-US 里都存在；另查两份键集漂移、
空值，以及「数据表含空串时 `enum_text` 的空值短路是否还在」。

解析规则逐条对齐 `Util/Locale.cpp::load_file`（去 BOM、去 `\r`、整行 trim、`#`/`;` 跳过、
键值各自 trim、重复键后写覆盖）—— 加载器会 trim 值两端空白，所以「`Type: `」这种尾随空格
存不进去，改用 `{}` 占位符 + `Locale::format`，空格由各族语言自己决定放不放。

### 3.1 全绿读数（见 `gate-check-locale-keys.txt`）

```
zh-CN keys = 79
en-US keys = 79
equip.speed.     枚举  9 条  zh 缺 0  en 缺 0
equip.stat.      枚举 15 条  zh 缺 0  en 缺 0
equip.type.      枚举 32 条  zh 缺 0  en 缺 0

GATE OK: 语言文件键集对齐，且所有数据表枚举均能折成已建档的键
```

### 3.2 变异自证（见 `gate-mutation-prefix-fold.txt`）

把 `fold` 退回修复前「只折空格」，门禁必须失败：

```
- zh-CN 缺动态键 equip.type.:
  equip.type.ONE-HANDED_AXE / ONE-HANDED_MACE / ONE-HANDED_SWORD
  equip.type.TWO-HANDED_AXE / TWO-HANDED_MACE / TWO-HANDED_SWORD
- en-US 缺动态键 equip.type.: （同上 6 条）
MUTATED EXITCODE=1 (期望非 0)
```

即：门禁能抓到本轮修的这个 bug，不是一台只会印 OK 的机器。

## 4. 真机自证

出包 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
`md5=76e04bb9ec7158f7c75bdb18fecc9c98`，4,455,936 B，11:42（前序 `3aefafb4` 11:35 为修复前）。
构建前置门禁 `python tools/gen_opcode_matrix.py --check` → `GATE OK (s9 44d0d5d9ca)`。

| 帧 | 内容 |
| --- | --- |
| `live-76e04bb9-equip-nohover.png` | 装备窗无悬停（锚点基线） |
| `live-76e04bb9-equip-window.png` | 悬停「上衣」格，tooltip 全窗帧 |
| `live-76e04bb9-tooltip-top-crop.png` | 同帧 tooltip 2x 裁切（判据图） |

裁切图可读出的行（全部来自 ini，非位图）：

```
白棉T恤 （男）            ← equip.gender.male（原 " (M)"）
攻击力增加                ← equip.label_atkinc（原 "ATT INCREASE"）
类型：上衣                ← equip.label_type + equip.type.TOP
防御力：3                 ← equip.stat.Defense
剩余强化次数：7           ← equip.label_slots
已用锤子：0               ← equip.label_hammers
```

**版式未回归**：同一张 `Equip_backgrnd.png` 模板锚点在新 exe 上仍是
`anchor=(258, 191) rmse=48.11 agree=0.952`，与修复前逐字一致（本轮只动文案，不动几何）。

要求等级/力量/运气/敏捷/智力那几行是**原生位图**，不经 `Text`，按定调保持原样。

## 5. 未做与原因

- **职业名那坨横排叠印**（裁切图里 `黑骑士` 位置的花字）：刻意**保持原样**。
  前序逐格测量已证明原生客户端同样这么画，见
  [`../w5-tooltip/equip-native-compare.md`](../w5-tooltip/equip-native-compare.md) §4。
  改成 i18n 反而偏离原生，属「最小改动」禁区。
- **带连字符武器类型的真机 hover**：本机测试角色无武器，未取证；由 §3 穷举门禁 +
  §3.2 变异自证覆盖。若后续拿到有武器的号，应补一张 `类型：单手剑` 的帧。
- **运行时切语言**：`Locale::reload()` 已按 `Configuration::ui.language` 取档，
  但游戏内切语言的 UI 入口未做（属后续 i18n 任务，非本轮范围）。
- 语言文件里 `worldselect.stub_view_map` / `worldselect.stub_view_all` /
  `worldselect.label_view_all` 三键**无代码引用**（前序 i18n 铺路遗留，非本轮新增）。
  按最小改动未在本提交清理。

## 6. 待用户验证

1. 中文环境下装备 tooltip、怪物手册文案是否还有漏网的英文字面量。
2. 把 `ui.language` 改成 `en-US` 后，上述两窗是否整窗切英文、无缺键回显。
