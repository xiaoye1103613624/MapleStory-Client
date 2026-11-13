# r01 · Angel 083 活动·小游戏面一手语义（逐字 Hex-Rays）

> 取证源：`E:\MXD\反编译IDA\v83\`Angel.i64`（13339, module `` `Angel.exe ``, imagebase 0x400000, hexrays_ready）。
> 交叉源：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`（13337, 同为 v83）。
> 产物：`evidence/angel-activity-ea.json`（844 具名函数/73 类）、`angel-activity-decompile-full.json`（267 函数逐字伪代码）、`angel-activity-facts.md`（3987 行/189016 B，SP_ 238 个）、`angel-beidou-activity-ea-crosscheck.json` + **`angel-beidou-activity-ea-bytelevel-audit.json`（本面独有补证）**。
> 结论一律区分「观察（逐字伪代码） / 推断（语义归纳） / 未知」。全部只读取证。

## §0 七条硬结论

1. **六面一致性成立（字节级）**：Angel 与 BeiDou 仍是同一份 v83 二进制的不同符号状态。登录 327/328、HUD 547/548、核心窗口 439/441、社交 752/752、宠物 968/968、活动 **844/844（字节级一致）**。crosscheck 报告 `beidou_missing=3 / conclusion="layout differs"` 是**机械误判**（见 §1.1），不是二进制布局差异。
2. **Angel 补的是符号语义层，不是功能/版本**：844 个具名函数中 839 在 BeiDou 侧退化为 `sub_*`（仅 5 个具名），但两端 EA 与 8 字节逐字相同。本面 Angel 提供的是「类/方法名 + 控件 id + 协议 opcode + 字符串节点」，不新增玩法。
3. **小游戏房间是同一个基类派生**：`CMiniRoomBaseDlg`（15 方法）为五子棋 `COmokDlg`/记忆 `CMemoryGameDlg`/猜拳 `CRPSGameDlg` 的共同基类，`OnPacketBase` 用 `Decode1` 一个字节分派；三者都只覆写 `OnKey`/`OnPacket`，**没有独立窗口资源族**（详见 §2）。
4. **活动场地是 `CField_*` 派生族（13 类）**，各自 `Init`/`OnPacket`/`OnClock`，通过 `OnPacket` 的 s2c 包驱动（怪物嘉年华 case 289–296），不是客户端本地 AI（详见 §3）。
5. **活动面 c2s opcode 全集 = 8 个**（267 函数内 `COutPacket::COutPacket(a1: N)` 全扫描）：60 脚本问答 / 107 任务 / 113 道具制作 / 123 迷你房间邀请 / 212 雪球 / 213 椰子 / 215 公会 Boss / 218 怪物嘉年华（详见 §5）。
6. **校准 ≠ 改回原生**：原生是 `CMiniRoomBaseDlg`/`CField_*`/`CUI*Dlg` 单体大窗 + `CScriptMan` 单体脚本机；当前端按 s9 协议 + 组件化重写，非一一对应。s9 适配冲突一律标注「改回原生会断联机，须交用户裁决，不得盲改」。
7. **IMG 层记阻塞（六面一致）**：orange-wz(IMG) MCP 未接入本 DSH Host（`mcp_connector_status` 仅 `json-photoshop`），替代链 = 仓库 `.nx` 节点字面量 + `nx_inspect.py` dump + 正确端截图 `C:\Users\11036\Desktop\正确客户端截图\`。

## §1 取证基线与三联门禁

| 项 | 值 |
|---|---|
| REA（逆向取证） | ✅ 端点 `server_health` 13339 ok/uptime 79508s、13337 ok/uptime 728379s；13338 headless 095 不用 |
| IDA（Angel 一手语义） | ✅ 5 脚本 `tools/ida_angel_activity_{ea,decompile,normalize,crosscheck,facts}.py` 全 exit 0 |
| IMG（资源样式） | ⚠️ 阻塞：orange-wz 未接入 Host；`~/.cursor/mcp.json:3-6` 仅 Cursor 侧声明；已知 `Index 16 out of bounds` 解析缺陷 |
| 采集脚本 | `tools/_gen_activity.py` 从第五面 5 模板生成，py_compile ALL SYNTAX OK，EXACT 73/TARGETS 53/FOCUS 53 |
| decompile | 267/267 OK；normalize `trunc_fixed 3`（`/output/<uuid>.json` 补全）、`empty 1`（`CGrenade::JustBomb`，见 §1.2） |
| facts | 3987 行/189016 B；SP_ 全集 238 个（`UI_UIWINDOWIMG_*` 54 个）；53 类标题 + §3 坐标段 |

### §1.1 ★ 本面独有异常：3 个 missing EA 的根因（推翻 crosscheck 的「layout differs」）

前五面 crosscheck 全部 `0 missing`。本面首次出现：

```json
{"angel_funcs": 844, "beidou_present": 841, "beidou_sub_named": 836,
 "beidou_real_named": 5, "beidou_missing": 3,
 "missing_eas": ["0x462738", "0x5617a1", "0x6cec47"], "conclusion": "layout differs"}
