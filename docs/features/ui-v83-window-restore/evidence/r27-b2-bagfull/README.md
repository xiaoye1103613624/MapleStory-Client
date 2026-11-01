# R27-B2 背包展开态（FullBackgrnd）逐格定案

用户报：「现有道具：背包栏物品显示怎么都偏上了？金币值位置偏下了，没有在框内」。
折叠态在 B1b 已按 `r27-oracle/Item.json` 重量过；本轮补 **展开态** 的 oracle（`ItemFull.json`），
建完立刻抓到真缺陷。

## 尺子（两条独立证据，都落在原生帧 `frames/native_bag_open.png` 602x291）

1. **72 颗 `Item/disabled` 全片暴力 SAD**（不是 FFT 匹配，逐格零容差）：命中 **12 列 × 6 行**，
   列 x = `157,193,229,265 | 306,342,378,414 | 455,491,527,563`，行 y = `51,85,119,153,187,221`（帧坐标）。
   页内列距恒 36，页首差 41 ⇒ **页距 = 3×36+41 = 149**。
2. **backgrnd 自己画的格线**（`full_grid_rows` band 探针，色 `(204,204,187)`）：行 y = `50,84,118,152,186,220`
   （窗口相对，锚点 dy=1），竖向列线 x = `7,43,79,115`（页内）+ 页边界 **双线** `151/156`
   ⇒ 页 0 列起点 `8/44/80/116`，与 `157−149` 相合。

折叠态帧上没有锁定格 ⇒ 「格顶」这条尺只有展开态量得到，(8,50) 对两态同时成立。

## 缺陷与改动

| 常数 | 旧 | 新 | 依据 |
| --- | --- | --- | --- |
| `UIItemInventory.cpp` 回退 `slot_pos` | `(8,51)` | `(8,50)` | 格顶与 `(204,204,187)` 格线**同一行**；旧值是从紧裁帧的**帧坐标**抄来的，没减锚点 dy=1 |
| 同文件 `page_dx[4]` | `{0,150,299,448}` | `{0,149,298,447}` | 页距实测 149；旧值把第 2/3/4 页整体右移 1px |

## 门禁读数

- `native-selfcheck.txt` —— `ItemFull.json` 对自己那张原生帧：**8/8 Δ=0**（6 颗 disabled 全 agree=1.000）。
- `live-pre-fix-check.txt` / `live-pre-fix-annotated.png` —— 同一套探针打在**改前的真机窗口帧**
  （1296x759，自动锚点 agree=0.825；带绿色选框的那张也留了，agree 因此从 1.000 掉到 0.979）
  ：**7/8 FAIL，6 颗 disabled 齐刷 Δ=(+1,+1)**，
  且 `full_grid_rows` 报出 **12 条双行** `[50,51,84,85,…]` —— 位图格线与覆盖层错开 1px 的典型症状，
  与上面两条尺子推出的常数误差完全一致。
- 构建：`build_check\MapleStory.exe` md5 `6f0a8bc10be012bd…` 4,422,144 B，MSBuild EXITCODE=0
  （协议门禁 GATE OK，只剩既有 C4701/LNK4098/LNK4099 告警）。
- **真机复验未做**：exe 被运行中的客户端占用，部署方式（A 你关掉 / B 优雅退出 / C 中止）还没拍板
  ⇒ 见 `CHECKLIST.md` §I.3 第 1 条。复验判据 = `check ItemFull.json <部署后真机帧>` 跑到 8/8 Δ=0
  且 `full_grid_rows` 回到 6 行。

## 顺带定案的两条

