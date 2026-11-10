# round70 · 伤害数字 / 伤害皮肤 按 Angel 083 只读取证复核（2026-11-11）

## 0. 授权与范围
- m04292：授权改源码 / 自测 / 测试完成提交 / 遇 MapleStory.exe 占用直关 / 不能自测记 backlog。
- User said (m09859)：「若碰到ui布局、动画一定要确认是否修复了，如果不是按照Angel 布局修复过的，可以重新修复；之前有很多参照BeiDou或者自行修复的ui、动画都有些问题；」—— 本轮据此逐面复核「自行修复」动画。
- User said (m10982)：「运行时默认是 `data.format=img` 零转换合并整目录 `.img`，而 `catalog_Effect.json` 恰好漏掉唯一一个 64MB 的 `DamageSkin.img` —— 这是「合并加载可能整根失败」的强线索。用最小探针实测 `open_merge` 单文件，别拿推断当结论。」—— 本轮据此写最小 C++ 探针实测。
- 门禁 18 已走：REA → IDA(083) → orange-wz(IMG) → `.nx` 回退 → 运行时 open_merge 探针。只读取证，不碰 s9 协议 / handler。

## 1. 083 权威语义（IDA 逐字）
- `?ShowDamage@CMob@@IAEXJJHHHH@Z @0x6691d3 size 0x9b`：`nDamage<=0 && !=0 → Effect_HP(a5:1)`；`==0 → Effect_Miss`；`>0 → Effect_HP(a5:0)`；`y = -15 - a3*(bHalfHeight?15:30) + Gety`。8 个 xrefs（Update / OnHit / OnDamaged / sub_670C63 / sub_670F41）。
- `?Effect_HP@CAnimationDisplayer@@QAEXJJJJH@Z @0x437d0f size 0xcd9`：`SP_5515_D` Format 数字串 → 逐位 `IWzProperty::GetItem` 取位图 → **`lWidth = 3 * v23 / 5`（按真实位图宽动态计算）** → canvas 高 57 → `v53 = 57 - (idx%2!=0?1:4) - rand()%2` 奇偶+随机 y 抖动 → `sub_402FAB(400,3)` / `(600,3)` / `(v81+1000,3)` 三段动画（≈1000ms 淡出）。
- `?Effect_Miss@CAnimationDisplayer@@QAEXJJJ@Z @0x438a21 size 0x115`：`SP_984_MISS` 专用位图 + `Effect_BasicFloat @0x43de28`。
- **0 命中**：`*DamageNumber*` / `*DamageSkin*` / `*CharSet*` / `*NoCri*` / `*CDamageNumber*` / `*SetNumber*` / `*MakeDamage*` / `*CField*OnPacket*` → 083 无 DamageNumber 类、无伤害皮肤系统、无 CharSet 类。伤害皮肤是 v83 之后功能。

## 2. 当前端 vs 083 对照清单
| 维度 | 083 原生 | 当前端（Journey + s9） | 判定 |
| --- | --- | --- | --- |
| 数字呈现 | Effect_HP 逐位图动画 + 动态 3/5 字宽 | DamageNumber + 硬编码 advance 表 `{24,20,22,22,24,23,24,22,24,24}` | 架构差异（083 无此类） |
| MISS | SP_984_MISS 位图 + BasicFloat | `charset[true].draw('M')` → 经 `Charset.cpp:30-43 charset_char('Miss')='M'` 解析到 98×38 专用 MISS 位图 | 等效（非字符 'M'，早前误判已更正） |
| 超长数字 | 无（逐位图无缩放概念） | `MAX_DAMAGE_WIDTH=260` 等比缩放（BasicEff 仅 0-9+Miss，.nx 实测属实） | 本端 s9 适配，注释自称 Journey 未冒称 083 |
| 伤害皮肤 | 无（v83 后功能） | `Effect["DamageSkin.img"][skin]` 字形切换 + s9 `DAMAGE_SKIN(247)` | s9 协议对齐产物，不动 |
| 动画时长 | 400/600/1000ms 三段 | `FADE_TIME=500` / `vspeed=-0.25` | 差异，属 D-A |
- 结论：与 round66/67/68「找错节点后自造兜底」不同，本面**资源根用对了、缩放注释也经 .nx 实测属实且自称 Journey**，不是「把自造兜底冒称 083」的病。真正差异是**架构/版本级**（083 根本没有 DamageNumber/皮肤系统）。

## 3. orange-wz 门禁 + .nx 事实
- `mcp__orange_wz__*`（endpoint `http://127.0.0.1:10012/mcp`）：65536B GMS key（`tools/_gms_key.b64` b64_len 87384 / sha256_16 `268a8407a47ee76c` / iv `TSPHKw==`），`load_files exclusive:false` 结构层 WORKS；内部节点 query 全部 `IMG 解析失败: <name>`（GBK 原文留证）→ 合规 `.nx` 回退。
- `Effect.nx`（126168494B / 2026-09-21）顶层 17 节点**无 DamageSkin.img** —— 旧缓存，仅 `data.format=nx` 才生效，当前默认不是，**非运行时权威**。
- `BasicEff.img` 数字集：`NoRed0/1`、`NoCri0/1`、`NoViolet0/1` 各 0..9，`Miss 98×38` 只在 `NoRed0/Miss` 与 `NoViolet0/Miss`；无 K/M/B → 缩放注释属实。