```

`tools/ida_angel_activity_crosscheck.py:107` 的阈值 `same = present >= len(items) - 2  # tolerate <=2 analysis-difference misses` 遇到 3 个缺失即机械判为「layout differs」。双端 `py_eval`（`idc.get_func_attr` / `get_wide_byte`，全部包进 `def _scan()` 规避双作用域 `NameError`）取证：

| EA | Angel 修饰名 | Angel 函数范围 | 8 字节 | BeiDou `get_func_attr` | BeiDou 同 EA 8 字节 |
|---|---|---|---|---|---|
| `0x462738` | `?OnKey@CBookDlg@@UAEXII@Z` | `0x462738..0x46275b` | `[246,68,36,11,128,117,25,131]` | `0xffffffffffffffff`（自动分析未定义为函数） | **逐字相同** |
| `0x5617a1` | `?OnPacket@CField_MassacreResult@@UAEXJAAVCInPacket@@@Z` | `0x5617a1..0x5617c5` | `[129,124,36,4,158,0,0,0]` | 同上 | **逐字相同** |
| `0x6cec47` | `?Update@CNoticeQuestProgress@@UAEXXZ` | `0x6cec47..0x6cec58` | `[86,139,241,232,47,26,49,0]` | 同上 | **逐字相同** |

**结论（观察 + 推断）**：3 个 EA 在 Angel 侧都是带修饰名的函数头，在 BeiDou 侧只是「自动分析未识别为函数」（`get_func_attr` 返回 -1），但**两端该地址的 8 字节逐字相同**。这是纯 IDA 分析覆盖差异，与前五面「同一份 v83 二进制不同符号状态」完全一致；crosscheck 的 `layout differs` 是阈值机械误判。→ 固化为 `evidence/angel-beidou-activity-ea-bytelevel-audit.json`（2884 B），README 按 **844/844 字节级一致** 表述。

### §1.2 CGrenade::JustBomb 反编译空响应 → 反汇编兜底

`0x58fbfc`，Hex-Rays 连续两次返回 `{"addr":"0x58fbfc","code":null,"error":"Decompilation failed at 0x58fbfc (address: 0x58fc17)"}`。该 EA 两端都是函数头（BeiDou `sub_58FBFC`）、字节一致，`disasm` 成功 12 条指令：

```
0x58fbfc push esi
0x58fbfd mov esi, ecx
0x58fbff push dword ptr [esi+44h]
0x58fc02 mov ecx, dword_BE78DC
0x58fc08 call ?GetSkill@CSkillInfo@@QAEPBUSKILLENTRY@@J@Z        @0x75c755
0x58fc0d mov ecx, [esi+10h]
0x58fc10 push esi
0x58fc11 push 320h
0x58fc16 push eax
0x58fc17 call ?ShowSkillSpecialEffect@CUser@@QAEXPBUSKILLENTRY@@JPAVCGrenade@@JJJ@Z  @0x936528
0x58fc1c pop esi
0x58fc1d retn
```

**语义（推断）**：`void CGrenade::JustBomb()` = `((CUser*)*(this+0x10))->ShowSkillSpecialEffect(CSkillInfo::GetSkill(dword_BE78DC, *(this+0x44)), 0x320, this, ...)`。已把带注释的 disasm 兜底写回 `angel-activity-decompile-full.json`（`empty=0`），重跑 facts → 3987 行。

