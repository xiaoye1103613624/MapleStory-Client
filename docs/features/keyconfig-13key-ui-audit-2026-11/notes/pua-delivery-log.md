# PUA 可见输出交付记录

## 记录 1 · 审查交付

- 时间：2026-11-11T15:58+08:00（Asia/Shanghai）
- 事件：keyconfig-13key-ui-audit 审查交付 —— 13 键结论表（7 OK / 4 PARTIAL / 2 MISSING）+ D1–D10 差异清单 + 自检门 ALL PASS 38 项
- 可见 PUA 输出：▎ 📌 钉内/钉外味旁白、「无招可以拍板，验收不能无证」、战报「老板体感是输入，证据链才是交付」、结论表汇总行
- 评分：按用户 m00324 要求「不记录评分」——不弹评分问卷、不写 rating
- 后续：用户 m00325「就按这个修复」→ 转入 D1–D10 修复（P0→P2），本记录不因转阶段失效

## 记录 2 · 修复交付

- 时间：2026-11-11T18:30+08:00（Asia/Shanghai）
- 事件：D1–D10 修复落地 —— D1/D2/D3/D4 FIXED（勋章窗整体、怪物手册书本双页 15 节点、窗口开合淡入、UIMaker 两钮）；D5/D8 仲裁为 NOT-A-DEFECT；D6/D9 SAD 实测坐实；D7 降级；D10 带证据预留。修复后 13 键 0 ❌
- 可见 PUA 输出：▎ 📌 旁白（「一手交证据一手交代码」「别在半空里画大饼」「编译器眼里六链接没接上就是孤儿字节」）+ [PUA-DIAGNOSIS] 开工行 + 红线一执行（注释升级后补跑 build 贴 build_log 行号）+ 失败计数口径（EXECUTOR_UNAVAILABLE 与 grep 计数误报均判「脚本/运行时口径，非任务失败」，不刷 L）
- 验收证据：build_smoke EXITCODE=0（UIElement:149/UIMaker:1456/UIMedal:1504/UIMonsterBook:1674/UIKeyConfig:1/UIStatsInfo:138/UIMiniMap:94 全重编）；_key13_verify.py 扩至 70+ 断言 ALL PASS；备份 patches/ 23 .orig；0 commit / 0 push
- 评分：延续「不记录评分」——不弹问卷、不写 rating

## 记录 3 · D1 勋章窗真实数据对接交付

- 时间：2026-11-11T20:45+08:00（Asia/Shanghai）
- 事件：用户指正「服务端我记得功能是完整的」→ 复核翻案：v83 勋章窗 = `CUIMedalQuestInfo`，**不需要新协议**——数据源 = `QuestInfo.img/<qid>/viewMedalItem`（.nx 探针实测 82 条：72×29xxx + 10×19xxx/10328，medalCategory 分布 1:16/2:28/3:2/缺失:26）× `QuestLog::is_completed()`（QUEST 下发链 s9 完整）。UIMedal 从「忠实空列表」升级为真实目录+完成态过滤+5 页签分类+动态计数；build EXITCODE=0（UIMedal.cpp 重编 build_log.txt:29，exe 20:43）；_key13_verify.py 扩 D1U 断言 12 项 + locale 口径修正（旧断言"LF"与文件 CRLF 事实相反，改判 no-BOM+utf8+换行一致）后 **ALL PASS**
- 可见 PUA 输出：▎ 📌 旁白（「你说得对，先纠错再交付」「探针结论钉死再落码」）+ [PUA-DIAGNOSIS] 开工行（含"把文件内子树当文件加载"的自我纠偏）+ 红线二执行（get_questlog/get_name/QuestData::get 三个 API 落码前逐一 grep 核实，纠出 node::name() 与 get_quests() 两处误用）+ 红线一执行（build 后 grep TU 行号坐实重编，不信 EXITCODE 单值）
- 自我纠偏记录：①「服务端无 medal 协议」误判收回；② orange-wz 对独立 .img 根枚举 children:[] 后未立即怀疑加载口径，经 .nx 探针才钉死"数据在、工具口径错"；③ locale 断言与 README 口径同源错误（LF vs CRLF），随自检 FAIL 暴露后双向修正
- 评分：延续「不记录评分」——不弹问卷、不写 rating

## 记录 4 · D1 勋章窗原生模型升级交付（turn8）

