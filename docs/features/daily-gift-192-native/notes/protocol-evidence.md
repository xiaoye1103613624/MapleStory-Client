# 192 原生 28 天每日礼物（dailyGift）协议取证

> 只读逆向取证。记录时间 2026-11-23（Asia/Shanghai）。
> 目标：为「把 192 原生 28 天每日礼物补成真窗」定协议——领奖 / 杀怪计数 / 日限 / MVP 等级。
> 证据等级：**观察**（IDA 反编译 / XML / javap 常量池直出）/ **推断**（多项交叉）/ **未知**（取证盲区）。
> 本次**仅写本文件**，未改仓库其它文件；临时脚本/反编译产物在 `%TEMP%/dg192/`（未入库）。

---

## 0. 证据源与工具状态（先说工具结论）

| 源 | 路径 | 可用性 | 备注 |
|---|---|---|---|
| **192 客户端 IDA** | `E:/MXD/【2023.8.8江浩生日福利】怀旧岛192整合版/怀旧岛192客户端/冒险岛online192/MapleStory.exe.i64` | ❌ **不可用** | 该 .i64 是**加壳/混淆态**：headless auto-analysis 仅识别到 3 个函数（`start` / `sub_149C88009` / `sub_149C8804C`，位于 0x149C88000 段），无 CUI* 符号、无业务字符串（见 `tools/_ida_192_final_checks.out`）。不能作为逻辑证据。 |
| **活 IDA 实例 13339（关键）** | `E:\MXD\反编译IDA\MapleStoryT.i64`（module `MapleStoryT.exe`，imagebase 0x400000，14409 条字符串缓存） | ✅ **可用** | 经 `http://127.0.0.1:13339/mcp`（ida-pro-mcp 1.0.0，66 tools）只读查询。**这份才是带完整符号的 192 原生 IDB**：`CUIDailyGift` / `DAILYGIFTDATA` / `DAILYGIFTMVPBONUS` / `CAttendanceUIMan` 全套具名。本报告客户端侧结论均出自它。 |
| 192 服务端 jar | `MapleServer192/config/lib/Server.jar`（1491 class，Java+Kotlin 混合） | ⚠️ **部分可用** | 字节码被**刻意混淆**（常量池索引越界，如访问 #17628 而池仅 46 项），CFR 0.152 / javap -c 均无法生成控制流；但**常量池字符串完整**，可证类/方法/字段/SQL/提示语结构。 |
| 192 UI 资源 XML | `MapleServer192/wz/UI.wz/UIWindow4.img.xml` | ✅ 可用 | `dailyGift` 整棵节点（行 46836 起）明文可读，含 300 只/等级门提示与 mvpLv/mvpBonus 子结构。 |
| 本端 s9 客户端 | `E:/project/MapleStory-Client`（本仓库） | ✅ 可用 | 0x3717/0x3718 现有签到私有包实现，见 §6。 |
| REA / orange-wz MCP | — | 未接入 | 本协议取证以 IDA + 服务端常量池 + XML 三证为主，未依赖 WZ 节点改写（只读）。 |

> 说明：13339 的 `MapleStoryT.i64` 与侧栏折叠任务（`<REDACTED_RELATIVE_PATH> 13339`）是**同一份 IDB**，其 192 原生身份已被该文档证实；本次直接复用其符号基建。

---

## 1. 窗口类 / 函数 / OnButtonClicked 调用链（客户端 13339）

### 1.1 类名确证

**原生类名 = `CUIDailyGift`**（单例 `TSingleton<CUIDailyGift>`），不是 `CUIEventGift` / `CDailyGift`。

关键交叉证据（IDA `xrefs_to`）：
- 字符串 `"UI/UIWindow4.img/dailyGift"` @0x24d90e4 ← 唯一代码引用 `0x1199e53`，属于 `??0CUIDailyGift@@QAE@XZ`（构造函数 @**0x1199d70**，size 0x3bb）。
- 字符串 `"/mvpMedal/"` @0x24d911c / `"/mvpLv/"` @0x24d915c / `"/mvpBonus/"` @0x24d9150 / `"/mvpLv/backgrnd"` @0x24d9164 ← 分别被 `OnCreate`、**`ShowMVPBonusToolTip@CUIDailyGift`** 引用。
- 结构体：`CUIDailyGift`（size 4448，22 方法）、`DAILYGIFTDATA`（size 100）、`DAILYGIFTMVPBONUS`（size 12）、`ATTENDANCECHECKREWARDDATA`（每日奖励项）、`ZMap<long,DAILYGIFTMVPBONUS,long>`（MVP 等级→加成表）。

