# Wave 2 · 2-16 · SHOW_STATUS_INFO mode 5 语义错位（金币当日志 SP）

- 日期：2026-11-10（round49）
- 授权：m04292「可以自测的自己测试，遇到客户端占用直接关闭，功能测试完成就提交代码，不能自测的记录下来后面我一起测试」
- 范围：`Net/Handlers/MessagingHandlers.cpp` 单个 handler 的 mode 5 分支；不改协议字节、不改 WZ/IMG/XML、不改 opcode 矩阵
- 结论：**改前把「聊天窗金币提示」解析成「获得 SP」，并弹技能加点窗**；已按 s9 + Angel 083 双端证据改正

## 1. 缺陷逐字

改前 `Net/Handlers/MessagingHandlers.cpp`（`ShowStatusInfoHandler::handle`，opcode `SHOW_STATUS_INFO = 39`）：

```cpp
else if (mode == 5)
{
    // SP gain（短/整均可；优先 short）
    int32_t sp = recv.available() >= 2 ? recv.read_short() : 0;
    if (recv.available() >= 2)
        recv.read_short(); // job（部分包）
    std::string tip = "You have gained SP (+" + std::to_string(sp) + ")";
    ...
    auto su = UI::get().get_element<UISkillUp>();   // 弹技能加点窗
    su->set_tip(tip);
}
```

三处错：① 服务端写的是 4 字节 `int`，这里按 `short` 读，只吃掉低 2 字节；② 读法错导致 `sp` 值是垃圾（例：`gain = 2147483647` → 读到 `-1`，`gain = -2147483648` → 读到 `0`）；③ 语义完全错——金币提示被显示成「You have gained SP」并弹出 `UISkillUp`。

## 2. 三方证据

### 2.1 s9 服务端写点（权威）

`E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java`

```java
// :1713 注释：3 = exp, 4 = fame, 5 = mesos, 6 = guildpoints
// :1763-1773
public static OutPacket getShowMesoGain(int gain, boolean inChat) {
    ...
    if (!inChat) { p.writeByte(0); p.writeShort(1); }   // v83 老式状态栏
    else          { p.writeByte(5); }                   // ← 聊天窗金币
    p.writeInt(gain);
    p.writeShort(0);
}
```

即 `mode = 5` 后紧跟 **4 字节 int**（+ 2 字节尾），与「SP」无关。

发点（证明是高频路径，不是边角）：

- `Character.java:3227-3234 spendMesoTransactionally` → `getShowMesoGain(-cost, true)`；调用者 3 处：`guild/Alliance.java:138`（建盟扣费）、`Alliance.java:1017`（购买）、`client/command/commands/gm0/MapWarpCommand.java:93`
- `gainMeso(..., true)` 侧：`server/quest/actions/MesoAction.java:57/60/62`（任务金币奖励）、`util/packets/Fishing.java:100`、`server/minigame/RockPaperScissor.java:23`、`server/maps/PlayerShop.java:298`、`MatchCheckerGuildCreation.java:120`、`GuildOperationHandler.java:242`

### 2.2 Angel 083 反编译（IDB `E:\MXD\反编译IDA\v83\Angel.i64`）

`CWvsContext::OnMessage @0xa209d4` 分派表：

| case | 处理函数 | 语义 |
| --- | --- | --- |
| 0 | `OnDropPickUpMessage @0xa20ad9` | 拾取 |
| 1 | `OnQuestRecordMessage` | 任务 |
| 2 | `OnCashItemExpireMessage` | 现金道具到期 |
| 3 | `OnIncEXPMessage @0xa21ac5` | EXP |
| **4** | `OnIncSPMessage @0xa2212d` | **人气（fame），1×int** |
| **5** | `OnIncMoneyMessage @0xa221f3` | **聊天窗金币，1×int** |
| 6 | `OnIncGPMessage @0xa222c9` | 公会点，1×int |

`OnIncMoneyMessage @0xa221f3`（0xd6 字节）：

```
v2 = CInPacket::Decode4(a2);          // ← 4 字节 int
if (v2 <= 0)  Format(SP_292_YOU_HAVE_LOST_MESOS_D)
else          Format(SP_290_YOU_HAVE_GAINED_MESOS_D)
CHATLOG_ADD(nType: 7)                 // 只进聊天窗日志
```

**没有任何 UISkillUp / SP 相关调用**，且只写聊天窗（`CHATLOG_ADD`），与客户端「非 chat 路径抑制」的设计同向。

### 2.3 SP 在两端都不走本包

- 服务端：`AbstractCharacterObject.java:783-797 changeRemainingSp → gainSp → changeStatPool`，走 `STAT_CHANGED` / `updateSingleStat(Stat.SP)` 通道；`gainSp(..., true)` 的 `silent=true` 语义也与本包无关。**s9 从不发「mode 5 = SP」的包。**
- 客户端：SP 已有独立入口 —— `Net/Handlers/PlayerHandlers.cpp:189-199 ChangeStatsHandler::need_skillbook_update` 含 `case MapleStat::Id::SP: return true;`；另有 `Net/Handlers/SetFieldHandlers.cpp:303` 掩码、`Net/Handlers/Helpers/LoginParser.cpp:149` 登录读 SP、`IO/UITypes/UISkillBook.cpp:896/:947` 消费。
- 因此删除 mode 5 里的 `UISkillBook` / `UIStatsInfo` / `UISkillUp` 三分支**不会丢功能**——那是错的入口，SP 刷新由 `ChangeStatsHandler` 负责。

## 3. 最小修复

`Net/Handlers/MessagingHandlers.cpp`（1 文件，17 insertions / 32 deletions）：

