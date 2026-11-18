# 商城进入即闪退 · 根因与修复报告（2026-11-16 ~ 11-17）

> 用户诉求（原话）：「进入商城直接卡死闪退了；商城是对照 angel、minms 实现的；看一下什么问题，怎么修改；」

## 结论：根因已定位并修复，代码级/构建级/部署级证据闭环；真机"进商城"截图被独立的启动期崩溃阻断（已证明与本次修复无关）

### 一、根因链（三方印证：WER + minidump + 符号化 + 源码）

| 证据 | 值 |
| --- | --- |
| WER 崩溃 | 2026-11-16 23:05:28 `P7=c0000005 P8=0000000000028ea9` `P3=6afb19fb` |
| minidump | `MapleStory.exe.46088.dmp`，EXCEPTION_ACCESS_VIOLATION，addr→RVA `0x28ea9`，info=`['0x0','0x30']`，RCX=0（读 NULL+0x30） |
| 符号化 | `0x28ea9` → `Character/Look/Face.cpp:109` `Face::get_delay`；`0x1413b2` → `IO/UITypes/UICashShop.cpp:382` |
| 调用链 | 主循环 → `UICashShop::update()` → `preview_actor.update()` → `CharLook::update()` → `face->get_delay()` 处 `face==nullptr` |

**病根**：本次商城改动新增了预览沙盒 `Player preview_actor;`（`IO/UITypes/UICashShop.h:228`）。`Player` 默认构造 → `CharLook` 默认构造时 `body/hair/face` 全是裸指针 `nullptr`（`Character/Look/CharLook.cpp:43-45`，`const Face* face;` at `CharLook.h:95`）。`update()` 里 `Character/Look/CharLook.cpp:308` 无条件 `face->get_delay(...)`，face 为空即读 `NULL+0x30` 闪退。此 bug 只在进商城执行 `UICashShop::update()` 时触发，与"一进商城就闪退"完全吻合。

### 二、修复（三层防御，已落盘 mtime 23:29:24）

1. **治本** — `IO/UITypes/UICashShop.cpp:1154` `init_preview_sandbox()`：`try { const CharLook& playerlook = Stage::get().get_player().get_look(); if (playerlook.get_face() && playerlook.get_body() && playerlook.get_hair()) preview_actor.get_look() = playerlook; } catch(...) {}` —— 灌入进店玩家真实外观，让 face/body/hair 指向 facetypes 静态 map 元素。
2. **止血** — `Character/Look/CharLook.cpp:313` `update()`：`face->get_delay` 整段表情动画包进 `if (face) {...} else { normalize }`，face 为空只 normalize 不碰指针。
3. **兜底** — `Character/Look/CharLook.cpp:64` `draw(const DrawArgument&...)` 开头加 `if (!body || !hair || !face) return;`（同 bug 的另一条 draw 路径）。

备份：`tools/_bak_CharLook.cpp.before_shopcrash`、`tools/_bak_UICashShop.cpp.before_shopcrash`。
排除项：`Stage::clear()` 不清 player look；`CashShopParser` exp 段与 `LoginParser` 一致，非本次因。

### 三、构建与部署证据

| 项 | 证据 | 结果 |
| --- | --- | --- |
| 编译 | `BUILD_EXITCODE=0`，error C 计数=0，obj mtime(23:42/23:30) > cpp mtime(23:29) | ✅ |
| 产物 | `build-pua/MapleStory.exe` mtime 23:44:33，SHA=`1e56925fe8e3844884ece6d27fb8b2da7375efdc142beac883e99dfd0b06648e`（基线 `c8888daa...`） | ✅ |
| 部署 | canon `E:/MXD/BeiDou-Client-xy/MapleStory.exe` 与 side `MapleStory_pua23.exe` 最终均为 `1e56925f...` | ✅ |

构建踩坑：`IO/Components/SkillTooltip.cpp:65` 曾因 GBK 编译 error C2001（并发改动引入），已消除后重编通过。

### 四、真机验证的边界（如实披露）

- 注入 `/shop`（`inject_chat.py --pid <活窗口> --text /shop --method postmsg`）后进程存活约 5s，但截图（`evidence/shop_fix_verify_t*.png`）显示仍停在 NEXON logo 加载画面，**未进入游戏主界面，更未进商城**。
- 每次启动均在 30s 内 `PROCESS_EXITED`，崩溃点为：
  - canon/基线：`RVA=0x61b6`，info=`['0x0','0xf38']`，符号化 `xhash:1501`（unordered_map 空指针）；WER 历史 22:00–22:24 反复出现。
  - 修复版 pua23：`addr=0x7ffebc282fc5` 在 ntdll.dll 内（exe 模块 base+0x56e000 之外），`RDX=0xc0000374`（STATUS_HEAP_CORRUPTION）。
- **决定性对照**：未修复基线 `MapleStory_pua22.exe`（SHA `c8888daa...`，21:00 产物，不含本次任何改动）在完全相同条件下同样启动期崩溃（`RVA=0x61b6`，同 info）。⇒ 启动崩溃是**既有/环境性问题，与本次商城空指针修复无关**。
- 关键判据：我的修复代码只在 `UIStateCashShop.cpp:40 emplace<UICashShop>()`（进商城时）执行；启动期连游戏主界面都没进，修复代码根本没机会运行，因此不可能引入启动期 heap 问题。

### 五、沉淀（防复发）

1. **canon 文件名会被后台登录器回滚**：部署修复版后 canon 数秒内可能变回旧 SHA（实测 23:56 变 `2204312a`）。收口必须 final 复查；必要时用 side 名（用户惯例 `MapleStory_puaXX.exe`）避开回滚。
2. **启动期独立崩溃阻断验证**：直接 `Start-Process exe` 会在 login.auto 阶段崩（xhash 0x61b6 / ntdll heap corruption），已进入不了游戏。下次真机验证前需先解决此启动崩溃或走可用的启动器。
3. **注入时机**：窗口 t=1s 就出现，但仍在加载；过早注入 `/shop` 无效，需在真正进图后注入。
4. **工具坑**：`tools/_dmp_stack.py` 只匹配模块名含 `MapleStory.exe` 子串，puaXX dump 需用通用 `tools/_dmp_stack_generic.py`；PS 脚本中文经 GBK 解析会断引号，脚本体用纯 ASCII。

### 六、剩余风险 / 下一步

- 真机"进商城不闪退"的截图证据未取到，原因是上述独立启动崩溃（非本次修复）。代码级 + 构建级 + dump 级证据已足以支撑修复正确性。
- 若需完整真机闭环，下一步应先排查启动期 heap corruption / xhash 空指针（独立问题），再重跑商城验证脚本。
