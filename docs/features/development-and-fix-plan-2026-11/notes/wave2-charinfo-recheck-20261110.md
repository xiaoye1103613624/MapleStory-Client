# Wave2 2-14 · UICharInfo 子窗口稀疏 map 裸 at(i) 守卫（round46 销案）

> 2026-11-10 · 第 13 例同族病「主体路径有守卫、旁路分支漏守卫」· 纯本地 L-A 增量
> 沿用 m04292 一次性 Plan（`docs/features/development-and-fix-plan-2026-11/README.md` §4 Wave2），不重开 Plan。
> 全部结论以 live 源码 + 库层注释 + orange-wz / `.nx` 资源实证为准，未改协议/包体/WZ/IMG/XML。

## §0 性质与授权

- 授权：m04292「现有功能补充、错误逻辑修复… 可以自测的自己测试… 功能测试完成就提交代码，不能自测的记录下来后面我一起测试」。
- 范围：单文件 UI 绘制路径的确定性崩溃修复 + 离线 harness 复刻；真机 UI 观感项进 `pending-live-tests` §25。
- 约束：083/v83 only 禁 095；校准≠改回原生；s9 优先；数值重合≠语义重合；十一面校准文档只读。

## §1 缺陷逐字（事实链）

`IO/UITypes/UICharInfo.cpp:221 void UICharInfo::draw(float inter) const` 内三段画按钮逻辑**范式撕裂**：

| 位置 | 代码 | 判定 |
| --- | --- | --- |
| 主窗口 `:225-230` | `for (size_t i = 0; i < Buttons::BtArrayGet; i++) { auto it = buttons.find(static_cast<uint16_t>(i)); if (it != buttons.end() && it->second) it->second->draw(position); }` | find+判空 ✅ |
| `:305` BtBook | `if (auto it = buttons.find(Buttons::BtBook); it != buttons.end() && it->second) it->second->draw(position);` | find+判空 ✅ |
| 子窗 Collect `:278-280`（修复前） | `for (size_t i = Buttons::BtArrayGet; i < Buttons::BtFAQ; i++) if (const auto button = buttons.at(i).get()) button->draw(position);` | **裸 `buttons.at(i)`** ❌ |
| 子窗 Damage `:294-296`（修复前） | `for (size_t i = Buttons::BtFAQ; i < buttons.size(); i++) if (const auto button = buttons.at(i).get()) button->draw(position);` | **裸 `buttons.at(i)`** ❌ 且上界用 `buttons.size()`（元素个数）当 id 域 |

- `buttons` 声明 `IO/UIElement.h:265 std::map<uint16_t, std::unique_ptr<Button>> buttons;`（**稀疏 map**）。
- 构造 `IO/UITypes/UICharInfo.cpp:77-81 auto try_btn = [&](uint16_t id, nl::node src, Point<int16_t> pos = {}) { if (src) buttons[id] = std::make_unique<MapleButton>(src, pos); };` — **只在资源节点存在时插入**，缺节点即缺键。
- `IO/UITypes/UICharInfo.h:55-76 enum Buttons : uint16_t { BtClose, ... BtArrayGet, BtArrayName, BtFAQ, BtRegist, BtBook };` → `BtArrayGet=14, BtFAQ=16, BtBook=18`。
- 库层自证：`IO/UIElement.cpp:166-177 remove_cursor()` 注释 `// 防御：buttons map 中可能存在 operator[] 只读访问产生的空 unique_ptr 条目（见 UIMiniMap::update_buttons 历史 bug），必须判空后再解引用。`；`:179-192 send_cursor()` / `:111-116 draw_buttons()` / `:123-126 update()` 均判空。
- 触发门控：`IO/UITypes/UICharInfo.cpp:267 if (collect_enabled)` / `:289 if (damage_enabled)`；`show_bottom_window():731-749` 按 BtPersonality/BtCollect/BtDamage 置位。
- `git blame -L 278,296`：两处 6 行全为 `25347525 (Ryan Payton 2020-02-11)` 上游原生；主窗口 `:227-230` 已由 `16d01198 (xiaoye 2026-10-21)` 修成 find+判空、`:305` 由 `93accb4f` 加 find → 子窗两处未跟随修。
- 确定性：稀疏 map 对缺失 id 调 `at()` 抛 `std::out_of_range`（构造期/绘制期无 try/catch）→ 崩。非猜、非编译器误报。
- 证据优先级：`opcode-matrix > live 源码构造点/调用点 > 库层注释 > git blame`。

