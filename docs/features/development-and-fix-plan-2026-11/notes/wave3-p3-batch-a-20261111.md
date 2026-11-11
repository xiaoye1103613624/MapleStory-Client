# round96 · P3 A 批前 3 条（D12 / D11 / D10）

## 1. 为什么这 3 条先做

`P3-ANGEL-REBUILD-PLAN.md` 把 P3 16 条按取证成熟度分 A/B/C。A 批 9 条中，
`login-flow/D12`、`skill/D11`、`corewindow/D10` 的 083 证据已在库或单点可补，
故作为第一批。三条**都不是**「加功能」，而是「把 083 的真实形态钉清楚、把本端口径纠正」。

## 2. `login-flow/D12` —— 逐字节文档（DOC_ONLY）

26 个字段逐一对照 s9 写方 / 当前端读方 / 083 `Decode`。结论：
- 26 个字段宽度全部一致（Evan SP 段由 `has_sp_table` 分流，round95 已补）；
- 与 083 的差异只有 3 处，全是 s9 有意改造：`level 1B→2B`、`exp 8B→4B + tail4`、`gachaexp`。
零代码改动。

## 3. `skill/D11` —— 三钮不是 083 的东西

orange-wz 128B 实测 `UIWindow.img/Skill` 的直接子节点：
`backgrnd / skill0 / skill1 / line / BtSpUp / Tab / tip0..tip2 / CoolTime / recommend`
—— **没有 `main`**。所以 `main["BtHyper"] / main["BtGuildSkill"] / main["BtRide"]` 三条在 083 上
恒为空，`classic = !usable(Skill["main"])` 也恒为 true。

083 `CUISkill::OnCreate @0x8aaa88` 里真正创建的按钮是 `id 2010..2013`
（x=119/159/199/239，y=131，步进 40，`Unknown` 从 119 起、`< 279` 止 → 4 个）、
`id 2023`（x=120, y=265）、页签 `2000/2001`。全库 `*Hyper*` / `*GuildSkill*` 符号命中 0。

→ 三钮属非 083 扩展。按 m09729「扩展功能可以不对齐」保留占位 + DISABLED，
不删枚举（`send_key` switch 与 `buttons` 索引耦合），不发明 083 行为。

## 4. `corewindow/D10` —— 083 是硬编码 666×524

- `CreateWorldMapDlg @0x9eb366:139`：`CDialog::CreateDlg(this, 67, 0, 666, 524, 10, 1, 0)`
- `OnCreate @0x9eb75b`：`sub_9ECC4A(12, 34, 642, 472)` → 内容 `(12,34) 642×472`
- 即 `666 = 12 + 642 + 12`、`524 = 34 + 472 + 18`，两侧各留 12px 内边距。

本端由 `Border` 贴图件推导：`640×470` + `top33/bot18/side7` → `654×521`，内容 `(7,33)`。
`Border` 件实测两端一致（`0=60×33 / 1=1×33 / 2=7×32 / 3,4=7×1 / 5,7=7×18 / 6=1×18`）。

差 12×3。若照搬 666×524 而边宽仍 7px，贴图与内容之间会留 5px 空隙（视觉破坏）。
→ 不改几何，只把差值来源钉在代码注释里；真机对齐项记 PLT §55-3。

## 5. 验证与算术

- 源码侧只有 `UISkillBook.cpp` / `UIWorldMap.cpp` 两处注释，**零行为改动**。
- 算术不变：`168 = 76+38+14+40`；`27 = 已注销10 + 残余17`；`17 = P1 0 + P2 0 + P3 16`；
  P3 完成 3 / 剩 13（A 余 6 + B 5 + C 2）。