### 1.2 方法表（`CUIDailyGift` 全部具名方法，IDA 地址）

| 地址 | 符号 | 作用 |
|---|---|---|
| 0x117F7D0 | `TSingleton<CUIDailyGift>::GetInstance` | 单例 |
| 0x1199510 | `OnDestroy` | — |
| 0x1199590 | `SetDailyGiftInfo()` | **从 DAILYGIFTDATA 取当日奖励填 m_RewardData**（含日期/clearDay） |
| 0x1199730 | `CheckDailyGiftReward()` | **领奖前置校验总闸**（客户端侧门控），调 `CAttendanceUIMan::CheckDailyGift(bAlert=1,nOption=7)` |
| 0x1199B40 | `ShowRewardToolTip(ATTENDANCECHECKREWARDDATA&,int,int)` | 每日格子 tooltip |
| 0x1199D70 | `CUIDailyGift::CUIDailyGift()` | 构造，绑定 `UI/UIWindow4.img/dailyGift` |
| 0x119A130 | `OnCreate(void*)` | 建窗/按钮（引用 mvpMedal/mvpBonus/mvpLv） |
| 0x119B120 | `Draw(tagRECT const*)` | 绘制 |
| 0x119C350 | `ShowMVPBonusToolTip(DAILYGIFTMVPBONUS&,int,int)` | **MVP 额外奖励 tooltip** |
| 0x119D100 | `OnMouseMove(int,int)` | hover |
| 0x119D240 | **`OnButtonClicked(unsigned int)`** | **领取按钮（id=1001）点击处理 → 发 C→S 包** |

### 1.3 领奖按钮调用链（反编译摘要）

`CUIDailyGift::OnButtonClicked(nId)` @0x119D240（伪代码精简）：

```c
if ( nId == 1001                                      // 领取按钮 id，来自 UIWindow4 dailyGift/button:Reward id=1001
  && CWvsContext::CanSendExclRequest(500, 0)          // 互斥请求 500ms 节流
  && CUIDailyGift::CheckDailyGiftReward(this) ) {     // ← 总闸（见 §3/§4）
    if ( CUtilDlg::YesNo(StringPool(0x2A9C), ...) == 6 ) {  // 二次确认弹窗（IDYES）
      CHATLOG_ADD(StringPool(0x2A9C?), ...);          // 聊天提示
      COutPacket oPacket(710);                        // ← C→S opcode 710 = 0x2C6
      bool cash = m_RewardData.bCash != 0;
      int  nItemID = m_RewardData.nItemID;
      if ( cash ) {                                   // 现金道具：查 CommoditySN 换成真 itemId
        CWvsContext::GetCommodityBySN(m_RewardData.nCommodityID, &pCSC);
        if ( pCSC.p ) nItemID = pCSC.p->nItemId.GetData();
      }
      oPacket.Encode4(nItemID);                       // ← 唯一 payload：int itemId
      CClientSocket::SendPacket(&oPacket);
      CWvsContext::SetExclRequestSent(1);
    }
}
```

> **关键结论**：客户端领奖只发 **1 个 int（期望 itemId）**，不发「day」、不发「kill count」、不发「mvp level」。所有门控由客户端本地预判 + 服务端校验后发奖励。XML 中 `button:Reward` 的 `ToolTip_Disabled` 即「等级范围怪物 300 只」提示（见 §3）。

### 1.4 窗口资源对照（交叉证据）

`UI/UIWindow4.img/dailyGift`（XML 行 46836）：
- `backgrnd` 741×647 / `backgrnd2` 741×677（行 46837/46841）
- `button:Close` id=**1000**（行 46964/46989）
- `button:Reward` id=**1001**（行 46991/47017），其 `ToolTip_Disabled`：
  `只有今日狩猎的等级范围怪物达到300只以上的角色才能领取礼物。`（行 **47018**，`useDisableToolTip=1`）
