# 批次实现约定（W1–W12 通用，先读这一页再动手）

> 本文件是给「逐窗校准」批次的统一口径。违反任一条 = 该批不可验收。

## 1. 事实基线（勿再猜）

- 权威节点清单：[asset-node-inventory.md](asset-node-inventory.md)。**只有里面标 ✓ 的件存在**；标 ✗ 的件不许再用（会静默变透明）。
- 只读取证：`python tools/nx_inspect.py list "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" <depth> <path>`
  - `origin [vector] (x, y)` 现已修好（PKG4 向量内联解码），**可直接当权威 anchor**。
  - 禁止写 `.img` / `.nxcache`，禁止 `save_as`。
- 参考截图（原生正确版，1:1 像素裁切，可直接量坐标）：`C:\Users\11036\Desktop\正确客户端截图\`
- 本机 Python stdout 是 cp936：**打印中文/符号会 UnicodeEncodeError**。度量结果一律写 UTF-8 文件再读。

## 2. 已就位的公共件契约（W0，已构建通过）

| 件 | 位置 | 契约 |
| --- | --- | --- |
| `UiAssetResolve::window(name)` | [Util/UiAssetResolve.cpp](../../../Util/UiAssetResolve.cpp) | `UIWindow2.img[name]` 优先，回退 `UIWindow.img[name]`；缺失时写一条 `diag_once` |
| `UiAssetResolve::window4(name)` | 同上 | 再多试 `UIWindow4.img` |
| `UiAssetResolve::usable(node)` | 同上 | **真判定**：有子件 或 自身是 bitmap；present-but-empty 空壳判 false |
| `UiAssetResolve::diag_once(tag,msg)` | 同上 | 追加一行到运行目录 `_ui_asset_diag.log`，同 tag 只写一次（构造期探针安全） |
| `ms::try_maple_btn(buttons, id, src, position, site)` | [IO/Components/MapleButton.h](../../../IO/Components/MapleButton.h) | **`position` = 贴图左上角（不是 anchor）**：内部把 `normal/0` 的 origin 加回去。资源缺失 或 未传坐标 → **不创建** + 一行 diag。首参哨兵 `maple_btn_nopos()` |
| `ms::MapleButton(src, point)` | 原生构造 | `point` 是 **anchor**，绘制时 `draw = pos - origin`（见 `DrawArgument::get_rectangle` 的 `rlt = pos - center - origin`） |
| `ms::Slider(...)` | [IO/Components/Slider.cpp](../../../IO/Components/Slider.cpp) | 非 CHATBAR 时 `Basic.img/VScr<type>` 不存在 → 钳到 `Basic.img/VScr` |

⚠ 因为 `try_maple_btn` 现在**按左上角**解释坐标，凡是它已有的调用点（UIKeyConfig、UISkillBook、UIStatsInfo、UIItemInventory）都在 W6–W10 里复核重算，不许留旧数。

## 3. 坐标推导优先级（按顺序，够用就停）

1. **烤进 backgrnd 的不要另画**：v83 窗体标题/槽位标签是烤在整张背景里的（例 `Item/backgrnd` 175×289 自带「现有道具」蓝条；`Equip/backgrnd` 175×304 自带「角色装备」+ 全部槽名）。→ 这类文字**禁止**再用 `Text`/`Font` 重画一遍（会变糊/双影）。
2. **角锚算术**：按钮尺寸已知（节点 `WxH`）+ 窗体已知（`backgrnd` 的 `WxH`）→ 右下角钮 `x = W_bg - w - k`。k 用截图量一次并记进 evidence，同窗复用。
3. **截图实测**：仅在前两条不足时用。做法：以 `backgrnd` 在截图里的左上角为窗口原点，量控件可见边的相对坐标；把「截图绝对坐标 → 窗口相对坐标 → 传给 `try_maple_btn` 的左上角」三步算式写进 evidence，允许复核。
4. 位图字库（`StatusBar.img/number` 5×7、`Basic.img/ItemNo` 8×11、`LevelNo`+`ShowLevel`）用 `Charset` 画；数字串含 `[ ] % /` 时要用 `Lbracket/Rbracket/percent/slash`。

## 4. 改动纪律

- **只改本批授权的文件**；需要碰别的文件 → 写进回报，不要擅自改。
- **不构建、不 MSBuild**（并行会撞 obj；由主线统一构建）。不部署、不覆盖 exe、不驱动客户端。
- 不改服务端、不动封包 opcode、不写库。
- 注释：只在「为什么/约束/依据」非显而易见时写一行；节点路径与尺寸这类依据**写进 evidence 文件**，不要塞满代码注释。禁止多段 docstring。
- 临时调试日志（`[DBG-...]` / 逐 tick 打印）不许留；需要观测就用 `UiAssetResolve::diag_once`（它天然只写一次）。
- 现有功能语义（点击发送的封包、收包驱动的数值）不许顺手改；版式问题只碰显示层。

## 5. 交付物（每批都要）

1. 代码改动（最小面）。
2. `docs/features/ui-v83-window-restore/evidence/wNN-<窗口名>/coords.md`：
   - 表格：控件 → 节点路径 → 节点 `WxH` + `origin` → 计算出的窗口相对左上角 → 依据（1/2/3 哪条）
   - 显式列出「本包没有、已放弃或预留」的件。
3. 回报正文 ≤150 字：改了哪些文件、关键判据、**未决阻塞**（若有）。