## §2 全族排除 + orange-wz 门禁 + .nx 回退

- 换族 `裸 .at()` + `.substr()`（round41 入口遮蔽 / 42 下溢 / 43 空串 UB / 44 裸 stoi / 45 整数除零 已闭环）：全仓 `.at(` 38 命中、`.substr(` 191 命中/37 文件。逐字排除：`Character/Player.cpp:573` count 守卫；`Gameplay/Combat/Combat.cpp:205` size 守卫；`Gameplay/MapleMap/Reactor.cpp:84,94` 已由 `set_state :111-142` / `destroy :144-167` 补洞修过；`IO/Components/EquipTooltip.cpp:755,898` 6 键恒填；`IO/UITypes/UIItemInventory.cpp` slotrange.at(tab) tab 恒 6 合法枚举；`IO/UITypes/UIStatusBar.cpp:547` 密集 map + operator[] 补洞；`IO/UITypes/UIWorldMap.cpp:229` 同 map 迭代键；`Net/Handlers/MapObjectHandlers.cpp:174` 是自定义位抽取 `MobPeek::at()` 非 std::map::at 且有 len 守卫。
- **否决候选**：`IO/UITypes/UIKeyConfig.cpp:615,619 item_icons.at(id)/skill_icons.at(id)` — `button_pressed` DEFAULT `:739-746` 漏 `load_icons()`，但 `get_keytype():1377-1387` 永不返 ITEM/SKILL，默认键无 ITEM/SKILL → `draw()` 分支走不到；与 round45 C4701 同口径「连多资源触发路径都没有」，不立项。
- orange-wz 门禁（AGENTS.md：先 MCP，解析异常才 `.nx` 回退）：python 直连 `http://127.0.0.1:10012/mcp` → `initialize` serverInfo `orange-wz-http-mcp v1.157.49`；`KEY_SPEC raw_len=65536 b64_len=87384 sha16=268a8407a47ee76c iv=TSPHKw==`（`tools/_gms_key.b64`）；`load_files(paths=[E:\MXD\BeiDou-Client-xy\Data\UI], key=GMS, exclusive:false)` → `{"loadedCount":1,"rootCount":1,"exclusive":false,"generation":1}`；`get_node_tree_json(rootPath=Data\UI, nodePath="UIWindow.img/UserInfo", maxDepth=2, autoParse:true)` → isError true、`IMG 解析失败: UIWindow.img`（GBK 原文留证，UTF-8 误读乱码）→ 命中合规回退；`unload_all` → `UNLOAD_OK`。
- `.nx` 回退（诚实可达性）：`python tools/nx_inspect.py list UI.nx 1 "UIWindow.img/UserInfo"`（`E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx` 86919577 bytes）逐字子节点含 `BtCollectionHide/Show`、`BtExceptionHide/Show`、`BtFamily/BtItem/BtParty/BtPet1..3/BtTrade/BtRegister/.../backgrnd..7`，**无 `character`/`collect`/`damage` 子节点、无 `BtArrayGet`/`BtArrayName`/`BtFAQ`/`BtRegist` 按钮名**；`UIWindow2.img/UserInfo` NOT FOUND。`IO/UITypes/UICharInfo.cpp:56-58` 走 `character=UserInfo`、backgrnd 存在 → 窗能开；但 collect/damage 资源子节点缺失 → 子窗按钮键稀疏，`collect_enabled`/`damage_enabled` 可由 `button_pressed :367-376` 置位。**诚实标注**：当前 shipped v83 包子窗循环不常进，但多资源包（带 collect/damage 节点）或裁剪包（缺部分按钮节点）下稀疏 map 裸 `at()` 是确定性抛异常；与 round45「多资源路径可达」同族、比 UIKeyConfig 否决候选强。不虚报「当前必崩」。

## §3 最小修复（1 file, 16 insertions(+), 4 deletions(-)）

两处子窗循环改回 `find+判空`（对齐 `:225-230` 既有范式），**循环边界一字不动**（`BtArrayGet..BtFAQ` / `BtFAQ..buttons.size()` 保留，不裁决 `buttons.size()` 上界语义）：