1. **数字串探针改比「右缘 x+w」**（`layout.py` 新增派生键，`Item.json/meso_digit`、
   `ItemFull.json/meso_digit_full`、`SkillMacro.json/sp_digit` 三处同步）：金币 / SP 都是
   `Text::Alignment::RIGHT` 画在固定 x，位数一变左缘就漂、字形尺寸两端字体又不同 —— 旧探针比 `x` 和 `w/h`
   在真机上必然假 FAIL（实测原生 6x9 vs 真机 5x8）。
   ⚠ **本条第 3 句作废并撤回**：写「改前真机帧就已经 Δ=0」时，`layout.py` 的 `cmp_probe` 还在用
   `actual.get("x+w")` 直接取值 —— 派生键 `x+w` 根本不在字典里，两侧都取到 0，差恒为 0，**这条尺当时是空转的**，
   Δ=0 不是测量结果。2026-11-01 修好 `cmp_probe`（走 `keyval()`，并加「基键缺失即记 MISSING=999」）后重测：
   原生帧 3 处 `x+w` **真 Δ=0**（131+6 == 131+6；尺子成立）；但改前真机帧报 **`x+w` −1**
   （实测 5x8 ⇒ 右缘 136）。⇒ 当时把这条退回「未定案」，判据是「两帧末位数字可能不是同一个字形」。
   **现已定案（见下一节）**：部署后真机帧与原生帧的末位都是「0」（`meso-glyph-bearing-compare.png` 6x 放大并排），
   原生「0」墨宽 6 / 右 bearing 0，SimHei 11px「0」墨宽 5 / 右 bearing 1，右对齐绘制点同为 advance 右缘 x=137
   ⇒ **−1 是字体光栅差，不是落位差**，客户端没有常数能修它（要修只能逐字形补偿 = 禁止局部补偿）。
   处置：`x+w` 从 `y` 探针里拆成独立探针 `*_ink` 并给 `delta_tol=1`（`layout.py` 新增逐探针 Δ 容差），
   纵向 `y` 仍按 spec `tol=0` 严判 —— 用户报的「金币值偏下」是纵向，那一条必须零容差。
   B4 的「SP 右缘差 2px」同理拆出 `sp_digit_ink`，仍归 #84。
2. **能力窗「基本属性点」板不是悬浮件**：`Stat.json/basicStat_panel` 打在第二张原生帧
   `frames/native_stat_fullscreen.png`（全屏 1280x720，鼠标停在右侧地图上、离能力窗很远）上
   **agree=1.000、整窗 9/9 Δ=0**（锚点还是自动匹配的 (175,191)）⇒ 撤回「原生仅悬浮显示、客户端无条件常显是缺陷」
   这条判断，客户端现状与原生一致。仍**未知**的是它是否随职业/等级门控（两张原生帧都是 1 级新手，
   板上文案本身就是「新手 1 转前自动分配」的告示）；IDA 里查不到 `basicStat` / `UIWindow.img` 字符串
   （当前 IDB 不是游戏客户端），要定这一条需要**原生端一个 1 转以后角色的能力窗截图**。

## 部署后真机复验（2026-11-01 19:5x —— B2 判据达成）

部署：`build_check\MapleStory.exe` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，两边 md5 同为 `6f0a8bc10be012bd…`
（部署前目标目录里是旧的 `1d43ea66661f…`；锁已自然释放，没有结束任何进程）。
驱动链：`_r27_login.py`（启动+登录+进世界）→ `_postkey.py i` / `_postkey.py s` → `_r27_expand.py`（模板匹配反推窗口左上，点 BtFull 展开）→ `_cap2.py` PrintWindow。

| 帧 | 门禁 | 结果 |
| --- | --- | --- |
| `live-postfix-expanded.png` | `check ItemFull.json` | **9/9 不合格 0**：`full_grid_rows` 从改前 12 条双行回到 `[50,84,118,152,186,220]` Δ=0；6 颗 `disabled_*` 全部 Δ=(0,0) agree=1.000（改前齐偏 +1,+1）；`meso_digit_full` y Δ=0 |
| `live-postfix-folded.png` | `check Item.json` | **4/4 不合格 0** |
| `live-postfix-stat.png` | `check Stat.json` | **8/8（跳过 1）不合格 0**，含 `basicStat_panel` agree=1.000 Δ=0 ⇒ 板上真机也常显，与原生一致 |

完整读数：`live-postfix-check.txt`。

## 未验证的残留

- 物品图标 32x32 盒是否与格重合：disabled 覆盖层只能钉住「格」，图标另有 origin 语义（见 #89 B1d）。
- `disabled_labels[4]`（`Stat/Disabled{STR,DEX,INT,LUK}`，绘于 y=243/261/279/297）画在 basicStat 板
  （y=195..316）底下，新手态下必然被盖住 ⇒ 现状无害，但等上面那条门控定案后要重新看绘制顺序。
