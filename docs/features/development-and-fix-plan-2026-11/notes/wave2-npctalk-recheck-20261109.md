# Wave2 2-3 · UINpcTalk SENDSIMPLE 键盘导航可视带守卫 · 现场复核（2026-11-09 round37）

> 性质：**按 m04292 授权的源码修复闭环**（非校准文档）。校准十一面文档仍只读。
> 味道：🟠 阿里味。诊断先行、证据闭环、不放宽验收标尺。

## 0. 结论

1. **orange-wz 门禁本面真跑闭合**：结构层 `Data/UI` 枚举到 20 个真 `.img` 子节点（WORKS）；内容层 `Basic.img`/`UIWindow.img` 的 `get_node_tree_json(autoParse:true)` 与 `search_node(keyword="UtilDlgEx")` 共 4 次返回 `IMG 解析失败`（FAILS）→ 命中 AGENTS.md 写死的回退条件，原始报错逐字 + 65536B GMS key 规格留证后合规回退 `.nx`（`UI.nx` 的 `UIWindow.img/UtilDlgEx` 控件表 24 项）。**不是「绕过 orange-wz」，是先跑后按规则降级**。
2. **真缺陷（非旧账、非假缺口）**：`UINpcTalk::send_key` 的 SENDSIMPLE 分支 UP/DOWN 只在全量 `simple_choices` 上挪 `simple_selected`，不动 `offset`，也不做可视带裁剪；而 `draw()` 与 `send_cursor()` 都按 `show_slider` 可视带裁剪。长选项菜单（拍卖行 9900001 等）键盘把高亮移到滚出可视带的行 → 选中虚线不画、鼠标点不到，但回车 `dispatch_simple_selection()` 仍会**提交一个不可见项**。这是 round34 `UIWorldSelect` / round35 `UICharSelect` / round36 `Textfield` 之后第四处「主体路径有守卫、旁路分支漏守卫」同族病。
3. **最小修复**：仅在 `send_key` SENDSIMPLE 分支插入 `ensure_visible()` lambda（与 draw/send_cursor 同源几何判据），UP/DOWN 各调一次；滚动 `offset` 把选中项拉回视野，并用 `slider.setrows(offset, unitrows, rowmax)` 同步滑块 thumb（该重载只回写 `row`、不触发 `onmoved`，避免二次回调）。`git diff` = 单文件 **45 insertions(+)**，不改协议、不改包体字节、不碰 WZ/IMG/XML。
4. 自测 `tools/test_npctalk_simple_visible.py` **PASS**（7 静态守卫 + 10 场景，修复前落到可视带外 16 次→修复后 0，no-slider/边界回归逐项一致）；`python tools/gen_opcode_matrix.py --check` → `GATE OK`；`tools/build_smoke.bat` → `GATE OK` + `EXITCODE=0` + 零 error（真重编译）。

## 1. orange-wz 门禁实证（AGENTS.md ACTIVE）

| 项 | 值 |
| --- | --- |
| 连接 | `custom-orange_wz`（global / streamable-http / `http://127.0.0.1:10012/mcp` / 29 tools） |
| key | `tools/_gms_key.b64` = `key_gms` 65536B，b64_len 87384，sha256_16 `268a8407a47ee76c`，IV `TSPHKw==`（GMS `[0x4D,0x23,0xC7,0x2B]`） |
| `load_files` | `exclusive:false`，路径 `E:/MXD/BeiDou-Client-xy/Data/UI` → OK `loadedCount=1` |
| `list_children` | 结构层 **20 个真 .img 子节点**（Basic/BuffIcon/CashShop/…/UIWindow.img） |
| 内容层下钻 | `descend_Basic.img` / `search_Basic.img` / `descend_UIWindow.img` / `search_UIWindow.img` → 4 次 `IMG 解析失败: *.img` |
| 收口 | `unload_all` → `{"rootCount":0,"ok":true}`，`list_loaded_roots` → `roots:[]` |
| 合规回退 | `tools/nx_inspect.py` 读 `UI.nx`：`root_children=20`，`util_dlg_ex_controls=24` |