- `mvpMedal/1..7`（行 47021–47057，7 档勋章图标）
- `mvpLv/backgrnd,1..7, day_21/23/28/11_01/11_02/11_03/07_01..07_03`（行 47058–47148）
  —— 证实 **MVP 7 档**、**21/23/28 天** 为里程碑日。
- `mvpBonus/1..N/...`（行 47149 起）
- `term/0..9`、`count/0..9`（行 46858/46912，两位数字精灵用于绘制「第N天」/计数）

---

## 2. 领奖 C→S / S→C 包结构与 opcode

### 2.1 C→S（客户端 → 服务端）领奖

| 项 | 值 | 依据 |
|---|---|---|
| opcode | **710 (0x02C6)** | `COutPacket::COutPacket(this:&oPacket, nType:710)` @0x119d325 |
| payload | `int nItemID`（4 字节，小端） | `COutPacket::Encode4(&oPacket, nItemID)` @0x119d382 |
| 节流 | `CanSendExclRequest(500ms)` + `SetExclRequestSent(1)` | @0x119d2a0/0x119d3a3 |

> 注意：这是**原生 192 每日礼物专用 opcode**，与服务端 jar 里那套签到（`SIGNIN_OPERATION`/`SIGIN_INFO`）**不是同一协议**——见 §5。

### 2.2 S→C（服务端 → 客户端）

**分发器**：`CUIContext::OnPacket(nType, CInPacket*)` @0x1180d40，`switch(nType)`：
- `case 1188:` → `CUIContext::OnDailyGift(this, iPacket)` @0x1180df7
- （邻接 1176 `OnAttendanceEventData` / 1177 `OnAttendanceEventUIData` / 1189 `SlidePuzzle`，说明 1188 属于 192 UI 扩展 opcode 段）

→ **S→C opcode = 1188 (0x04A4)**。

`CUIContext::OnDailyGift(CInPacket*)` @0x117fce0（伪代码）：

```c
byte sub = iPacket.Decode1();
if ( sub == 0 )                       // 0 = 下发每日礼物配置数据
   CAttendanceUIMan::SetDailyGiftData(iPacket);          // → DAILYGIFTDATA::Decode
else if ( sub == 1 )                  // 1 = 领奖结果回执
{
   if ( CUIDailyGift::Instance ) CUIDailyGift::SetDailyGiftInfo();   // 刷新窗
   int nRet     = iPacket.Decode4();  // 结果码
   int nItemID  = iPacket.Decode4();  // 道具
   CAttendanceUIMan::NoticeDailyGiftMsg(nRet, nItemID);             // 弹提示
   if ( nRet != 4 ) CWvsContext::SetExclRequestSent(0);             // 清互斥
}
```

**S→C 子包 0（配置）`DAILYGIFTDATA::Decode(CInPacket&)` @0x1027da0 —— 权威结构：**

```
DAILYGIFTDATA:
  FILETIME  ftStart              DecodeBuffer(8)        // 活动起始（UTC FILETIME）
  FILETIME  ftEnd                DecodeBuffer(8)        // 活动结束
  int32     nDayPerMonth         Decode4               // 每月可领天数上限（账号/月日限）
  int32     nPremiumCount        Decode4               // premium(付费)额外可领次数
  int32     rewardCount          Decode4               // 28 天奖励表长度
  repeat rewardCount:
      int32 key(day)             Decode4               // 第 N 天（map key）
      ATTENDANCECHECKREWARDDATA:
          int32 nItemID          Decode4
          int32 nCount           Decode4
          int32 nLimitType       Decode4
          int32 nPeriod          Decode4
          byte  bCash            Decode1
          int32 nCommodityID     Decode4               // 现金道具 SN
  int32     nLevelMin            Decode4               // ★等级门（Client CheckDailyGift 用，见 §3；对应「33」）
  int32     checkQuestCount      Decode4               // 杀怪 quest 表长度
  repeat checkQuestCount:
      int32 jobGroupKey          Decode4               // = JobCode/100（职业群）
      int32 questId              Decode4               // ★该职业群对应的「等级范围怪物 300」计数 quest
  int32     mvpBonusCount        Decode4               // MVP 加成表长度
  repeat mvpBonusCount:
      int32 key(day)             Decode4
      DAILYGIFTMVPBONUS:
          int32 nMVPLevel        Decode4               // 需要的 MVP 等级
          int32 nMVPCountBonus   Decode4               // 数量倍率加成
          int32 nCommodityID     Decode4               // 替换的现金道具 SN
```

