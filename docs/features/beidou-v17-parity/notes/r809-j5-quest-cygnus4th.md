# r809 · J5：任务数据/版式落地 + 骑士团四转「空壳」的证据化结论

对应任务 #72。日期：2026-11-01。

## 1. 已落地的改动（本段未提交，`git diff` 可复核）

| 面 | 文件 | 改了什么 | 为什么 |
| --- | --- | --- | --- |
| 任务数据 | `Data/QuestData.{h,cpp}`（+68/-…） | ① 文案根从 `strsrc` 改回 `info`；② 新增 `area` 字段与 `get_area()` | 北斗包的 `QuestInfo` 把 `name`/`summary` 直接挂在 `info` 下，旧代码按 `qsrc["0"]["info"]` 取，取不到就整条任务空白；`area` 是「寻找NPC位置」分区的唯一数据来源，原来根本没读 |
| 任务窗版式 | `IO/UITypes/UIQuestLog.cpp`（+1110/-377） | 命中宽、展开/收缩、按区域分组标题行可点、正文折行与上下缘裁剪、列表底衬右缘收进滚动条内侧 | 对应 CHECKLIST §J.4 的 J4-1..J4-7 |
| 出站封包 | `Stat.SKIN` 写宽 | 服务端按**掩码值分档**决定宽度：`Stat.SKIN = 0x1` 走 `writeByte`，不是 `writeInt` | 皮肤色一旦按 int 写，后面所有 statup 的读位整体错位，表现为「换装后属性窗乱码/掉色」 |
| 角色/属性窗 | `IO/UITypes/UIStatsInfo.cpp`、`UICharInfo.cpp`、`Net/Handlers/PlayerHandlers.cpp`（+197/-19） | 职业名表与家族判定接入 `Job` 改动；属性窗版式 | 职业名表证据内联在 `Character/Job.cpp::get_name()` 注释里 |

## 2. J5-⑥：骑士团四转是**数据侧空壳**，不是客户端 bug

排查「骑士团转完 4 转技能窗空白 / 学不到技能」时，先怀疑客户端命名空间位数（J2 已修），
但真因在**服务端 WZ 数据**：

```
gms-server/wz/Skill.wz/
  1110.img.xml   50,410 B
  1111.img.xml  134,606 B      <- 三转有内容
  1112.img.xml      278 B      <- 四转
  1211.img.xml  132,184 B   /  1212.img.xml    278 B
  1311.img.xml   71,322 B   /  1312.img.xml    278 B
  1411.img.xml   89,975 B   /  1412.img.xml    278 B
  1511.img.xml   88,062 B   /  1512.img.xml    278 B
  （对照：战神 2111 88,817 B、2112 118,656 B —— 战神四转是满的）
```

五个 278 字节逐字相同，内容只有：

```xml
<imgdir name="1112.img">
  <imgdir name="info">
    <canvas name="icon" width="26" height="30">
      <vector name="origin" x="-4" y="30"/>
    </canvas>
  </imgdir>
  <imgdir name="skill">
  </imgdir>          <!-- 空 -->
</imgdir>
```

⇒ `skill` 子树为空 ⇒ 四转技能**一条都没定义**。客户端再怎么修命名空间也变不出技能来。

再叠一层等级上限（同仓 Java 证据）：

- `org/gms/client/Character.java:5011` —— `return isCygnus() ? 120 : 200;`
- `org/gms/constants/game/GameConstants.java:488` —— `(job.getId() / 1000 == 1) ? 120 : 200;   // 4th job: cygnus is 120, rest is 200`

⇒ 骑士团练到 120 就顶格，且顶格后拿到的四转职业（1112…1512）没有可学技能。

**结论**：骑士团「四转」在当前参考端是**刻意留白的半成品内容**，属于资源缺口，不属于客户端缺陷。
本轮不做任何"看起来修好了"的补偿（不伪造技能、不改上限），只把判据钉死在这里。
要真闭环需要外部资源：把 `Skill.wz/1x12.img.xml` 按对应版本补全，或明确接受「骑士团止于 120 级三转」。

## 3. 仍需真机回归的（J5-④⑤）

| # | 场景 | 判据 |
| --- | --- | --- |
| ④ | 冒险家 1→4 转 | 转职后技能窗按新命名空间出技能；`can_use()` 放行对应族技能；属性窗职业名正确（名表证据见 `Character/Job.cpp::get_name()`） |
| ⑤ | 骑士团 1→3 转 | 同上，且**不要**去验 4 转（§2 已定为数据空壳） |

已并入 CHECKLIST §K.2。
