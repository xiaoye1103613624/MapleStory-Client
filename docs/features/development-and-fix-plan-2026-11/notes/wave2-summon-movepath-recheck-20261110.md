# round40 · Wave2 2-8 召唤兽 MOVE_SUMMON 影子缓存洪泛复核（2026-11-10）

> 性质：只读取证 + 最小源码修复 + 离线自测 + 构建验证 + 原子提交。
> 用户 m04292 授权「可自测的自己测 / 遇 MapleStory.exe 占用直接关闭 / 功能测试完成就提交 / 不能自测的记录后面一起测」。
> 硬约束：083/v83 only 禁 095；校准≠改回原生；s9 优先；数值重合≠语义重合；校准十一面文档只读。

---

## 0. 结论

1. **orange-wz 资源层门禁本轮真闭合**：`Data/Skill`(76) / `Data/Effect`(17) / `Data/Npc`(7198) 三根结构层 `list_children` 全部 WORKS；6 次内容层下钻全部 `IMG 解析失败`，命中 AGENTS.md 写死的回退条件 → 留原始报错 + 65536B GMS 全表 key 规格后合规 `.nx` 回退（`Skill/Effect/Npc.nx` root_children 一致）。`no_fake_green`：PARSE_FAIL 严格分类，不把报错文本计成 OK。
2. **第 7 例同族「主体路径有守卫、旁路状态不同步」缺陷**：`MapFieldMarkers::report_owned_c2s` 的 `last_summon_oid[8]` 影子缓存只在 `clear()` 复位；die 动画播完 / lifetime 到期 / `RemoveSummonHandler` 三条回收路径都从 `markers` 移除对象却不清数组槽 → 8 槽被僵尸 oid 占满后新召唤每 TIMESTEP 重发一次 `MoveSummonPacket`（≈60/s）直到换图。
3. **最小修复**：8 槽三字段换成 `std::unordered_map<int32_t, Point<int16_t>> last_summon_pos` 键控缓存 + 每帧按 live marker 状态自愈裁剪 + 按 oid 查缓存；无槽上限、无僵尸残留。不改协议、不改包体字节、不碰 WZ/IMG/XML。
4. **自测实证**：`tools/test_summon_movepath_flood.py` 10 静态守卫 + 10 场景 PASS（F 系 `before flood=59/29/119 → after 0`，累计 `before=207 → after=0`；回归 7/7）；`gen_opcode_matrix.py --check` GATE OK；`build_smoke.bat` GATE OK + EXITCODE=0（真重编译，日志 mtime 见 §4）。
5. 真机观感 / 回包 hex / 多召唤切换图进 `pending-live-tests` §19，4 项待用户一起测。

---

## 1. 证据优先级 + orange-wz 门禁口径

证据优先级（承 `feature-gap-inventory` §2.3）：`opcode-matrix.json` > 源码 `.dispatch()/emplace` > 现场重算 > 真机记录 > 旧账。本轮不引 11-04 旧账，只以 live 源码 + 本轮真跑为准。

AGENTS.md ACTIVE（YG 2026-11-09）：WZ/IMG/XML 一律先 orange-wz MCP（`custom-orange_wz` / `http://127.0.0.1:10012/mcp` / 29 工具），仅 `IMG 解析失败` / `Index N out of bounds` / GBK 错误文本才合规 `.nx` 回退，回退须记录原始报错 + key 规格。

`tools/wave2_gate_resource_probe.py` 实跑 [exit 0]：

| 阶段 | 工具 | 结果 |
| --- | --- | --- |
| 加载三根 | `load_files(paths=[Data/Skill,Data/Effect,Data/Npc], key=GMS, exclusive:false)` | `OK {"loadedCount":3,"rootCount":3,"generation":2}` |
| 结构层枚举 | `list_children` ×3 | `OK children=76 / 17 / 7198` |
| 内容层下钻 | `get_node_tree_json(autoParse:true)` + `search_node(keyword="summon")` ×6 | **6 次全 `PARSE_FAIL IMG 解析失败: <000.img / BasicEff.img / 0002000.img>`** |
| 收口 | `unload_all` → `list_loaded_roots` | `OK {"rootCount":0,"ok":true}` |
| 合规回退 | `tools/nx_inspect.py` PKG4 `Nx(.nxcache/{Skill,Effect,Npc}.nx)` | `Skill root_children=76 / Effect=17 / Npc=7198` |