## §2 小游戏房间族（CMiniRoomBaseDlg 及派生）

`CMiniRoomBaseDlg::OnPacketBase(int CInPacket*)` 逐字（单字节分派）：

```
v1 = CInPacket::Decode1(this: a1);
result = dword_BE7910;                    // 当前房间实例单例
if ( dword_BE7910 == 0 ) {                // 无房间（受邀方）
  v3 = v1 - 2;  if (v3==0) return OnInviteStatic(a1);
  v4 = v3 - 1;  if (v4!=0) { v5 = v4-2; if(v5==0) return OnEnterResultStatic(a1);
                              if(v5==9) return OnCheckSSN2Static(a1); return; }
  return OnInviteResultStatic(a1);
}
// 有房间（房内）：虚表调用 vfunc+72 判定，再 switch(v1)
case 3: return OnInviteResultStatic(a1);
case 4: return OnEnterBase(a1);
case 6: return (*(*v6+80))(a1: v6, a2: a1);   // 虚函数：子类 OnPacket
case 9: return OnAvatar(a1);
case 10: return sub_65EDB5(a1);
default: return (*(*v6+64))(a1: v6, a2: v1, a3: a1);   // 子类默认分派
```

- `COmokDlg::OnKey` / `CMemoryGameDlg::OnKey`：逐字**同一份代码**——`if (a3>=0 && a2==13)`（回车）取 `CCtrlEdit::GetText` 聊天输入 → `CMiniRoomBaseDlg::CheckAndSendChat(this: this-1, ...)` → 清空编辑框 + `SetFocus`；`a2==27`（Esc）吞掉，其余 `CDialog::OnKey`。→ **五子棋/记忆游戏在 083 里只有「回车聊天」这一处键盘覆写，棋盘点选/翻牌走鼠标 + 收包**（推断）。
- `CRPSGameDlg::OnPacket(int a1)`：`Decode1` 分派，`v1<6 return`；`v1<=7 → LABEL_23`；`v1==8` → 若 `dword_BE7910!=0` 抛 `_CxxThrowException(...CDisconnectException...)`（`0x21000003`），否则 `Decode4` 报名费 + `SP_3675_EBROCK_PAPER_SCISSORS_NOTICENK...` 文案 + `ZAllocEx::Alloc(1768)` 造 `CUtilDlgEx` 提示框。→ **猜拳报名费/通知是服务端下发、客户端弹 CUtilDlgEx**（观察）。
- `CBookDlg`（9 方法，含 `OnKey @0x462738` 即 §1.1 的 missing EA 之一）：留言册/配对本类，Angel 有修饰名，BeiDou 未识别为函数。

## §3 活动场地族（CField_* 13 类，服务端包驱动）