- 产物：`evidence/orange-wz-npctalk-ui-structure.json`（6301B，`raw_error_texts_verbatim` 逐字 + key 规格 + PARSE_FAIL 严格分类）+ `evidence/nx-ui-npctalk-content-probe.json`（4336B）。
- 脚本：`tools/wave2_gate_npctalk_probe.py`（从 `tools/face11_gate_and_quest_probe.py` 派生，自包含 JSON-RPC/SSE 桥，`classify()` 命中 `IMG 解析失败`/`out of bounds`/`Index`/`缺少参数`/`失败`/`Error`/`找不到根节点` 判 PARSE_FAIL，不假绿）。
- 结论口径：**结构层 WORKS / 内容层 FAILS**，与前九/十/十一面一致；历史 `orange-wz 未连接/阻塞/.nx 为准` 口径已被 YG 2026-11-09 覆盖，本面按新规则执行。

## 2. 缺陷逐字证据（`IO/UITypes/UINpcTalk.cpp` live 1961 行 / `.h` 347 行）

- `send_key :733-758`（修复前）：`if (type == TalkType::SENDSIMPLE){ if (keycode==UP){ if (simple_selected>0) --simple_selected; return; } if (keycode==DOWN){ if (!simple_choices.empty() && simple_selected+1 < static_cast<int32_t>(simple_choices.size())) ++simple_selected; return; } if (keycode==RETURN){ if (dispatch_simple_selection()){ clear_input_field(); deactivate(); } return; } }` —— **全段只动 `simple_selected`，无 `offset`、无可視带判据**。
- `draw() :225-263` 选中虚线：`scroll = show_slider ? offset*scroll_step : 0`、`vline = text_draw_y + c.pos.y() - scroll`、`visible = !show_slider || (vline>=band_top && vline+ls<=band_bottom)`；`visible=false` 时不画虚线。
- `send_cursor() :630-651` 命中：`if (show_slider && (vline < band_top || vline + ls > band_bottom)) continue;`（滚出可视带的选项不可点）。
- `onmoved :146-154`：`int16_t shift = upwards ? -unitrows : unitrows; if (offset+shift>=0 && offset+shift<=rowmax-unitrows) offset += shift;` —— **滑块是 `offset` 的唯一驱动，键盘导航不触发**。
- `change_text :1271-1289`：超长文本建滑条 `show_slider=true`、`scroll_step=Text::linespace(A12M)`、`rowmax=(over+scroll_step-1)/scroll_step+1`、`unitrows=1`、`Slider(...,onmoved)`；`:1300-1307` `text_draw_y = top.height()+TEXT_Y_PAD`；`:1457 simple_selected=0`。
- `Slider.cpp:116-121 void Slider::setrows(int16_t nr,int16_t ur,int16_t rm){ rowmax=rm-ur; recalc_rowheight(); row=nr; }`（**不触发 onmoved**）；`Slider.h:34` 声明该三参重载。
- `UINpcTalk.h:255-259` `int16_t offset, unitrows, rowmax, scroll_step=18;`、`:295 int32_t simple_selected=0;`、`:281-291 struct SimpleChoice{selection,first,last,pos,size}; std::vector<SimpleChoice> simple_choices;`。
- `dispatch_simple_selection() :1037-1051`：`idx=(simple_selected>=0 && <size)?simple_selected:0` → `NpcTalkMorePacket(simple_choices[idx].selection).dispatch()`（有 idx 兜底，但**不校验可见性** → 回车提交不可见项的直接出处）。
- ops_grep 41 命中：`offset` 仅出现在 `:78/:149-153/:216/:233/:621/:1282`；`simple_selected` 仅在 `:229-232/:652/:737-745/:1042-1056/:1457` → **send_key SENDSIMPLE 分支是唯一缺可视带守卫处**。