- key 规格逐字：`tools/_gms_key.b64` = `key_gms` 65536B（`tools/vendor/NoLifeWzToNx-master/Keys.cpp`），`b64_len=87384`、`sha256_16=268a8407a47ee76c`、`ivBase64=TSPHKw==`（GMS 4B `[0x4D,0x23,0xC7,0x2B]`）。
- evidence：`docs/features/development-and-fix-plan-2026-11/evidence/orange-wz-resource-structure.json`（含 `raw_error_texts_verbatim` + `conclusion{structure WORKS, content FAILS, fallback_condition_met, no_fake_green}`）+ `evidence/nx-resource-content-probe.json`。
- 结论：结构层能跑、内容层解析失败 → 与第七~十一面 / round37 同构，不是「未连接」，回退合规。

---

## 2. live 源码逐字对照（缺陷立项）

触发源：`Gameplay/Stage.cpp:270` `field_markers.report_owned_c2s(player.get_oid(), player.get_position(), mobs)` 每帧调用（Constants::TIMESTEP ≈ 16ms）。

修复前 `Gameplay/MapleMap/MapFieldMarkers.cpp:344-367`：

```cpp
bool moved = true;
for (uint8_t i = 0; i < last_summon_n; ++i)
{
    if (last_summon_oid[i] == sum->get_oid())
    {
        moved = last_summon_pos[i] != pos;
        last_summon_pos[i] = pos;
        break;
    }
}
if (moved)
{
    bool found = false;
    for (uint8_t i = 0; i < last_summon_n; ++i)
        if (last_summon_oid[i] == sum->get_oid()) { found = true; break; }
    if (!found && last_summon_n < 8)
    {
        last_summon_oid[last_summon_n] = sum->get_oid();
        last_summon_pos[last_summon_n] = pos;
        ++last_summon_n;
    }
    Movement mv(pos.x(), pos.y(), pos.x(), pos.y(), 0, 1);
    MoveSummonPacket(sum->get_oid(), pos, mv).dispatch();
}
```

`Gameplay/MapleMap/MapFieldMarkers.h:61-68`（修复前）：

```cpp
MapObjects markers;
Point<int16_t> last_summon_pos[8]{};
int32_t last_summon_oid[8]{};
uint8_t last_summon_n = 0;
Point<int16_t> last_dragon_pos{};
int32_t attack_cd_ms = 0;
```

三条回收路径**都不清 `last_summon_oid[8]`**：

| 路径 | 逐字证据 |
| --- | --- |
| 唯一复位点 | `MapFieldMarkers.cpp:304-311 clear()` 里 `last_summon_n = 0;`（仅换图） |
| 对象移除 | `Gameplay/MapleMap/MapObjects.cpp:119-129 remove(oid)`；`:59-95 update()` 对 `newlayer==-1` 隐式 `objects.erase(iter)` |
| die 播完 | `Gameplay/MapleMap/MapSummon.cpp:190-193` `deactivate(); return -1;` |
| 服务器移除 | `Net/Handlers/MapObjectHandlers.cpp:1423-1434 RemoveSummonHandler` → `summon_die(oid)` 或 `remove(oid)` |

**后果链**：8 槽被僵尸 oid 占满 → 新召唤 `moved` 恒 true、`found` 恒 false、`n<8` 不成立 → 每 TIMESTEP 发一次 `MoveSummonPacket`，静止不动也刷 ≈60/s 直到换图。这是「主体 `markers` 自己有 `is_active()` 守卫、旁路影子缓存永不回收」的状态不同步病，与 round34 `UIWorldSelect` / round35 `UICharSelect` / round36 `Textfield` / round37 `UINpcTalk` / round38 `UIRaceSelect` / round39 `UIGuildBBS` 同族第 7 例。

决策：数组槽回收要写压缩逻辑且「8 槽上限」本身是缺陷源 → 直接换键控 `unordered_map` 消除槽满条件，每帧自愈裁剪 gone / inactive / non-MapSummon / foreign 条目。**不改 `MoveSummonPacket` 协议/包体**，`s9` 上行仍只在真实位移时发一次。