- `CField_MonsterCarnival::OnPacket` 逐字 `switch(a2) case 289,290,291,292,293,294,295,296`（8 个 s2c 子包）。
- `CField_MonsterCarnival::OnEnter`：`Decode1` + 6× `Decode2` + `Decode1`（队伍/CP/时间等字段序）。
- `OnPersonalCP` = `Decode2 + Decode2`；`OnTeamCP` = `Decode1 + Decode2 + Decode2`。
- `OnRequestResult` 按 `Decode1` 分支挂字符串：`SP_4082` CP 不足 / `SP_4083` 不能再召唤怪 / `SP_4084` 不能再召唤「存在」/ `SP_4085` 已召唤 / `SP_4086` 未知错误。
- `OnProcessForDeath`：`SP_4078_MAPLE_RED` / `SP_4079_MAPLE_BLUE` 队色 + `SP_4080`（失去战斗能力且扣 CP）/ `SP_4081`（无 CP 不扣）。
- `OnShowGameResult`：`SP_4087` 胜 / `SP_4088` 负 / `SP_4089` 加时平局 / `SP_4090` 对方离场 abrupt。
- `OnShowMemberOutMsg`：`SP_4096` 成员退出 / `SP_4097` 队长退出改任。
- `CField_Battlefield`：`DecodeFieldSpecificData`=`Decode1`；`OnScoreUpdate`=`Decode1+Decode1`；`OnTeamChanged`=`Decode4+Decode1`；`OnClock`=`Decode1+Decode4`；另有 `VCScoreboard_Battlefield`（计分板）。
- `CField_Dojang`（道场）：`OnClock`=`Decode1+Decode4` + `SP_4552_UI_UIWINDOWIMG_MURUENGRAID_NUMBER_BAR`；`UpdateTimer` 挂 `SP_1000_BLAZE`/`SP_2501_DOJANG_TIMEOVER`/`SP_2558_DOJANG_TIMEOVER`。
- `CField_Coconut::Init`：`SP_2776_COCONUT` / `SP_2777_COUNTFALLING` / `SP_2778_COUNTBOMBING` / `SP_2779_COUNTSTOPPED` / `SP_2780_MAP_OBJ_ETCIMG_COCONUT_BACKGRND` + `SP_956_CANVAS`；`BasicActionAttack` 发 `COutPacket(213)`。
- `CField_SnowBall::Update` 发 `COutPacket(212)`；`CField_GuildBoss::BasicActionAttack` 发 `COutPacket(215)`。
- 其余 `CField_ContiMove`(4)/`CField_Massacre`(1)/`CField_MassacreResult`(2)/`CField_SpaceGAGA`(1)+`VCTimerboard_SpaceGAGA`/`CField_Witchtower`(1)/`CField_LimitedView`(3)/`CField_PartyRaid*` 3 类/`CField_Wedding`(4)/`BattlefieldTeamCond`(1) 均在 facts.md 逐函数列关键行（§3 坐标段起）。

## §4 活动 UI 族（控件 id / 坐标 / 资源节点，逐字）

### §4.1 CUIMonsterCarnival::OnCreate（16 方法类）
资源节点：`SP_4102..4105_UI_UIWINDOWIMG_MONSTERCARNIVAL_BACKGRND3_{TOP_0,MIDDLE1_0,BOTTOM_0,MIDDLE0_0}`（4 片可拉伸底）、`SP_4098/4099/4112/4113_..._ICON0..3`（4 图标）、`SP_4106_..._BTSIDE`、`SP_4107/4108_..._TAB_ENABLED/DISABLED`、`SP_1262_UI_BASICIMG_BTMAX`、`SP_4109_UI_BASICIMG_BTDECIDE`、字体 `SP_1410_CANVASFONT` + `SP_5527_ARIAL`。
控件（推测为 `(*vt+32)(a2: this, a3: id, a4: x, a5: y, a6: ?, a7: ZRef)`）：
```
CCtrlButton (a3:3, a4:126, a5:8)      // 需结合 RequestSend 判定
CCtrlButton (a3:2, a4:140, a5:8)
CCtrlButton (a3:4, a4:149, a5:28)
CCtrlTab    (a3:0, a4:5, a5:4, a6:107, a7:162, a8:0) + SetTab(*(this+376))
```
`CUIMonsterCarnival::RequestSend` 逐字：`COutPacket::COutPacket(a1: 218)` + `Encode1(v3)` + `Encode4(*(this+727) - 1)` + `CClientSocket::SendPacket(dword_BE7914, &v6)` → **怪物嘉年华召唤/请求 c2s=218**（观察）。

### §4.2 CUIItemMaker（道具制作，23 方法类）
`OnCreate` 控件：`30000 (a4:3,a5:77,a6:71,a7:78,a8:18)` / `40000 (3,157,71,143,18)` 两个 ComboBox（类别/配方）；`20000 (82,356)` / `20001 (163,356)` 两个按钮。`RequestItemMake` 逐字 `COutPacket(113)` + `Encode4(v5)` + `Encode4(*(this+371))` + `Encode1(*(this+1976))` + `Encode4(*(this+485))` + `Encode4(Data)` + `Encode4(3)` + `Encode4(4)` + `Encode4(v6)` + `Encode4(*(this+495))` + `Encode4(v14)` + `SendPacket`。字符串：`SP_753_PRODUCTION_FAILED`、`SP_598_STRENGTHENING_GEM`/`SP_599_MONSTER_CRYSTAL`/`SP_600_DISASSEMBLE_EQUIPMENT`、确认文案 `SP_756..761`、`SP_759_WOULD_YOU_LIKE_TO_MAKE_`。→ **c2s=113 道具制作/分解**（观察）。