## 3. 最小修复（`tools/_apply_wave2_fix.py` 一次性精确补丁）

- 补丁脚本先**从 live 探测锚点缩进**（`ANCHOR_BASE 2 EXTRA 1`），再对 OLD/NEW 模板统一重定缩进后 `assert count==1` 才替换 —— 照摘要拼锚点会漂一层 tab，这是 round34/35 edit 失配的同类坑，改用动态探测根治。`WROTE_OK crlf=True +lines=45`。
- 新插入 `ensure_visible` lambda 逐字要点：
  - 前置早退：`if (!show_slider || simple_choices.empty() || simple_selected<0 || static_cast<size_t>(simple_selected)>=simple_choices.size()) return;`
  - `const SimpleChoice& c = simple_choices[static_cast<size_t>(simple_selected)];`
  - `const int16_t ls = Text::linespace(Text::Font::A12M);`、`band_top = top.height()-1`、`band_bottom = band_top+height-TEXT_SLACK`、`maxoffset = max(rowmax-unitrows, 0)`；
  - `int16_t vline = text_draw_y + c.pos.y() - offset*scroll_step;`（与 `draw():234`/`send_cursor():636` 同源）；
  - `vline+ls > band_bottom` → `while (offset<maxoffset && ...) ++offset`；`vline < band_top` → `while (offset>0 && ...) --offset`；末 `slider.setrows(offset, unitrows, rowmax);` 同步 thumb。
- `UP`/`DOWN` 各在移动 `simple_selected` 后调 `ensure_visible();`（返回前）。`RETURN` 分支不动（选中恒可见后回车必落在可见项）。
- `git diff --stat` = `IO/UITypes/UINpcTalk.cpp | 45 +++++++++...`，1 file changed, 45 insertions(+)。

## 4. 自测 + 构建门禁（本面验收证据）

- `python tools/test_npctalk_simple_visible.py` → `TEST_RC=0`：
  - 7 静态守卫逐字在盘（含 `auto ensure_visible = [&]()` / `vline + ls > band_bottom` / `vline < band_top` / `slider.setrows(offset, unitrows, rowmax);` / draw `visible = !show_slider` / cursor 守卫保留 / `ensure_visible()` 调用数=2）；
  - A1 20 项长菜单 DOWN：`before(invisible=8, end off=0) -> after(invisible=0, end off=8)`；A2 回滚 UP：`sel=0, off=0`；
  - B 短菜单 n=2/3/5/8（no-slider）修复前后逐项一致；C 边界 n=0/1/2 无 runaway；D 修复前不可见回车目标 8 个（例 `sel=12 off=0`）；
  - `PASS: 7 static guards + 10 scenarios (before bad=16 -> after bad=0; regression OK)`。
  - 首跑曾因 `cursor_guard_kept` 按摘要拼成单行 `... ) continue;` 而 FAIL；读 live `:641-642` 发现 `continue` 独占一行 → 按 live 逐字取判据行修正，不放宽标尺、不删守卫，重跑 PASS。**自测口径自身也按事实自净**（承 round36 harness 矛盾输出教训）。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`。
- `MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c "tools\build_smoke.bat"` → `GATE OK` + `EXITCODE=0`；`build_log.txt` mtime `2026-11-09 23:14:41` → `2026-11-10 00:08:03`、2867B → 3551B（真重编译），`ERROR_LINES` 空；tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` + 复制文件行。既有 warning（C4458 speaker 隐藏类成员 `UINpcTalk.cpp:1202` / LNK4098 / LNK4099）非本轮引入。
- `taskkill /F /IM MapleStory.exe /T`：34476/6108/56016 三个 32K 空壳 `Access is denied`（不占资源、不阻断构建，用户 m04292 占用策略已执行）。

## 5. 不在本轮范围（诚实挂起）

