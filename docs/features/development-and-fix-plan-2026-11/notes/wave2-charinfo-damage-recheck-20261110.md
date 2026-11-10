# Wave2 2-15 · UICharInfo damage 子窗循环上界销案（round47）

- 提交：`#devfix-charinfo-damage-bound`
- 代码：`IO/UITypes/UICharInfo.cpp`（1 文件，4 insertions / 4 deletions）
- 自测：`tools/test_charinfo_damage_bound.py`（14 静态守卫 + 13 场景 PASS）
- 门禁：`GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)` + `build_smoke.bat` `EXITCODE=0`，04:38 真重编译
- 台账出处：`docs/features/pending-live-tests/README.md` §25-4（round46 登记为「L-B 待真机裁决」）

---

## 0. 性质与授权

- 授权：m04292（用户授权本轮客户端开发/修复：可改源码、可自测、测过即提交、客户端占用直接关闭、不能自测的记录到 backlog）。
- 性质：**纯本地 L-A 缺陷**。判定只依赖 live 源码里的枚举定义、稀疏 map 填充范式、以及循环上界与键域的数学关系，不涉及协议、包体、WZ/IMG/XML 内容，不需要读服务端。
- 与 round46 的关系：round46（`#devfix-charinfo-buttonguard`，`9edf89e0`）修的是同一 `draw()` 里子窗口的**裸 `buttons.at(i)` 抛异常**，并刻意**一字不动**保留原循环上界 `i < buttons.size()`，把上界语义作为 §25-4 留给真机裁决。
- 本轮为何直接销案：`buttons.size()` 是**元素个数**，而子窗按钮的键域是**枚举区间**；两者仅在「键域连续填满且无域外键」时巧合相等。这是可从源码直接证伪的数学错误，不依赖任何运行时观测，因此不再挂 L-B。

---

## 1. 缺陷逐字

### 1.1 按钮枚举与键域

`IO/UITypes/UICharInfo.h:55-76`：

```cpp
enum Buttons : uint16_t
{
	BtClose,      // 0
	BtCollect,    // 1
	BtDamage,     // 2
	...
	BtFriend,     // 12
	BtVisit,      // 13
	BtArrayGet,   // 14
	BtArrayName,  // 15
	BtFAQ,        // 16
	BtRegist,     // 17
	BtBook        // 18
};
```

三段键域（`draw()` 里的分段循环）：

| 段 | 区间 | 上界写法 | 状态 |
| --- | --- | --- | --- |
| 主窗口 | `[0, BtArrayGet)` = 0..13 | `i < Buttons::BtArrayGet` | 正确（枚举分段） |
| Collect 子窗 | `[BtArrayGet, BtFAQ)` = 14..15 | `i < Buttons::BtFAQ` | 正确（枚举分段） |
| Damage 子窗 | `[BtFAQ, BtBook)` = 16..17 | **`i < buttons.size()`** | **缺陷** |
| BtBook | 18 | 单独 `buttons.find(Buttons::BtBook)` | 正确 |

### 1.2 修复前现场

`IO/UITypes/UICharInfo.cpp:294-296`（round46 修复 `at()` 之后、本轮修复上界之前）：

```cpp
for (size_t i = Buttons::BtFAQ; i < buttons.size(); i++)
{
	auto it = buttons.find(static_cast<uint16_t>(i));
	if (it != buttons.end() && it->second)
		it->second->draw(position);
}
```

`buttons` 是稀疏容器：`IO/UIElement.h:265 std::map<uint16_t, std::unique_ptr<Button>> buttons;`。构造函数 `IO/UITypes/UICharInfo.cpp:77-81` 的 `try_btn` lambda 只在**资源节点存在**时插入键，因此 `buttons.size()` 与任何键域都没有数量关系。

### 1.3 三段错误（纯代码可证）

设 damage 子窗实际插入了 `k` 个键，`size` 为全 map 元素数：

1. **`size < 16` → 循环一次都不跑**：`i = 16` 时 `16 < size` 为假，`BtFAQ`(16) / `BtRegist`(17) 即使都在 map 里也**全部漏绘**。这是最常见的退化解（裁剪包、缺主窗按钮节点都会压低 `size`）。
2. **`size == 17` → 只跑到 `i = 16`**：`BtRegist`(17) 漏绘。
3. **`size >= 19` → 多跑 `i = 18`**：`BtBook` 在 `:317-318` 已被**单独绘制**，此处造成**同一帧重复绘制两次**；`size` 更大时还会空转到更外层的键上。