### §4.3 其它活动 UI 的 CreateCtrl 锚点
- `CUIQuestInfo::OnCreate`：`CCtrlTab (2000, a4:1,a5:3,a6:23,a7:239,a8:0)` + `(2001, 1,3,225,50,309)` + 3× `SetTab`；资源 `SP_1891_UI_UIWINDOWIMG_QUEST_BACKGRND2` / `SP_1896_..._LINE` / `SP_1897..1901_..._ICON0/1/2/4` / `SP_2664/2665_..._TAB_ENABLED/DISABLED`。
- `CUIMedalQuestInfo::OnCreate`：`CCtrlTab (2000,1,4,24,300)` + `(2001,1,3,239,48,12)` + `SetTab(100)`；字符串 `SP_5300_NUMBER_OF_MEDALS_OBTAINED_D` / `SP_5301_OBTAINABLE_TITLE` / `SP_5302_TITLE_BEING_CHALLENGED` / `SP_5105_VIEWMEDALITEM` / `SP_5106_MEDALCATEGORY`。
- `CUINewYearCardSenderDlg::OnCreate`：控件 `0 (295,65)` / `1 (124,164)` / `2 (181,164)` / `3 (46,66,243,15)` / `4 (13,105,333,45)` / `5 (1,3,498,7,93)`（新年贺卡发送窗）。
- `CUICashItemGachapon::OnCreate`：控件 `2000 (68,149)` / `2001 (150,8)` + `SP_5066_UI_UIWINDOWIMG_CASHGACHAPON_BTOPEN` / `SP_5067_..._BTCLOSE`；`OnCashItemGachaponResult` = `Decode4 + Decode1 + Decode1`。
- `CSlideMenuDlg`（3 方法）：资源 `SP_4505_UI_UIWINDOWIMG_SLIDEMENU_D` / `SP_4506_..._BACKGRD` / `SP_4507_..._BTMAIN_D` / `SP_4512_..._CHOICE` / `SP_4513_..._COVER`（滑条菜单，问答/自动反外挂选择用）。
- `CUITitle`（5 方法）：`SP_5162/5164_UI_UIWINDOWIMG_TITLE_BACKGRND2/4`、`SP_5168/5169_..._ICON1/2`、`SP_5170/5171_..._TAB_DISABLED/ENABLED`。
- `CUIEnergyBar`（2 方法）：`SP_1883_UI_UIWINDOWIMG_ENERGYBAR` + `SP_1886_DISPLAYS_THE_ENERGY_LEVEL_THATS_BEEN_CHARGED_NUSE_THE_MOUSE_TO_CHANGE_POSITION`（充能条，可拖动）。
- `CUIEventAlarm`（1 方法 `CreateEventAlarm`）/ `CUIQuestAlarm`(6)/`CNoticeQuestProgress`(5)/`CUIMonsterBook`(5)+`CMonsterBookMan`(6)/`CUIEnchantDlg`(2)/`CUIGameMenu`(1)/`CUIRevive`(3)/`CUIAntiMacroCharacterName`(1) 均在 facts.md 逐类列 SP_ 与关键行。
- 怪物杀计数族（本题材资源最密集）：`SP_5371..5393_UI_UIWINDOWIMG_MONSTERKILLING_{COUNT_BACKGRD0/1,COUNT_NUMBER*,GAUGE_*,RESULT_BACKGRD*,RESULT_RANK_A/B/C/D/S}`。

## §5 协议 opcode 全集（267 函数内 `COutPacket::COutPacket(a1: N)` 全扫描，8 个）