（逐行对应反编译 0x1027da6–0x1027f0d；`Encode` @0x1027840 逐字段对称。）

**S→C 子包 1（结果）**：`byte sub=1` + `int nRet` + `int nItemID`。`NoticeDailyGiftMsg(nRet,nItemID)` @0x5be8d0 是 `switch(nRet)` 的文案分派（case 2/3/5… 从 StringPool 取串），nRet 由服务端给出；客户端据此提示（如 4 = 特殊「不发互斥重置」）。

---

## 3. 杀怪计数（「等级范围怪物 000/300」）从哪来

### 3.1 计数源：**服务端 quest 状态（questId），按职业群区分**

`CUIDailyGift::CheckDailyGiftReward` 与 `CAttendanceUIMan::CheckDailyGift` 反编译给出完整链路：

- 服务端在 `DAILYGIFTDATA` 里下发 `mCheckQuest: ZMap<int,int>`（key=职业群 `JobCode/100`，value=questId）。
- 客户端 `CAttendanceUIMan::CheckDailyGift(bAlert, nOption)` @0x5bebc0：
  ```c
  int nTodayClearCount = JobCode / 100;                    // @0x5bec88 职业群
  int nCheckQuest = 0;
  mCheckQuest.GetAt(&nTodayClearCount, &nCheckQuest);      // @0x5bec90 取该群 questId
  if ( level < p->nLevelMin                                // @0x5becb9 等级门
    || (nCheckQuest != 0 && CWvsContext::GetQuestState(nCheckQuest,0) != 2) )  // @0x5becb9
      return 0; // 不可领 + 弹提示
  ```
  → **300 只计数 = 服务端 quest 完成状态（`GetQuestState(questId)==2` 即完成）**，按职业群各一个 questId。**不是纯客户端本地计数**，权威在服务端 quest 进度。

### 3.2 客户端侧「今日记录」：NX Record（防篡改本地记录）

`CAttendanceUIMan::GetDailyGiftRecordInfo` @0x5be010：从 `CharacterData::GetNxRecord(p, nNxRecordID=15, ...)` 读三个键：
- `"day"` → `nClearDay`（本月已领天数 / 当前第几天）
- `"date"` → `nLastClearDate`（上次领取日期 yyyymmdd）
- `"count"` → `nTodayClearCount`（今日已领次数）

并用 `Util::STGetNow` 取当前日期比较：若 `date != 今天`，则跨月/跨日重置 `nClearDay`/`count`（@0x5be26b–0x5be2d2）。