```cpp
else if (mode == 5)
{
    // mode 5 = 聊天窗金币提示，不是 SP。三方证据：…
    int32_t gain = recv.read_int();
    while (recv.available())
        recv.skip_byte();
    std::string tip = gain <= 0
        ? ("You have lost mesos ("  + std::to_string(gain) + ")")
        : ("You have gained mesos (+" + std::to_string(gain) + ")");
    // inChat 语义即聊天窗提示（Angel CHATLOG_ADD），不做 StatusBar/Stats 抑制
    if (auto chatbar = UI::get().get_element<UIChatBar>())
        chatbar->show_message(tip.c_str(),
            gain <= 0 ? UIChatBar::MessageType::RED : UIChatBar::MessageType::WHITE);
}
```

设计取舍：

- **只读 `int`，不读 `short` 双段**：s9 只写一个 int；Angel 也只 `Decode4`。原 `read_short ×2` 的「job」注释没有服务端依据。
- **负数为「失去金币」红色**：对齐 Angel `SP_292_YOU_HAVE_LOST_MESOS_D`。
- **只走聊天窗**：`inChat=true` 就是聊天窗语义（Angel `CHATLOG_ADD`），不再抑制。
- **不用 `show_status`**：那是 status bar 的 `mode 0/0/1` 老式通道，mode 5 是 chat 通道，混用会在 StatusBar 已开时被抑制而丢提示。

## 4. 自测与门禁

- `tools/test_showstatusinfo_mode5.py`：**15 静态守卫 + 6 场景**
  - `PASS: 15 static guards + 6 scenarios (before mislabels 6/6 -> after 0; stable 6/6)`，`TEST_RC=0`
  - 场景含边界：`gain = 2147483647` 改前读成 `-1`、`gain = -2147483648` 改前读成 `0`——正是 short 截断的真实形态
  - 守卫覆盖：`read_int` 读法、无 `read_short`、无 SP 文案、无 `UISkillUp`、金币双文案、仅 `UIChatBar`、亏损红色、余量吞尽、3 条引用注释、`ChangeStatsHandler` 仍持 SP、mode 6 GP 未被误伤、s9 `writeByte(5)` 原文可对照
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`
- `git diff --check HEAD` → 干净
- `tools\build_smoke.bat`（`MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c`）→ `GATE OK` ×2 + `EXITCODE=0` + `BUILD_RC=0`；`build_log.txt` `04:38:46 / 5256B` → `10:21:30 / 4775B`，首行即 `MessagingHandlers.cpp`（真重编译），`ERROR_LINES 0`，尾行 `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- harness 自身两处 bug 已就地修（不计任务失败）：① 场景未先消费 mode 字节 → 读成 `gain + (5<<16)`；② 守卫按裸子串判定，被自己新写的注释（含「gained SP」「UISkillUp」）误触 → 改为去注释后只判代码行

## 5. 范围边界（本轮不动）

- 不动 `mode 0 / 1 / 3 / 4 / 6` 分支
- 不动 `SHOW_ITEM_GAIN_INCHAT`（另一 opcode）与 `getShowInventoryStatus` 通道
- 不动 SP 的 `STAT_CHANGED` 链路
- 不新增 opcode、不改 `opcode-matrix.json`

## 6. 真机 backlog

无强制项（纯文本提示路径，harness 已覆盖读法与文案）。可选观察：

1. 任务金币奖励 / 商店售出后聊天窗出现 `You have gained mesos (+N)` 且**不再**弹技能加点窗
2. `MapWarpCommand` 扣费出现红色 `You have lost mesos (-N)`
3. 升级获得 SP 时仍由 `ChangeStatsHandler` 正常刷新技能窗（未被本修影响）
4. 大额金币（≥ 65536）数值显示正确，不再出现负数/截断

## 7. 提交 scope

```
M Net/Handlers/MessagingHandlers.cpp
A docs/features/development-and-fix-plan-2026-11/notes/wave2-showstatusinfo-mode5-recheck-20261110.md
M docs/features/development-and-fix-plan-2026-11/README.md          (§4 追加 2-16)
M docs/features/pending-live-tests/README.md                        (追加 §27)
A tools/test_showstatusinfo_mode5.py
```

零署名；不 add `build_log.txt` / `*.obj` / 取证 json。

## 8. 附带发现（本轮未修，交裁决）

编译期 `warning C4804: ">=" 在操作中使用类型 bool 不安全` 暴露了同一文件乃至全仓的**通用缺陷**：

`Net/InPacket.h:37 bool available() const;` + `Net/InPacket.cpp:30-33 return length() > 0;`，而全仓有 **393 处**把它当字节数用（`recv.available() >= 4` / `>= 8` / `< 5` / `== 21` / `recv.skip(recv.available() - 5)`）。`bool` 提升成 int 只有 0/1，于是：

- `>= 2 / >= 4 / >= 8` 恒 false → 这些「短包守卫」全部是死代码
- `< 5 / < 14 / < 24` 恒 true → 长度预检全部误判为「不足」
- `recv.skip(recv.available() - 5)` → `0/1 - 5` 在 `size_t` 下溢

分布：`WaveAShellHandlers.cpp` 205、`MapObjectHandlers.cpp` 85、`CashShopHandlers.cpp` 75、`MessagingHandlers.cpp` 8、`TradeHandlers.cpp` 8，其余 6 文件各 1–3。

一行类型修复即可（`size_t available() const { return length(); }`），但会让 393 处原本恒假的守卫**同时变成活路径**，等于一次性改变几乎所有 handler 的短包行为——按既有立项门槛（确定性可达 + 零协议决策），这属于必须真机回归的协议级裁决，不在本 note 的处理范围内。已登记为待裁决项（见 `pending-live-tests` §27 与 dev-plan §3）。