| c2s opcode | 构造点（逐字函数） | 语义 |
|---|---|---|
| 60 | `CScriptMan::OnSay/OnAskYesNo/OnAskText/OnAskBoxText/OnAskNumber/OnAskMenu/OnAskAvatar/OnAskMembershopAvatar/OnAskPet/OnAskPetAll/OnAskSlideMenu` | NPC 脚本应答统一封包 |
| 107 | `CQuest::StartQuest`（多处） | 任务开始 |
| 113 | `CUIItemMaker::RequestItemMake` | 道具制作/分解 |
| 123 | `CMiniRoomBaseDlg::SendInviteResult/SendCashInviteResult/OnCheckSSN2Static` | 迷你房间邀请应答 |
| 212 | `CField_SnowBall::Update` | 雪球活动上行 |
| 213 | `CField_Coconut::BasicActionAttack` | 椰子活动攻击 |
| 215 | `CField_GuildBoss::BasicActionAttack` | 公会 Boss 活动攻击 |
| 218 | `CUIMonsterCarnival::RequestSend` | 怪物嘉年华召唤/请求 |

**脚本问答 action code（`COutPacket(60)` 后首个 `Encode1`）逐字**：`OnSay=0`、`OnAskText=2`、`OnAskNumber=3`、`OnAskMenu=4`、`OnAskAvatar=7`、`OnAskMembershopAvatar=8`、`OnAskPet=9`（还带 `Encode1(2)` 与二次 `Decode1`）、`OnAskPetAll=10`、`OnAskBoxText=13`、`OnAskSlideMenu=14`。`OnAskYesNo`/`OnAskQuiz`/`OnAskSpeedQuiz`/`OnSayImage` 在伪代码里未显式命中 action 常量（可能被常量折叠或走虚表），记「未知，待单独核对」。

**收包解码集中点**：`CField_MonsterCarnival::OnPacket` (case 289–296)、`CScriptMan::OnScriptMessage`（3× `Decode1`）、`CMiniRoomBaseDlg::OnPacketBase`（1× `Decode1` 分派）+ `CInPacket`(9)/`COutPacket`(8) 基础类。本面出站构造点较稀薄（8 个），收包字段序是主力。

## §6 SP_ 238 节点关键子集（按活动族聚合）

- 迷你房间/商店拒绝文案：`SP_394..406`（房间关闭/满员/不同地图/拒绝邀请/事件中不可/死亡不可等）、`SP_428/433/434/435/436/472/5256`（商店/锦标赛/入场费/频道限制）、`SP_5567_THE_PASSWORD_IS_INCORRECT`。
- 怪物嘉年华：`SP_4078..4097` 队色/CP/胜负/成员退出（见 §3）+ `SP_4098..4113` UI 件表（见 §4.1）+ `SP_4128_CP_EARNED_THAT_CAN_BE_USED`/`SP_4129_CP_REMAINING_D___CP_EARNED_D`/`SP_4130/4132` 规则说明。
- 椰子：`SP_2776..2784`；道场：`SP_2501/2558/4552`；雪球/空间 GAGA：`SP_4063_AUTOSTART`/`SP_4066_ONESHOT`/`SP_4067_TIMELIMIT`（活动通用计时标记）。
- 卷轴/强化：`SP_387..389`（成功/失败/销毁）、`SP_3965/3966`（白卷）、`SP_1246_ENCHANTSUCCESS`/`SP_1247_ENCHANTFAILURE`/`SP_1248_WHISTLE`。
- 任务：`SP_1891..1901`/`SP_2664/2665`/`SP_2973_QUEST`/`SP_3199_QUEST_ACTIMG`/`SP_3212_QUEST_SAYIMG`/`SP_3214_ASK`/`SP_3216_ORDER`/`SP_3217_ITEM`/`SP_3220_QUEST_CHECKIMG`/`SP_3232_QUEST_QUESTINFOIMG`/`SP_3620_QUEST_HELPER_D_5`/`SP_3621_UI_BASICIMG_BTCLOSE2`。
- 怪物书：`SP_2239_STRING_MONSTERBOOKIMG`/`SP_2240_STRING_MOBIMG`/`SP_2584_UI_UIWINDOWIMG_MONSTERBOOK`/`SP_2356_ICON`/`SP_2366_ID`。
- 道具制作：`SP_598..600`/`SP_753..761`；扭蛋：`SP_5066/5067`；称号/勋章：`SP_5105..5302`/`SP_5162..5171`/`SP_5576_TITLE`。
- 其它：`SP_1883/1886` energy bar、`SP_4505..4513` slide menu、`SP_5138_UI_TUTORIALIMG_D`（新手教程）、`SP_5371..5393` 怪物杀计数、`SP_1300_UI_UIWINDOWIMG_FLOATNOTICE_D`（浮动公告，配 `CFloatNotice`/`CSlideNotice`）、`SP_3954..` 无（本面无 095 专属节点）。
- 共有噪声：`SP_0/1/2/3`（Nexon 官网 URL 串）、`SP_1410_CANVASFONT`/`SP_5527_ARIAL`（字体）、`SP_1345..1354_UI_LOGINIMG_TITLE_*`（登录按钮资源，因 `CUIEventAlarm`/`CWnd` 通用引用带入，非本面功能）。