- 观感/像素级：A11b–A11j 下项钮/高亮/GetText/GetNumber/AskAvatar/AskBoxText 逐像素对齐、玩家列表五页签、UIGuildBBS 多页 —— 需 orange-wz 内容层恢复或真机截图，进 §6 与 `pending-live-tests`。
- 2-1 F 维 12 项样式降级、2-2 资源层（Photoshop MCP 位图 / 真精灵 / MapleTV 双屏）、2-4 隐藏门三态/SKILL_EFFECT/NPC_ACTION 真机触发 —— 未动。
- Wave3（L-B 涉包体结构/新增 c2s/s2c 差集）待用户裁决 D1–D14；场地伤害族 c2s、Quest 107 x/y、PARTY_SEARCH_REGISTER(220)、MTS_OPERATION(0xFD) 等保持 s9 优先、不改协议。
- 本轮不改 `Net/*`、不碰 WZ/IMG/XML 内容层、不动校准十一面文档。

## 6. 真机联测 backlog（→ `pending-live-tests/README.md` §16）

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 16-1 长菜单键盘 | 拍卖行 9900001 / 长 #L 列表对话，连按 DOWN 越过可视带 | 高亮虚线始终跟随、滑块 thumb 同步下滚、无「看不见却被选中」 | harness 只复刻几何，需真机字图渲染 + UtilDlgEx 资源 |
| 16-2 回车提交 | 16-1 末尾直接回车 | 提交的是当前高亮可见项，与点「下项」一致 | 需登录服 + 服务端脚本回包验证 selection |
| 16-3 回滚 | 16-1 后连按 UP 回首行 | offset 收敛回 0、thumb 归位、无残留滚动 | 需真机滑条控件状态 |
| 16-4 短菜单回归 | 2/3/5/8 项短对话（无滑条）上下键 + 回车 | 与修复前一致、无回归 | 需多个 NPC 脚本触发点 |

exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 00:08 重编译）。

## 7. 提交 scope（精确 add，零署名）

```
git add IO/UITypes/UINpcTalk.cpp \
        docs/features/development-and-fix-plan-2026-11/README.md \
        docs/features/development-and-fix-plan-2026-11/notes/wave2-npctalk-recheck-20261109.md \
        docs/features/development-and-fix-plan-2026-11/evidence/orange-wz-npctalk-ui-structure.json \
        docs/features/development-and-fix-plan-2026-11/evidence/nx-ui-npctalk-content-probe.json \
        docs/features/pending-live-tests/README.md \
        tools/test_npctalk_simple_visible.py \
        tools/wave2_gate_npctalk_probe.py \
        tools/_apply_wave2_fix.py
```

- 提交标题：`🐛 fix(ui): NPC 对话 SENDSIMPLE 键盘导航可视带守卫（选中恒可见+滑块同步）(#devfix-npctalk-visible)`。
- `.gitignore` 混合编码（`ERR_INVALID_ENCODING`，byte 0xb1 @ pos 2415）不改，改精确 `git add`；`build_log.txt`/`*.obj`/`*.log`/`*.nx` 等不进提交。
- 校准 evidence / 截图 / `*.pyc` / `_*.py` 一次性补丁脚本是否入库：本轮 `_apply_wave2_fix.py` 与两个 probe/harness 均作为**可复现证据链**随功能提交保留（同 round34/35/36 做法），不进游戏构建。

## 8. 性质标注

- 本文件是**开发/修复现场复核记录**，非校准文档、非实现提交说明的替代品。
- 全程：083/v83 only 禁 095；校准≠改回原生；s9 协议优先；数值重合≠语义重合；orange-wz MCP 优先、解析异常才合规 `.nx` 回退（原文 + key 规格留证）；校准十一面文档只读。
- 本轮按用户 m04292 授权改游戏源码（`UINpcTalk.cpp`），可自测项自测通过后才提交，真机项进 backlog 待一起测。