> **归纳**：「今日狩猎等级范围怪物 ≥300」由**服务端 quest 状态**驱动（quest 由服务端计数并同步）；领奖次数/天数由客户端 NX Record(#15) 本地记录 + `DAILYGIFTDATA.nDayPerMonth/nPremiumCount` 上限共同约束。XML `ToolTip_Disabled` 的 300 文案与该门一一对应（XML 行 47018）。

---

## 4. 等级门 / 账号日限 / MVP 额外奖励的服务端判定逻辑

### 4.1 等级门（`nLevelMin`，即题述「33」）

- 判定位置（客户端执行、服务端下发阈值）：`CAttendanceUIMan::CheckDailyGift` @0x5becb9：
  `BasicStat::_ZtlSecureGet_nLevel(m_Data->m_basicStat) < p->nLevelMin` → 不可领。
- 阈值来源：`DAILYGIFTDATA.nLevelMin`（S→C 子包 0 字段，@0x1027e57 解码）。
- **服务端侧**：192 jar 中**未发现** `nLevelMin` / `DAILYGIFTDATA` 字段（常量池检索 0 命中），说明该门在**本客户端实现**；数值 33 应在配置/活动下发数据里（本次未从 jar/xml 拿到明文「33」阈值常量，属**未知**：阈值随 DAIILYGIFTDATA 下发，需抓包或查配置库确认具体值）。

### 4.2 账号日限 / 月限

`CheckDailyGift` 的 `nOption` 位掩码（客户端此处传 7 = bit1|bit2|bit4，@0x11997ad）：
- **bit1**：非主城/非 StarPlanet 地图 + 等级门 + 职业群 quest 完成（@0x5bec20–0x5becb9）。
- **bit2**（月天限）：`nDayPerMonth <= nClearDay` 或当日奖励项 itemId/CommodityID 皆空 → 不可领（@0x5bee5f）；提示 `NoticeDailyGiftMsg(nRet=5)`。
- **bit4**（premium 额外次数）：`v21 = m_bPremium ? nPremiumCount : 1; if (v21 <= nTodayClearCount) return 0;`（@0x5bee86）→ 非付费玩家只能领 1 次/日，付费玩家 `nPremiumCount` 次；提示 `nRet=7`。
- 配套：`nDayPerMonth` / `nPremiumCount` 均由 `DAILYGIFTDATA` 下发。

### 4.3 MVP 额外奖励

判定位置：`CUIDailyGift::CheckDailyGiftReward` @0x1199805–0x119984e（客户端执行加成，服务端下发表）：
```c
int dayKey = m_nClearDay + 1;                                  // @0x119981d 第 N+1 天
DAILYGIFTMVPBONUS b;
if ( p->mMVPBonus.GetAt(dayKey, &b) ) {                         // @0x1199828 查天对应的 MVP 加成
   if ( MVP_Service::GetGrade(pWvs) >= b.nMVPLevel ) {          // @0x119982b/0x1199837 玩家 MVP 等级 ≥ 要求
      if ( b.nMVPCountBonus > 0 ) nCount *= b.nMVPCountBonus;    // @0x1199841 数量翻倍
      if ( b.nCommodityID > 0 ) nCommodityID = b.nCommodityID;   // @0x119984c 换成高级现金道具
   }
}
```
- MVP 等级来源：`MVP_Service::GetGrade(CWvsContext*)` @0xd3ba50（客户端 MVP 服务，等级由服务端 MVP 系统同步）。
- 表来源：`DAILYGIFTDATA.mMVPBonus : ZMap<day, DAILYGIFTMVPBONUS{nMVPLevel,nMVPCountBonus,nCommodityID}>`（S→C 子包 0 下发，解码 @0x1027e9d）。
- UI：`ShowMVPBonusToolTip` @0x119C350 + `mvpMedal/1..7`、`mvpBonus/*` 资源（XML 行 47021/47149）。

> **服务端判定的可取证边界**：192 jar 字节码被混淆，无法还原 handler 控制流；常量池证明「服务端有一套签到/礼包系统」，但它与本章 `CUIDailyGift` 原生协议的字段名不吻合（见 §5）。上述门控逻辑**明确在本客户端代码里执行**，服务端只需：下发 DAILYGIFTDATA（子包 0）、维护职业群 300 计数 quest、维护 MVP 等级、收 710 包校验后发奖。**服务端是否二次校验等级/quest，本次无直接字节码证据，标未知。**

---

## 5. 192 服务端架构 vs 本端 s9：能否对接

### 5.1 192 服务端不是原生 dailyGift 协议的对手端（关键结论）

192 服务端 `Server.jar`（`handling/channel/handler/PlayerHandler` / `server/MapleSignin` / `packet/SigninPacket` / `handling/opcode/*`）是一套**自研的、与客户端 `CUIDailyGift` 不同源的签到系统**：

| 维度 | 192 客户端原生 dailyGift（IDA 13339） | 192 服务端 jar 现有签到 |
|---|---|---|
| 下发 opcode | `1188 (0x4A4)` → `OnDailyGift` | `SendPacketOpcode.SIGIN_INFO`（`SigninPacket.showSigninUI`） |
| 领奖 opcode | `710 (0x2C6)` + `int itemId` | `RecvPacketOpcode.SIGNIN_OPERATION`（`MapleServerHandler.openSigin`） |
| 配置源 | `DAILYGIFTDATA`（ftStart/ftEnd/nDayPerMonth/nPremiumCount/nLevelMin/mCheckQuest/mMVPBonus/mReward） | MySQL `data_signin_reward`（`SELECT * FROM data_signin_reward`）→ `SiginRewardInfo(rank,itemid,quantity,expiredate,isCash)` |
| 记录 | 客户端 NX Record #15（day/date/count）+ 职业群 quest | `MapleSigninStatus`（`count=1;day=N;date=yyyyMMdd`）+ `characters.lastattendance` |
| 计数 | 职业群 quest 完成状态 | `SigninPacket$SIGNIN_TYPE{UNKNOWN,签到窗口,领取奖励}` |
| 附加表 | `ZMap<day,DAILYGIFTMVPBONUS>` | `getUnknownMap()`、`reward_gift` 表、`星星礼物奖励`/`右边礼包奖励领取` |

- 常量池检索 `nDayPerMonth|nPremiumCount|mMVPBonus|nMVPLevel|nMVPCountBonus|nLevelMin|DAILYGIFTDATA|GetDailyGiftRecordInfo`：在 1491 个 class 中**0 命中**。
- 检索 `710 / 0x2c6 / 1188 / 0x4a4`：无对应 opcode 常量；服务端仅有 `SIGIN_INFO` / `SIGNIN_OPERATION` / `REWARD_GIFT`（星星奖励，`packet/RewardPacket.StarRewardInfo`）——那是**另一套**「星星礼物」系统。

**结论：192 服务端 ≠ 本客户端原生 dailyGift 的原生对手端。若照搬 192 服务端签到协议（SIGIN_INFO/SIGNIN_OPERATION）喂给客户端 0x4A4，客户端不会认。**

### 5.2 与 083 / 本端 s9 服务端协议能否对接

- 本端是 C++ 客户端 + **本端 s9 服务端**（在别处，本仓库仅客户端）。本端已用**私有扩展 opcode 段 0x3717/0x3718** 做「每日签到」28 天窗（`UIDailyCheckin`），其结构：
  - C→S `0x3717`：`byte action`（0 OPEN / 1 CLAIM）+ `byte day(1..28)`（`Net/Packets/DailyCheckinPackets.h:13-31`）。
  - S→C `0x3718`：`byte resp=1, int8 currentDay, int32 claimedMask, int8 justClaimed, byte count, count×int iconId, count×string tooltip`（`Net/Handlers/WaveAShellHandlers.cpp:3383-3410`）。
- 192 原生 `CUIDailyGift` 用 **710/1188**，与本端 s9 现有 0x3717/0x3718 **不兼容**（opcode 与 payload 全不同）。因此**不能**直接把 192 原生协议接到本端 s9 服务端；本端应继续走 0x3717/0x3718 自有私有包，或另开新私有 opcode。

### 5.3 「按本端 s9 现有 opcode 段自定私有包」最小可行方案（建议，供 lead 决策）

沿用本端既有做法（0x3717/0x3718 签到、0x3719 魔女森林、0x371B 突破石、0x371D 强化族等私有扩展段），**不建议照搬 192 的 710/1188**：

1. **opcode 槽位**：0x37xx 段已占用 0x3714–0x371E、0x3730、0x373D/0x373F、0x3741/0x3743/0x3745。建议 **C→S 0x3721、S→C 0x3722**（0x371F/0x3720 空位优先；若服务端 keymap 已占则顺延到 0x3747/0x3749），与本端 `OutPacket::Opcode` / `PacketSwitch` / `WaveAShellHandlers` 三处同步登记（参照 `DailyCheckinPackets.h` + `DailyCheckinHandler`）。
2. **C→S 领奖包（自定，byte 门控下沉服务端）**：
   `byte action`(0 OPEN / 1 CLAIM) + `byte day(1..28)`（与 0x3717 同构，服务端可按天校验，不再只收 itemId）。
3. **S→C 快照包（服务端权威下发，模拟 DAILYGIFTDATA）**：
   `byte respType`(0 SNAPSHOT / 1 CLAIM_RESULT) + 若 SNAPSHOT：
   `int levelMin` (=33) + `int dayPerMonth` + `int premiumCount` + `byte count` + count×{`int day`,`int itemId`,`int qty`,`int limitType`,`int period`,`byte cash`,`int commodityId`} + `int questId` + `int mvpGrade` + `byte mvpBonusCount` + mvpBonusCount×{`int day`,`int reqMvpLevel`,`int qtyMul`,`int commodityId`}；
   若 CLAIM_RESULT：`byte ok` + `int day` + `int itemId` + `int qty`。
   —— 把 §2.2 的 `DAILYGIFTDATA` 字段**平铺进私有包**，服务端一次下发，客户端 UI 按字段渲染（无需 192 的 `CUIDailyGift` 二进制）。
4. **杀怪计数 300**：**服务端维护职业群 quest 进度**（照 192 思路），在快照里下发 `killCount`/`questId` + `need=300`；客户端只显示门槛，不本地判定。若本端服务端无 quest 机制，可用自建计数表 + 每日 0 点重置（参照 192 `Other_AutoSave.js:resetDaySignLog` 的定时重置范式）。
5. **等级门 33 / 日限 / MVP**：全部在**服务端 CLAIM handler 里判定**（本端 s9 服务端），opcode 用私有段；客户端仅展示服务端下发的状态位。这样既符合本端「服务端权威」惯例，也绕开 192 客户端本地 `CheckDailyGift` 的不可移植性。
6. **资源**：UI 走本端 `UIDailyCheckin` 现有 28 格窗（已按 192 原生栅格还原），或新开 `CUIDailyGift` 风格窗；img 必须经 orange-wz `batch_convert_key(GMS,verify)` 入库（见 AGENTS.md 跨端换钥门禁），禁止裸拷 192 UIWindow4.wz。
7. **不动现有功能**：新增 opcode/UI 不触碰 0x3717/0x3718 现有签到与 0x3719/0x371B 等私有包；灰度用配置开关。

> 待 lead 拍板：A) 复用现有 0x3717/0x3718「签到窗」扩充到 28 天真礼物（最小改动），还是 B) 新开 0x3721/0x3722 + 新 UI（与现有签到并存）。**推荐 A**（既有端到端链路已通，仅扩字段与服务端门控）。