---

## 3. 最小修复（逐字）

`Gameplay/MapleMap/MapFieldMarkers.h`：加 `#include <unordered_map>`；private 三字段 →

```cpp
// oid -> last MOVE_SUMMON position. Keyed (no 8-slot ceiling);
// report_owned_c2s prunes gone / inactive / non-MapSummon / foreign entries each frame.
std::unordered_map<int32_t, Point<int16_t>> last_summon_pos;
```

`Gameplay/MapleMap/MapFieldMarkers.cpp:308` `last_summon_n = 0;` → `last_summon_pos.clear();`

`:321-332` 新增每帧自愈裁剪（`attack_cd_ms` 递减后、`for (auto& kv : markers)` 之前）：

```cpp
// 影子缓存自愈：die 动画播完 / lifetime 到期 / RemoveSummonHandler 都会让
// 召唤兽从 markers 消失，旧数组槽却永不回收；槽满后新召唤每帧重发
// MOVE_SUMMON（约 60/s）。先按 live 状态裁剪，再按 oid 查缓存。
for (auto it = last_summon_pos.begin(); it != last_summon_pos.end();)
{
    auto ex = markers.get(it->first);
    auto* ex_sum = ex ? dynamic_cast<MapSummon*>(ex.get()) : nullptr;
    if (!ex || !ex->is_active() || !ex_sum || ex_sum->get_owner() != local_cid)
        it = last_summon_pos.erase(it);
    else
        ++it;
}
```

`:357-367` 原 20 行 moved/found 双循环 →

```cpp
auto cache = last_summon_pos.find(sum->get_oid());
bool moved = cache == last_summon_pos.end() || cache->second != pos;
if (cache == last_summon_pos.end())
    last_summon_pos.emplace(sum->get_oid(), pos);
else
    cache->second = pos;
if (moved)
{
    Movement mv(pos.x(), pos.y(), pos.x(), pos.y(), 0, 1);
    MoveSummonPacket(sum->get_oid(), pos, mv).dispatch();
}
```

`tools/_apply_wave2_summon_fix.py` 一次性精确补丁（动态探测 tab/CRLF，避开 edit 工具对 tab+中文注释失配的 round34/35/37/38 教训）。补丁脚本首版曾引入两处毛边：① `last_summon_pos[sum->get_oid()] = pos;` 对 `Point` 无默认构造不安全 → 改 `emplace`/`cache->second` 分支；② `ind + ""` 产生尾随 tab 空行 → 改 `""` 并 `git checkout HEAD --` 两文件后重跑。最终 `git diff --check HEAD -- <两文件>` 为空、`WROTE_OK`、`git diff --stat` = `2 files changed, 24 insertions(+), 23 deletions(-)`。**补丁自己有毛边就回退重跑，不把「看起来对」当闭环**。

---

## 4. 自测 / 门禁实证

`tools/test_summon_movepath_flood.py`：10 静态守卫（hdr `#include <unordered_map>` / 键控字段 / `last_summon_pos.clear()` / 裁剪判据行 / `erase(it)` / `emplace` / `MoveSummonPacket...dispatch()` 仍在 / 旧 `last_summon_n`、`last_summon_oid[]`、`last_summon_oid[8]` 全部 absent）+ Python 复刻 `OldCache(8 槽数组)` 与 `NewCache(键控 map + prune)` + 10 场景（F 僵尸洪泛 / N 常规回归 / P 裁剪）。

实跑 [exit 0]：

```
STATIC GUARDS OK (10 needles, 2 files)
[OK] F1[F] before flood=59 -> after flood=0 (totals 60->1, legit=1)
[OK] F2[F] before flood=29 -> after flood=0 (totals 30->1, legit=1)
[OK] F3[F] before flood=119 -> after flood=0 (totals 120->1, legit=1)
[OK] N1[N] before flood=0 after flood=0 (expect both 0)
[OK] N2[N] totals equal 20==20
[OK] N3[N] before flood=0 after flood=0 (expect both 0)
[OK] N4[N] before flood=0 after flood=0 (expect both 0)
[OK] P1[P] before flood=0 after flood=0 (expect both 0)
[OK] P2[P] before flood=0 after flood=0 (expect both 0)
[OK] N5[N] totals equal 10==10
[flood] zombie cases cumulative packets: before=207 after=0 (legit excluded)
[regression] non-flood/normal cases ok: 7/7
PASS: 10 static guards + 10 scenarios (before flood>0 -> after 0; regression 7/7)
TEST_RC=0
```