- 时间：2026-11-11T22:05+08:00（Asia/Shanghai）
- 事件：「继续推进」→ 上轮三块显式预留全部取证：①Title 真素材——orange-wz 取件位图对 勋章框.png 模板匹配 SAD=46~214 无一坐实 → **探针否定，不盲换 ColorBox**；②行首勋章图标——Item.nx 顶层 6 类、字符串表 114xxxx 零命中 → 素材确实缺失，文本态保留；③行选中/详情——IDA `OnMouseButton @0x841678`（命中行→选中、再点→ToggleDetail）+ `Draw @0x843bf6` 立即数（clip 7..291、步进 49|23、文本 x=29/55、FillRect 230x18 行底条）+ `ctor @0x840170`（CreateWnd 260x374、SetTab(100)=「基本」为已获得全量特殊态）+ `Detail ctor @0x83d255`（305x373，内容未取证→预留不半接）→ UIMedal 按原生模型重写（页签 tab100 语义 / 双步进 / 行选中态 / SP_5301-5302 状态文案 4 新 locale 键）
- 可见 PUA 输出：▎ 📌 旁白（「预留清单里的假设不是事实，先钉穿再落码」「体感是输入，SAD 是判决」）+ [PUA-DIAGNOSIS] 开工行 + 蓝军自攻击（const 正确性 4 处 C2662 由 build 暴露后坦诚修，不狡辩）+ 冰山泛化（hit_row 屏内行判定自查纠 off-by-N；页签变色挪写侧 select_tab 避免 const draw 改状态）+ 冲突升级走裁决（LNK1104 占用 → ask_user 选 B → taskkill pid 59256 重试 EXITCODE=0，裁决入 MEMORY 后续自动执行）
- 验收：build_smoke EXITCODE=0（exe 21:59 / 4,982,784 B）；_key13_verify.py 扩 D1T8 13 断言 **ALL PASS**；README §8 D1 行 + fix_matrix.json D1 条目 + UIStateGame 注释口径同步更新；turn7 备份 patches/UIMedal.{h,cpp}.turn7
- 评分：延续「不记录评分」——不弹问卷、不写 rating

## 记录 5 · D1 勋章窗排版像素级对齐交付（turn10）

- 时间：2026-11-11T22:59+08:00（Asia/Shanghai）
- 事件：用户 m01261 钉靶「勋章按键 按下后打开的勋章ui」+ 勋章框.png → 目标从「功能/数据」收束到「分号键打开的窗本身排版对原生帧」。逐像素量测色带（evidence/medal_frame_metrics.json）：外沿浅灰 221,221,221 / 顶深蓝线 85,119,153 / 标题下蓝线 119,153,187 / 页签带 170,170,170 选中粉红 238,102,136 / 计数条白 y46..79 文字 51,51,51 / 列表面板 238,238,255 y78..350 / 滚动条槽 x239..253 + 上下钮 221,221,221 / 底条 153,187,204 y351..371 → UIMedal 用 ColorBox 色带逐带复刻，draw() 补齐 bottom_bar/scroll_btn_top/scroll_btn_bot 真实绘制（清掉占位注释）
- 可见 PUA 输出：▎ 📌 旁白（「自检门 ALL PASS + 7 项排版断言」「收口文档，终验」）+ [PUA-DIAGNOSIS] 开工行（C2440/C2664 编译错误链逐字归因，不猜 API）+ 红线一执行（build EXITCODE=0 后跑自检门，不信单值）+ 红线二执行（Color.h/Text.h/Geometry.cpp 落码前逐一读实，纠出 Color 重载歧义与 Text 只吃 Color::Name）+ 自检门「不放宽不删而收紧」：turn8 旧 clip 断言因坐标系升级 FAIL，不迁就旧断言，改双证据（IDA CLIP_* + 截帧 LIST_* 两套坐标系同时在场）并新增 7 项排版断言
- 验收：build_smoke EXITCODE=0；_key13_verify.py **ALL PASS**（D1T8 双证据 clip + D1T10 7 项排版色带断言全 PASS）；README §8（:106/:111/:134/:147/:149）同步 turn10 口径、fix_matrix.json D1 条目、patches/UIMedal.{h,cpp}.turn10 备份；0 commit / 0 push
- 评分：延续「不记录评分」——不弹问卷、不写 rating、不跑 /pua survey