---

## 6. 本端现有私有包实现（可直接复用的落点）

| 文件:行 | 内容 |
|---|---|
| `Net/OutPacket.h:313` | `DAILY_CHECKIN = 14103 (0x3717)` 注释 `byte action[+byte day]` |
| `Net/PacketSwitch.cpp:328-330` | `DPT_TRACKER=0x3714 / CHAT_EMOTICON=0x3716 / DAILY_CHECKIN=0x3718`（S→C） |
| `Net/Packets/DailyCheckinPackets.h:13-31` | `DailyCheckinOpenPacket`(write_byte 0) / `DailyCheckinClaimPacket`(write_byte 1 + write_byte day) |
| `Net/Handlers/WaveAShellHandlers.cpp:3383-3410` | `DailyCheckinHandler::handle` 解析 0x3718 快照（resp/currentDay/mask/justClaimed/count/icons/tooltips） |
| `IO/UITypes/UIDailyCheckin.h:2-39` | 28 天窗（DAYS=28, 7×4 栅格），`apply_snapshot(cd,mask,jc,icons,tips)` |
| `IO/UITypes/UIDailyCheckin.cpp` | 按 v4 整窗栅格 + cell 状态图渲染 |

---

## 7. 证据索引（可复核）

### IDA 13339（MapleStoryT.i64）反编译地址
- `CUIDailyGift::OnButtonClicked` 0x119D240（710/Encode4）
- `CUIContext::OnPacket` 0x1180d40（`case 1188`）
- `CUIContext::OnDailyGift` 0x117fce0（sub0=配置 / sub1=结果）
- `DAILYGIFTDATA::Decode/Encode` 0x1027DA0 / 0x1027840
- `DAILYGIFTMVPBONUS::Decode/Encode` 0x1023370 / 0x1027810
- `CAttendanceUIMan::CheckDailyGift` 0x5bebc0（等级/月限/quest 门）
- `CAttendanceUIMan::GetDailyGiftRecordInfo` 0x5be010（NX Record #15）
- `CAttendanceUIMan::SetDailyGiftData` 0x5bf570（配置落地）
- `CAttendanceUIMan::NoticeDailyGiftMsg` 0x5be8d0（结果码文案）
- `CUIDailyGift::SetDailyGiftInfo` 0x1199590 / `CheckDailyGiftReward` 0x1199730 / `ShowMVPBonusToolTip` 0x119C350
- 字符串锚点：`"UI/UIWindow4.img/dailyGift"` 0x24d90e4、`"/mvpLv/"` 0x24d915c、`"/mvpBonus/"` 0x24d9150

