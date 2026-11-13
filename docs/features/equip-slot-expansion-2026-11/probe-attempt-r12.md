# Round 12：探针验证的失败与一次源码事故

> 目标：用文件探针补上 Round 11 缺的「UI 是否真的打开」判定。
> 结果：**探针验证失败**（键到不了游戏循环），且**我在清理探针时误删了源码 7 行**，
> 已完整恢复并重编验证。本轮零功能交付，但三条结论都是硬的。

## 1. 先修正 Round 11 的一个测试设计错误

Round 11 的「按 U 两次看对称性」测试**从设计上就是错的**。
证据：`IO/UITypes/UIEquipInventory.cpp:426`（`send_key`）—— 当窗口**已激活**时，
`U` 走的是「卸下选中装备 / 弹 SoftKey」分支，**不是关窗**。关窗要走 `ESC`
（同函数 `:424` → `toggle_active()`）。

所以 Round 11 观察到的「再按 U 差异不回落」不是「窗口没开」，而是「第二次 U 干了别的事」。
**该轮结论里「对称性失败 ⇒ 注入无效」的推论不成立**（虽然最终「无需自动化」的整体判断仍然成立）。

## 2. 探针方案与结果

加了两处文件探针（Release 下 `LOG` 是 no-op，只能写文件）：

| 探针 | 位置 | 作用 |
|---|---|---|
| `[PROBE-SLOT]` | `UIEquipInventory::draw()` 首次调用 | 证明窗口被绘制 + 输出 10 个关键槽位坐标与 itemid |
| `[PROBE-CTOR]` | `UIEquipInventory` 构造函数 | **正对照**：证明探针文件机制本身可用 |

真机执行（已确认进图：标题 `【1-Admin】【132-暗之黑骑士】【在线时长 00:00:12】`）：

```
hwnd=41292250  fg==target True  clicked(1280,772)  sent U x2
=== PROBE LOG ===  NO PROBE LOG AT ALL
```

**连正对照 `CTOR_CALLED` 都没有** —— 这排除了「`draw` 没被调用」，直接证明
**`keybd_event` 注入的键盘事件到不了 SDL 的输入路径**（窗口类 `SDL_app`，
Round 11 已证 `PostMessage` 投递成功但 `SendMessage` 返回 0）。

结论：**本客户端的 UI 状态无法被外部进程自动化验证**，三条通道（PrintWindow / ImageGrab / 消息注入）+ 本轮的文件探针路径全部堵死。

## 3. 源码事故与修复（必须记录）

**事故**：清理探针的删除脚本按「行号区间 + 缩进猜块边界」删代码，误删了
`UIEquipInventory` 构造函数开头 7 行：

```cpp
nl::node close = basic_close_btn();
nl::node Equip = UiAssetResolve::window4("Equip");
if (!UiAssetResolve::usable(Equip)) { active = false; dimension = Point<int16_t>(0,0); return; }
```

后果：`error C2065: "close": 未声明的标识符` 等 6 个错误，编译失败。

**根因**：两次删除尝试中，第一次的正则把 ctor 探针删了但 draw 探针没删；第二次
按行号删 draw 探针时，行号已经被第一次删除改变，且我用「`\t\t}` 结尾」猜块边界，
实际缩进层级不同 → 切到了构造函数。

**修复**：整文件从 `git show HEAD:` 恢复，再**精确重放**本工程对该文件的 4 处真实改动：

| # | 改动 | 校验锚点 |
|---|---|---|
| A | `SUBWEAPON` 坐标行 → 同值别名注释 | ① 说明 |
| B | 新增 `PENDANT2=(38,98)` / `SHOULDER=(71,230)` | ②③ 坐标 |
| C | `modify()` 改 `from_wire_bodypart()` | 符号归一化 |
| D | `EquipIcon(EquipSlot::Id s)` | 类型安全 |

修复后校验 5 项锚点全 OK、`PROBE` 残留 = 0，单 TU 编译 `EXIT=0`，
整工程 Rebuild `EXIT=0`，产物 5,026,304B（与 Round 9 干净版同尺寸）。

**教训（已写入记忆）**：按行号区间删代码前必须用**内容锚点**校验首尾，
不能靠缩进猜块边界；改动源码前先 `git show HEAD:` 留底，出问题能整文件回滚。

## 4. 顺带修正一个门禁误报

`verify_no_conflict.py` 报「候选首位 Heart 版本小于 Weapon，会读到残缺数据」×3。
实测证伪：

| itemid | Heart | Weapon | 实际差异 |
|---|---:|---:|---|
| 01672059 | 1206B | 1262B | 节点树**完全相同**，仅编码差 56B |
| 01672063 | 1206B | 1262B | 同上 |
| 01672096 | 1224B | 1262B | 两边都有 icon/iconRaw，27×30 vs 26×30（不同美术版本） |

**体积不是完整性的代理**。判据改为「候选首位那份必须包含 `icon` 节点」——
缺了才算客户端真的读不到图标。改后 19 个多版本 itemid 全部判为「读取确定」。

## 5. 本轮验收

| 项 | 结果 |
|---|---|
| 整工程 Rebuild | **EXIT=0**，产物 5,026,304B |
| 门禁套件 | **7/7 PASS** |
| live 部署 | 干净版（无探针），SHA `BABF63B98B92ABB1` |
| 源码状态 | 仅含本工程 4 处真实改动，`PROBE` 残留 0 |
| 临时脚本/日志 | 已清理（8 个 .py + probe log） |

## 6. 仍然待办

②③④⑤⑥⑦ 的**视觉**确认需要人按一次 `U`（装备栏默认键，`UIKeyConfig.h:151`）。
这是本客户端 UI 自动化验证的硬边界，不是没做。