```cpp
// buttons 是稀疏 map（try_btn 只在资源节点存在时插入），裸 at(i) 对缺失 id
// 会抛 std::out_of_range；主窗口 :225-230 的 find+判空才是库层要求范式
// （IO/UIElement.cpp:168-176 注释明言「必须判空后再解引用」）。
for (size_t i = Buttons::BtArrayGet; i < Buttons::BtFAQ; i++)
{
    auto it = buttons.find(static_cast<uint16_t>(i));
    if (it != buttons.end() && it->second)
        it->second->draw(position);
}
```

- 选择 `find+判空` 而非 `if (buttons.count(i))`：与既有主体范式逐字一致，且 `it->second` 判空覆盖库层注释所述空 unique_ptr 条目。
- 合法构图（范围内键齐备）画到的按钮集合与修复前逐字一致；缺键/空指针键跳过而非抛异常。
- 不改协议/包体/WZ/IMG/XML；不扩大改动面到 `buttons.size()` 上界的语义裁决（属 L-B / D1–D14 类）。

## §4 自测与门禁（实证）

- `tools/test_charinfo_button_guard.py`：`PYCOMPILE_OK` / `STATIC GUARDS: 13 checked, 0 failed` / `COUNT GUARDS: 2 checked` / `PASS: 13 static guards + 2 count guards + 13 scenarios (before THROW 6 -> after 0; regression 7/7)` / `TEST_RC=0`。13 场景 = 6 异常（collect 缺 BtArrayGet/BtArrayName/两键全缺、damage 缺子窗键三种）+ 7 回归（键齐备 / 带空 unique_ptr / 空 map / 当前 shipped 包范围空）。期望值全部由 SCENARIOS 自身数据推导（round43 脚本自身死值断言教训）。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`。
- `git diff --check HEAD` → `DIFFCHECK_CLEAN`。
- `tools\build_smoke.bat`（`MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c`）：`BUILD_RC=0` + `GATE OK` x2 + `EXITCODE=0` + `ERROR_LINES 0`；`build_log.txt` `2026-11-10 03:29:29 / 9426B` → `2026-11-10 04:00:15 / 5256B`（真重编译）；`taskkill` 34476/6108/56016 `Access is denied`（32K 空壳历史同款，不阻断）。

## §5 范围边界

- 只改 `IO/UITypes/UICharInfo.cpp` 两处 `draw()` 子窗循环；循环上界、按钮枚举、资源加载、`show_bottom_window` 状态机一字未动。
- `buttons.size()` 当 id 上界对稀疏 map 语义本就存疑，但本轮按最小 L-A 处理「确定性抛异常」，不裁决上界正确性（登记后续台账）。
- 未触及 `IO/UITypes/UIKeyConfig.cpp` 否决候选（无可达触发路径）、未触及 `.substr()` 族其余命中。

## §6 真机 backlog（进 `pending-live-tests` §25）

25-1 当前 shipped 包主窗口/BtBook/人物信息渲染回归；25-2 带 collect/damage 资源包的子窗按钮渲染；25-3 裁剪/畸形资源包（缺部分按钮节点）不闪退；25-4 `buttons.size()` 上界语义是否另立裁决（需真机观察 damage 子窗按钮是否画全）。

## §7 提交 scope（精确 add，零署名）

`IO/UITypes/UICharInfo.cpp` / `docs/features/development-and-fix-plan-2026-11/README.md`（2-14 行）/ `docs/features/development-and-fix-plan-2026-11/notes/wave2-charinfo-recheck-20261110.md` / `docs/features/pending-live-tests/README.md`（§25）/ `tools/test_charinfo_button_guard.py`。commit `#devfix-charinfo-buttonguard`。

## §8 结论

第 13 例同族链 round34→46 闭环：UIWorldSelect → UICharSelect → Textfield → UINpcTalk → UIRaceSelect → UIGuildBBS → MOVE_SUMMON → 入口遮蔽 → 无符号下溢 → 空串 UB → 裸 stoi → 资源驱动整数除零 → **稀疏 map 裸 at()**。全部以 live 源码逐字证据 + 离线 harness + GATE + build 真重编译交付，当前 shipped 包可达性诚实标注、不虚报。