### 192 服务端 jar（常量池，类:串）
- `server/MapleSignin.class`：`SELECT * FROM data_signin_reward`、`getSiginRewards`、`MINLEVEL`
- `packet/SigninPacket.class`：`showSigninUI`、`SIGIN_INFO`、`SIGNIN_TYPE{UNKNOWN,签到窗口,领取奖励}`、`getSigninReward(领取奖励)`
- `handling/opcode/RecvPacketOpcode.class`：`SIGNIN_OPERATION`；`SendPacketOpcode.class`：`SIGIN_INFO`/`REWARD_GIFT`
- `handling/channel/handler/PlayerHandler.class`：`每日签到第`、`领取每日签到必须在线`、`每日签到：无效的奖励。`、`SELECT * FROM reward_gift`、`星星礼物奖励`
- `handling/MapleServerHandler$1.class`：`SIGNIN_OPERATION`
- `constants/GameConstants.class`：`每日签到系统_签到记录` / `_当前时间` / `每日签到`
- `server/MapleSignin$SiginRewardInfo.class`：`rank,itemid,quantity,expiredate,isCash`

### 资源
- `MapleServer192/wz/UI.wz/UIWindow4.img.xml`：`dailyGift` 节点 行 **46836**；300 只提示 行 **47018**；`mvpLv` 行 47058；`mvpBonus` 行 47149；`button:Reward id=1001` 行 47017；`button:Close id=1000` 行 46989。