happy-path（`size == 18`，即键域 0..17 连续无缺口）前后**逐字等价**，所以 round46 的回归测试 7/7 全过也没暴露这条缺陷——这正是「主体路径正常、旁路分支漏守卫/漏同步」的同族特征。

---

## 2. 排除的假阳性（避免误改）

本轮此前已全量核过一批相邻族，均判定**不立项**，记录在此防止后续重复扫：

| 族 | 结论 |
| --- | --- |
| `.substr()` 越界（229 命中） | `substr(find+1)` 天然安全（`npos+1` 回绕成 0）；真正会抛的 8 个 SoftKey tip 解析点全有 `npos` 判 + `try/catch`；`NpcText.cpp:54/86/119` 是注释里的死代码 |
| 空 `std::function` 裸调用（50 命中 / 32 heuristic） | `fadeprocedure()` 包 try/catch；`ontext`/`onreturn`/`callbacks.at`/`oncomplete` 全有守卫；`Slider` 默认构造的空 `onmoved` 在 24 个 UI 交互前都被带 lambda 的 `Slider(...)` 覆盖；`Combat` 三队列构造即给 lambda → 无 `bad_function_call` 可达路径 |
| `Mob::set_stance` 缺键 `.at(stance)` | 构造 `:67-101` 无条件插入 STAND/MOVE/JUMP/HIT/DIE；`uint8_t` 版 `:204-205` 有 find 回退；`Stance` 版实参只来自守卫后的 byte / map 自集 slots / 五个常量 → 缺键不可达 |
| `Optional<T>` 裸 `->`/`*`（76 处） | 全量逐字读 10 段，全部 `if (opt)` 守卫 |
| `UIKeyConfig::draw` 的 `item_icons.at(id)` | `get_keytype():1377-1387` 永不返 `ITEM`/`SKILL`，默认键无这两类 → 分支走不到 |
| `MessagingPackets.h:52` C4267 `write_byte(recipients.size())` | 收件人上界受好友 `capacity` 约束（0/25 台账），需真机 + s9 读法才能定，属协议线，本轮不碰 |

---

## 3. 最小修复

`IO/UITypes/UICharInfo.cpp:300-303`：

```cpp
// 同上：稀疏 map 用 find 判空。damage 子窗键域是 [BtFAQ, BtBook)（BtFAQ=16 /
// BtRegist=17），与主窗 [0,BtArrayGet)、collect [BtArrayGet,BtFAQ) 同为枚举分段；
// 原上界 buttons.size() 是元素个数而非键域，缺资源时漏绘子窗按钮、键齐时又重复绘 BtBook(18)（:317 单独绘）。
for (size_t i = Buttons::BtFAQ; i < Buttons::BtBook; i++)
```

- 只改**上界一个表达式**：`buttons.size()` → `Buttons::BtBook`。循环下界、循环体、`find` 判空、`position` 参数一律不动。
- 与主窗口 `:225-230`、Collect 子窗 `:281-286` 的写法**完全同构**，代码里三段键域从此都是枚举分段。
- `BtBook`(18) 继续由 `:317-318` 单独绘制，重复绘制路径消失。
- 不动协议、包体、WZ/IMG/XML、不动 `UICharInfo.h` 枚举。

### 3.1 同时改写 round46 的旧注释

round46 的注释写的是「保留原循环边界不改动可画到的按钮集合」——那是当时**刻意留待裁决**的措辞，本轮已裁决该上界是错的，注释若继续留在源码里会误导后来人，故一并改写为「为什么原来的写法是错的」。

---

## 4. 自测与门禁

### 4.1 harness

`tools/test_charinfo_damage_bound.py`：

- **14 项静态守卫**：新上界 `i < Buttons::BtBook` 在位、旧 `buttons.size()` 上界已移除、注释含三段键域说明、`find` 判空保留、主窗/Collect 仍是枚举分段、`BtBook` 单独绘、`BtArrayGet/BtFAQ/BtRegist/BtBook` 枚举名存在。
- **枚举解析**：从 `UICharInfo.h` 现场解析 `Buttons` 枚举，断言 `BtArrayGet=14 / BtFAQ=16 / BtRegist=17 / BtBook=18`——防止源码枚举被改动后 harness 仍假绿。
- **13 个场景**：模拟 `damage_drawn(keys, bound)`，`BEFORE` 用 `bound=len(keys)`（复刻原 `size()` 语义），`AFTER` 用 `bound=BtBook`。