## 4. 运行时后端定性 + 最小探针（翻案，决定性）
- `Configuration.h:389 DataFormat = "img"`（生产默认），部署无 `config.ini`/`data.format` 覆盖 → `ResourceService::init` 走 `ImgBackend` → `ImgSource::init`。
- `ImgSource.cpp:297-309 load_standard`：`gather(Effect 目录全部 18 个 .img)` → `nl::img_file::open_merge` → `RootTable::bind(Effect)`。`WzRuntime.cpp:227-243 open_merge` 按 rel 文件名建节点，故 `Effect["DamageSkin.img"]` 解析成功时可达。
- `catalog_Effect.json` 逐文件 `open_merge`（`ResourceProbe.cpp:637-667`）17 文件、漏 DamageSkin.img，`_resource_probe.log:80 [cat] Effect files=17`。
- **用户 m10982 的强线索（合并整根失败）经最小探针证伪**：`_tools/dmg_skin_probe`（镜像 `mapobj_probe` MSVC 配方）5 组实验全绿 ——
  1. `BASICEFF_SINGLE` content_valid=1 children=41（对照）；
  2. `DAMAGESKIN_SINGLE` content_valid=1 children=624（catalog 同款路径 OK）；
  3. `WHOLE_MINUS_DAMAGESKIN` 17 文件合并 → root 17、Effect[DamageSkin.img] 无效（预期）；
  4. `WHOLE_ALL` 18 文件合并 → root 18、**`Effect["DamageSkin.img"]_valid=1 children=624`**、皮肤 1/10/100/1000/1001 的 `NoRed1/NoCri1` 齐全、`OPEN_MERGE_OK`；
  5. `DAMAGESKIN_CATALOG_FULL` 全量遍历 624 节点零异常。
  - 首跑 `PROBE_EXIT=-1073741515`（`0xC0000135` DLL 缺失、stdout 空）→ 补 `liblz4.dll`+`zlib1.dll` 后 `PROBE_EXIT=0`。装载失败≠解析失败，未误判。
- **catalog 漏文件改判为「过期快照」**：catalog 生成 2026-11-05 00:10（17 文件），Effect 目录 mtime 2026-11-10 17:08（18 文件）> catalog → 快照早于当前目录内容；是观测盲区，不是解析失败、更不是死链。
- `[PUA生效 🔥]` 主动用实测证伪用户强线索，避免把「DamageSkin.img 疑似死链」错误定性写进交付 / 决策点。

## 5. 零风险补丁 + 决策点 D-A
- 4 处纯注释插入（`DamageNumber.cpp` ×3 / `.h` ×1）：083 权威依据（Effect_HP 0x437d0f / 3/5 / 57 / 三段动画 / MISS 专用位图）、本端 Journey+s9 身份、DamageSkin.img 可达性实测结论、潜在缺陷说明。**0 行为字面量改动**（harness A2 逐条守卫）。
- **潜在缺陷（登记决策点，不擅改）**：`apply_skin` 把 charset 换成 DamageSkin 字形后，`getadvance()` 仍用硬编码 BasicEff 宽度表 → 皮肤数字间距可能错位；083 用动态 3/5 字宽。整体按 083 重排属架构/视觉级，且 083 无此类系统、当前端是 Journey+s9，按「校准≠改回原生 / s9 优先」不整体重写。
- **一次性 Plan（待 D-A 裁决后执行）**：① 五屏截图（普通/暴击/皮肤 A/皮肤 B/MISS）核间距；② 若裁决对齐 083，改 `getadvance` 为按 `charset` 实际位图宽动态取 3/5 + canvas 57 + 三段动画；③ 保持 s9 `DAMAGE_SKIN(247)` handler 与 `Effect["DamageSkin.img"]` 路径不动；④ harness + GATE + 真重编；⑤ 原子提交。

## 6. 真机验收 backlog（PLT §36）
- 36-1 伤害皮肤实际显示（启 s9 + 装备皮肤 1/10/100）+ 与 BasicEff 默认对比截图；
- 36-2 皮肤数字间距观感（D-A 前置证据）；
- 36-3 MISS 是否正确显示 98×38 专用位图（非字符 M）；
- 36-4 超长伤害（16 位）缩放不截断；
- 36-5 普通/暴击/TOPLAYER 三型字形与颜色。
- 已知限制（非缺陷）：083 无 DamageNumber/皮肤系统，本端不按 083 整体重写；皮肤间距动态化属 D-A 待裁决；DamageSkin.img 可达性已闭环、不再是决策点。

## 7. 提交 scope（精确 git add，零署名）
- `Gameplay/Combat/DamageNumber.cpp`、`Gameplay/Combat/DamageNumber.h`、`tools/test_round70_damagenumber_083.py`、`docs/features/development-and-fix-plan-2026-11/evidence/damagenumber-083-round70.json`、`docs/features/development-and-fix-plan-2026-11/notes/wave2-damagenumber-083-review-20261111.md`、`docs/features/development-and-fix-plan-2026-11/README.md`、`docs/features/pending-live-tests/README.md`（7 文件）。
- 一次性脚本 `tools/_dmg_*.py`、探针 `_tools/dmg_skin_probe/*`、IDA 反编译 `tools/_ida_dmg/*`、探针 stdout `tools/_dmg_probe_stdout.txt` 仅本地证据链留存，不进版本库；evidence JSON 已汇总其结论与路径。

## 8. 教训
1. 推断不是结论：catalog 漏一个文件有多种解释（时序/解析/过滤），必须最小探针实测 `open_merge` 再定性。
2. 装载失败 ≠ 解析失败：`0xC0000135` DLL 缺失、stdout 空，先把运行时依赖补齐再读业务结论。
3. 缓存（Effect.nx）≠ 运行时权威：先定 `data.format`，再决定读哪份数据。
4. 数值/资源重合 ≠ 语义重合：083 有 ShowDamage 不代表有 DamageNumber/skin；校准≠改回原生，s9 优先。