### 工具/脚本（临时，未入库）
- `%TEMP%/dg192/`：`cfr.jar`、`all_cp.txt`（1491 class 常量池全量）、`decomp_out.txt`、`gate_out.txt`、`last.txt`、`ida_*.py`（13339 MCP 只读查询封装）。
- 本仓库现成：`tools/_ida_192_*.py/out`（加壳 .i64 的失败证据）、`tools/_ida_mcp.py`（13339 客户端范式）。

---

## 8. 诚实边界 / 未知

1. **13339 IDB 是 `MapleStoryT.i64`**（v83 系带符号），与「192 客户端 .i64」不同源；但侧栏任务已确认它就是 192 原生 UI 家族的权威符号库，本次 dailyGift 全套具名印证这一点。若 lead 要求「必须来自 `MapleStory.exe.i64`」，那一份已证实加壳不可反编译（§0），只能用 13339 替代。
2. **等级门「33」的确切数值**未从 jar/XML 拿到明文常量；它来自服务端下发的 `DAILYGIFTDATA.nLevelMin`，需抓包/查配置确认具体值（本次标未知，但门控代码位置确定）。
3. **服务端二次校验**：192 jar 混淆致 handler 控制流不可读；服务端是否对 710 包做 level/quest/MVP 二次校验无直接证据，仅能从客户端本地门控 + 客户端只发 itemId 推断「服务端应做权威校验」。本端方案（§5.3）已按「服务端权威」设计，不依赖该未知。
4. **未做真机联调 / 未抓包**：opcode 710/1188 与包结构来自 IDA 静态反编译，未经运行时验证；S→C 子包 1 的结果码枚举（nRet 值含义）仅部分从 `NoticeDailyGiftMsg` 的 StringPool 索引推得，完整码表未知。
5. **未写任何 WZ/IMG**：本报告纯协议取证；UI 资源移植（换 key / orange-wz）不在本次范围。
