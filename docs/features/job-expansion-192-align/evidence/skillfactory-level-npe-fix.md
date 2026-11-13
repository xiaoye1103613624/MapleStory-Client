# SkillFactory level NPE fix（2026-11-12）

## 是否本会话导致

**是。** Wave1 Cygnus 四转（`1112`–`1512`）与 Wave2 双刀/龙神（`430`–`434`、`2001`/`2200`/`221x`）合入的 Skill XML 为现代 GMS `common/maxLevel` 公式格式，**没有** Odin/BeiDou `SkillFactory` 必需的展开 `level/` 子树。

空壳 Evan（`2210/2212/2213/2215/2216/2218`，478B、`skill/` 空）**不会**触发 NPE（无 skill 子节点可迭代）。

## 根因

`SkillFactory.loadFromData`（约 376 行）原逻辑：

```java
for (Data level : data.getChildByPath("level")) { ... }
```

`getChildByPath("level") == null` 时对 null 做 for-each → NPE →「加载wz任务执行失败」。

首个按文件名顺序踩雷的是 `1112.img.xml / 11121000`（Cygnus）。合计缺 `level/` 的 skill：**291**。

## 修复

1. **XML**：`tools/_expand_skill_common_levels.py` 将 `common` 公式展开为 `level/1..maxLevel`（`d`=floor，`u`=ceil，`x`=等级）。已处理 15 个 img，展开 291 个 skill。备份：`tools/_staging/wave1_cygnus4/bak_skill_pre_level_expand/`。
2. **Factory 防御**：`SkillFactory` 对 `level` 做 null 判断，避免再因个别缺树整服起不来。
3. **验证**：`tools/_verify_skill_level_nodes.py` → `scanned_files=87 skills=892 missing_level=0`。

## 再启服

在 `BeiDou-Server_s9` 按原方式启动即可（WZ 读 `gms-server/wz/Skill.wz`）。若仍失败，先确认进程读的是该目录而非其它语言副本/旧缓存，并重跑：

```text
python tools/_verify_skill_level_nodes.py --skill-dir E:\project\BeiDou-Server_s9\gms-server\wz\Skill.wz
```