`python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`。

`tools/build_smoke.bat`（`MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c`，round34 教训：裸 `cmd.exe //c` 在 MSYS 下只回显 banner 是假执行）：真重编译，`GATE OK` + `EXITCODE=0` + `ERROR_LINES 0`，tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`；既有 warning `LNK4098 LIBCMT` / `LNK4099 vc120.pdb` / `/LTCG`。`taskkill /F /IM MapleStory.exe /T` 对 34476/6108/56016 三个 32K 空壳 `Access is denied`（不占资源、不阻断构建）。一次后台 job `bash-d50bd6312472497a` 返回 `EXECUTOR_UNAVAILABLE: Niubash execution tree link count mismatch: provenance.json` `[effects:none; No command was executed]`（与第八面 arch.exe / round34 同类纯运行时抖动，非任务失败），按提示原命令重试一次即成功。

> 构建日志 mtime / size 实证：`build_log.txt` `2026-11-10 00:52:38 / 2866B` → `2026-11-10 01:32:45 / 278340B`（本轮全量真重编译，含既有 warning），`ERROR_COUNT 0`、`EXITCODE=0`、`GATE OK`；tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` + 「已复制 1 个文件」×3。零 `error|错误|失败|fatal` 行。

---

## 5. 不在本轮范围

- `DamageSkin` 运行时资源 / 宠物 gage / 场标真精灵 / MapleTV 双屏位图：属 2-2 Photoshop MCP 出图，内容层 orange-wz 仍 `IMG 解析失败`，本轮只做纯 C++ 状态机修复，不碰资源。
- `MapSummon` 帧集只有 stand/hit/die/summoned（无游走/浮动帧）：动画观感项，真机联测。
- s9 `MOVE_SUMMON` 回包 hex / 双开观感 / 召唤兽跟随距离阈值（120/80）：真机联测 §19。
- Wave3 L-B（场地伤害族 c2s、Quest 107 x/y、PARTY_SEARCH_REGISTER(220) 等）：待用户裁决 D1–D14，本轮不动。

---

## 6. 真机联测 backlog

见 `docs/features/pending-live-tests/README.md` §19（4 项：19-1 8 次召唤死亡后新召唤静止不刷包 / 19-2 正常移动每帧真实位移发包 / 19-3 换图 clear / 19-4 多召唤 + 他人召唤不串扰）。

---

## 7. 提交 scope

精确 `git add` 以下文件（不批量 `git add -A`，校准 evidence / 截图 / `*.obj` / `_*.log` / `build_log.txt` 不进功能提交）：

- `Gameplay/MapleMap/MapFieldMarkers.cpp`（M）
- `Gameplay/MapleMap/MapFieldMarkers.h`（M）
- `docs/features/development-and-fix-plan-2026-11/notes/wave2-summon-movepath-recheck-20261110.md`（A，本文件）
- `docs/features/development-and-fix-plan-2026-11/README.md`（M，§4 Wave2 新增 2-8）
- `docs/features/pending-live-tests/README.md`（M，新增 §19）
- `tools/test_summon_movepath_flood.py`（A）
- `tools/_apply_wave2_summon_fix.py`（A）
- `tools/wave2_gate_resource_probe.py`（A）
- `docs/features/development-and-fix-plan-2026-11/evidence/orange-wz-resource-structure.json`（A）
- `docs/features/development-and-fix-plan-2026-11/evidence/nx-resource-content-probe.json`（A）

提交模板（git-commit-standard，零署名）：`🐛 fix(gameplay): 召唤兽 MOVE_SUMMON 影子缓存自愈（8槽僵尸洪泛）(#devfix-summon-movepath)`。

---

## 8. 性质标注

本文件是开发/修复销案复核记录，不是校准文档、不是实现 Plan 替代品、不是提交本身。全程未改协议/包体/WZ/IMG/XML；只读校准十一面文档未被改写。