实跑输出：

```text
STATIC GUARDS: 14 checked, 0 failed
ENUM VALUES OK: BtArrayGet=14, BtFAQ=16, BtRegist=17, BtBook=18
SUMMARY bad_before=9 bad_after=0 dupBtBook_before=3 dupBtBook_after=0 regression=4/4
PASS: 14 static guards + 13 scenarios (before BAD 9 / BtBook-dup 3 -> after 0; regression 4/4)
TEST_RC=0
```

关键场景：

| 场景 | size | before 绘 | after 绘 | 判定 |
| --- | --- | --- | --- | --- |
| S2 sparse-14keys | 14 | `[]` | `[16,17]` | before 全漏 |
| S4 sparse-17keys | 17 | `[16]` | `[16,17]` | before 漏 17 |
| S5 exact-18keys | 18 | `[16,17]` | `[16,17]` | **逐字等价（happy-path 回归）** |
| S9 only-faq-1key | 1 | `[]` | `[16]` | before 漏 |
| S11 btbook-no-regist | 19 | `[16,18]` | `[16]` | before 重复绘 18 |
| S12 extra-key-20 | 20 | `[16,17,18,19]` | `[16,17]` | before 越域 |

### 4.2 门禁与构建

```text
GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)
DIFFCHECK_CLEAN
BUILD_RC=0 / EXITCODE=0
```

`build_log.txt` 从 5256B 更新（04:38:46），首行即 `UICharInfo.cpp` 编译记录，尾行 `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`，`ERROR_LINES 0`。`MapleStory.exe` mtime `2026-11-10 04:38:46 / 4928512B` → 真重编译。

### 4.3 harness 自身两处 bug（已就地修，不计任务失败）

1. `enum_values()` 按逗号切分时把 `enum Buttons : uint16_t {` 当成第一个 token，导致全体编号 `-1`（`BtArrayGet=13`）。修法：定位 `{` 之后再切分。
2. S11 场景把「19 个键里缺 BtRegist」标成 `before_ok=False`，但 `bound=size()` 时 `i<19` 只跑到 18、且该键集无 20 号键，实际 `drawn=[16]` 与期望一致。修法：改为 19 键含 `BtBook` + 域外填充键 21，使 `before` 真的越域。

两次都是**测试脚本自身逻辑错误**（与 round43 的元组解包、round46 的场景标注同类），不是修复方案失败，不记入任务失败。」

---

## 5. 范围边界

- 只改 `IO/UITypes/UICharInfo.cpp` 一个表达式 + 一段注释。
- 不改 `UICharInfo.h` 枚举值（改枚举会动协议键位映射）。
- 不改主窗口 `[0, BtArrayGet)` 与 Collect `[BtArrayGet, BtFAQ)` 两段（本来就是对的）。
- 不改 `BtBook` 的单独绘制（`:317-318`）。
- 不碰 `damage_enabled` / `collect_enabled` 的置位逻辑与 `show_bottom_window()`。

---

## 6. 真机联测（进 `pending-live-tests` §26）

harness 只复刻「上界 × 键集」的算法，**不能证明真机渲染正确**。以下必须用户一起测：

1. shipped v83 包打开人物信息窗、切 damage 子窗 → 与修复前视觉一致，无空白/闪退。
2. 含完整 damage 子窗节点（`BtFAQ` / `BtRegist`）的资源包 → 两个按钮都渲染出来。
3. 裁剪包（缺 damage 子窗按钮节点）→ 不闪退，缺的键跳过，其余照常。
4. `BtBook` 只画一次、不出现重影。

---

## 7. 提交 scope

```text
IO/UITypes/UICharInfo.cpp                                              (M)
tools/test_charinfo_damage_bound.py                                    (A)
docs/features/development-and-fix-plan-2026-11/README.md               (M, §4 追加 2-15)
docs/features/development-and-fix-plan-2026-11/notes/wave2-charinfo-damage-recheck-20261110.md (A)
docs/features/pending-live-tests/README.md                             (M, 追加 §26 + §25-4 销案)
```

零署名，一个提交只装这一件事。

---

## 8. 证据优先级

`opcode-matrix` → live 源码枚举定义/循环上界 → 稀疏 map 填充范式（`try_btn` + 库层注释） → harness 场景复刻 → git blame → 旧台账。

本轮结论全部落在前三级：枚举值现场解析、上界表达式现场读、`try_btn` 只在节点存在时插键。台账（§25-4）只是触发入口，不作为判定依据。