## §7 与 MEMORY.md 083 术语互证

- G 键=GUILD=「家族」=公会（UIUserList 第 3 页签）、F 键=FAMILY=「学院」。本面 `CField_GuildBoss`/`CField_PartyRaid*` 是 083 已有的公会/组队活动场地，与 095 专有的 v95 导师 Academy / 远征 BossParty 独立面板区分——**判「不做」的仍只有 095 专有项**。
- `SP_3675_EBROCK_PAPER_SCISSORS...`（猜拳报名费）与 `SP_3458_SIGNBOARD`/`SP_3475_YOU_MAY_NOT_OPEN_THE_STORE_NEAR_A_PORTAL` 等证明 083 原生已有小游戏/开店全套拒绝文案族。
- 未引任何 095 线符号/IDB（`Maplestory95.i64`、port-095 映射、ExpeditionIntermediary）作依据。

## §8 六面一致性表

| 功能面 | Angel 具名函数 | BeiDou 同 EA | missing | 结论 |
|---|---|---|---|---|
| 登录 | 327 | 328 | 0 | 同 v83 不同符号状态 |
| 状态栏 HUD | 547 | 548 | 0 | 同上 |
| 核心窗口 | 439 | 441 | 0 | 同上 |
| 社交经济 | 752 | 752 | 0 | 同上 |
| 宠物召唤龙 | 968 | 968 | 0 | 同上 |
| **活动小游戏** | **844** | **844（字节级）** | **3（IDA 未识别，字节相同）→ 0（实质）** | 同上，crosscheck 阈值机械误判已纠正 |

## §9 逐字伪代码索引（供 r02 销案）

- `CMiniRoomBaseDlg::OnPacketBase`（§2 全文）、`COmokDlg::OnKey`/`CMemoryGameDlg::OnKey`（同文）、`CRPSGameDlg::OnPacket`（§2 摘要）。
- `CField_MonsterCarnival::OnPacket/OnEnter/OnPersonalCP/OnTeamCP/OnRequestResult/OnProcessForDeath/OnShowGameResult/OnShowMemberOutMsg`（§3）。
- `CUIMonsterCarnival::OnCreate/RequestSend`、`CUIItemMaker::OnCreate/RequestItemMake`、`CUIQuestInfo::OnCreate`、`CUINewYearCardSenderDlg::OnCreate`、`CUICashItemGachapon::OnCreate/OnCashItemGachaponResult`、`CUIMedalQuestInfo::OnCreate`（§4）。
- 完整逐字伪代码见 `evidence/angel-activity-decompile-full.json`（267 键）；SP_ 全集 + 逐函数关键行 + 坐标段见 `evidence/angel-activity-facts.md`。

## §10 限制与未知

- `CGrenade::JustBomb` 无 Hex-Rays 伪代码，仅 disasm 兜底（语义标注为推断）。
- `CScriptMan::OnAskQuiz/OnAskSpeedQuiz/OnAskYesNo` 的 action code 未从伪代码显式读出，记未知。
- `CMovePath`/`CVecCtrlGrenade` 等仅取本类自身方法，深层字节布局未展开。
- IMG 层未做像素级视觉复核（`read_image` 对 PNG 报模型不支持图像输入）；orange-wz 未接入，节点件表以 `.nx` 字面量 + 正确端截图替代。
- 全程只读取证：未改产品源码、未构建、未实机、未提交。
