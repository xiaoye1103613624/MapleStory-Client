# IDA 盘点：13337 → `v83/083/v83.i64`（Angel.exe 同源）

> 采集日期：2026-11-12  
> 方法：live `server_health` / `survey_binary` / `py_eval`（HTTP 直连 13337 & 13339）  
> **身份铁律**：端口会复用；以 `idb_path` 为准。当前 **13337 不是 BeiDou**。

## 0. 一句话结论

- **13337** 当前挂的是 `E:\MXD\反编译IDA\v83\083\v83.i64`，module/input 仍是 **`Angel.exe`**（MD5 `cbe5ccba...`）。
- **13339** 挂的是同目录上一级的 `` `Angel.i64 ``，**同一份 exe** 的另一份 IDB。
- 「大小不一样」主要来自 **IDB/id0 数据库体积与打开时长**，不是不同版本二进制；`image_size` 两侧均为 `0xa95000`，函数数仅差 1。
- 符号可读性：两侧同级（3122 MSVC 修饰名 / 399 类 / 2984 类方法），**不是**历史「Angel 有符号 vs BeiDou 全 sub_*」。

## 1. 身份与大小（详见 notes）

| 文档 | 结论 |
|---|---|
| [notes/r00-identity-and-size.md](notes/r00-identity-and-size.md) | 13337=`v83.i64`；与 Angel 同 MD5；大小差在 IDB 文件 |
| [evidence/identity-live-health.json](evidence/identity-live-health.json) | live 身份 JSON |
| [evidence/angel-vs-v83i64-compare.json](evidence/angel-vs-v83i64-compare.json) | 指标对照 |
| [evidence/live-survey-minimal.json](evidence/live-survey-minimal.json) | survey_binary |
| [evidence/live-health-disk.json](evidence/live-health-disk.json) | 磁盘 IDB 体积 |
| [evidence/key-class-method-eas-13337.json](evidence/key-class-method-eas-13337.json) | 关键类方法 EA 抽样 |

## 2. 全局规模

| 指标 | 13337 v83.i64 | 13339 Angel.i64 |
|---|---:|---:|
| func_total | 55015 | 55016 |
| func_real_named | 3691 | 3691 |
| MSVC mangled | 3122 | 3122 |
| classes | 399 | 399 |
| class methods | 2984 | 2984 |
| CUI* classes | 77 | 77 |
| strings | 1281 | 1281 |
| image_size (survey) | 0xa95000 | 0xa95000 |
| input_md5 | `cbe5ccba04cbe2760d8eabc537926bd1` | `cbe5ccba04cbe2760d8eabc537926bd1` |

## 3. 功能点索引（每点独立文档）

| # | 功能点 | 一句话 | 类/方法 | vs Angel | notes | evidence |
|---:|---|---|---:|---|---|---|
| 1 | `login` | 登录/选角/频道：CLogin 等 10 类 / 98 方法 | 10/98 | 一致 | [notes/r01-login-facts.md](notes/r01-login-facts.md) | [evidence/feature-login.json](evidence/feature-login.json) |
| 2 | `statusbar_hud` | 状态栏/HUD：CUIStatusBar 等 7 类 / 108 方法 | 7/108 | 一致 | [notes/r02-statusbar_hud-facts.md](notes/r02-statusbar_hud-facts.md) | [evidence/feature-statusbar_hud.json](evidence/feature-statusbar_hud.json) |
| 3 | `corewindow` | 窗口控件基座：CWnd/CCtrl*/CWndMan 等 22 类 / 181 方法 | 22/181 | 一致 | [notes/r03-corewindow-facts.md](notes/r03-corewindow-facts.md) | [evidence/feature-corewindow.json](evidence/feature-corewindow.json) |
| 4 | `social` | 社交：好友/家族/Messenger/UserList 等 8 类 / 77 方法 | 8/77 | 一致 | [notes/r04-social-facts.md](notes/r04-social-facts.md) | [evidence/feature-social.json](evidence/feature-social.json) |
| 5 | `pet` | 宠物：CPet/CUIPetEquip 等 3 类 / 33 方法 | 3/33 | 一致 | [notes/r05-pet-facts.md](notes/r05-pet-facts.md) | [evidence/feature-pet.json](evidence/feature-pet.json) |
| 6 | `activity` | 活动/临时状态：EnergyBar/EventAlarm/TemporaryStat 3 类 / 12 方法 | 3/12 | 一致 | [notes/r06-activity-facts.md](notes/r06-activity-facts.md) | [evidence/feature-activity.json](evidence/feature-activity.json) |
| 7 | `cashshop` | 商城：CCashShop 为主 2 类 / 70 方法 | 2/70 | 一致 | [notes/r07-cashshop-facts.md](notes/r07-cashshop-facts.md) | [evidence/feature-cashshop.json](evidence/feature-cashshop.json) |
| 8 | `skill` | 技能 UI：CUISkill* / CSkillInfo 4 类 / 14 方法 | 4/14 | 一致 | [notes/r08-skill-facts.md](notes/r08-skill-facts.md) | [evidence/feature-skill.json](evidence/feature-skill.json) |
| 9 | `npc` | NPC：CNpc/CNpcPool 等 4 类 / 54 方法 | 4/54 | 一致 | [notes/r09-npc-facts.md](notes/r09-npc-facts.md) | [evidence/feature-npc.json](evidence/feature-npc.json) |
| 10 | `map` | 地图/副本 Field：CField* 等 32 类 / 207 方法 | 32/207 | 一致 | [notes/r10-map-facts.md](notes/r10-map-facts.md) | [evidence/feature-map.json](evidence/feature-map.json) |
| 11 | `drop` | 掉落：CDropPool 1 类 / 7 方法 | 1/7 | 一致 | [notes/r11-drop-facts.md](notes/r11-drop-facts.md) | [evidence/feature-drop.json](evidence/feature-drop.json) |
| 12 | `quest` | 任务：CQuestMan/CUIQuest* 6 类 / 50 方法 | 6/50 | 一致 | [notes/r12-quest-facts.md](notes/r12-quest-facts.md) | [evidence/feature-quest.json](evidence/feature-quest.json) |
| 13 | `packet` | 封包：CIn/OutPacket + CClientSocket 3 类 / 35 方法 | 3/35 | 一致 | [notes/r13-packet-facts.md](notes/r13-packet-facts.md) | [evidence/feature-packet.json](evidence/feature-packet.json) |
| 14 | `crypto` | 加密：CAESCipher/CIGCipher/CSecurityClient 3 类 / 18 方法 | 3/18 | 一致 | [notes/r14-crypto-facts.md](notes/r14-crypto-facts.md) | [evidence/feature-crypto.json](evidence/feature-crypto.json) |
| 15 | `character` | 角色：CUserLocal/CUser/CAvatar 等 8 类 / 285 方法 | 8/285 | 一致 | [notes/r15-character-facts.md](notes/r15-character-facts.md) | [evidence/feature-character.json](evidence/feature-character.json) |
| 16 | `inventory` | 背包装备：CItemInfo/CUIItem/CUIEquip 等 6 类 / 92 方法 | 6/92 | 一致 | [notes/r16-inventory-facts.md](notes/r16-inventory-facts.md) | [evidence/feature-inventory.json](evidence/feature-inventory.json) |
| 17 | `monsterbook` | 怪物书：CUIMonsterBook/CMonsterBookMan 2 类 / 11 方法 | 2/11 | 一致 | [notes/r17-monsterbook-facts.md](notes/r17-monsterbook-facts.md) | [evidence/feature-monsterbook.json](evidence/feature-monsterbook.json) |
| 18 | `medal` | 勋章称号任务：CUIMedalQuestInfo* 2 类 / 13 方法 | 2/13 | 一致 | [notes/r18-medal-facts.md](notes/r18-medal-facts.md) | [evidence/feature-medal.json](evidence/feature-medal.json) |

## 4. 校准建议

1. 当前 13337 与 13339 **都是 Angel 符号 IDB**，可互换作 v83 原生逻辑源。
2. 历史「BeiDou stripped 交叉验证」链路若仍需要，须重新打开 BeiDou IDB（会拿到新端口），**不能**假定还在 13337。
3. Cursor 默认 `user-ida-pro-mcp` 可能挂在 13339；分析「用户说的 13337」须 HTTP 直连或先 `server_health` 读 `idb_path`。

## 5. 段布局（两侧相同）

```json
[
  {
    "name": "___",
    "start": "0x401000",
    "end": "0xaf0000",
    "size": 7270400,
    "perm": 7
  },
  {
    "name": ".idata",
    "start": "0xaf0000",
    "end": "0xaf03f8",
    "size": 1016,
    "perm": 7
  },
  {
    "name": "___",
    "start": "0xaf03f8",
    "end": "0xbf9000",
    "size": 1084424,
    "perm": 7
  },
  {
    "name": ".idata__",
    "start": "0xc19000",
    "end": "0xc1a000",
    "size": 4096,
    "perm": 6
  },
  {
    "name": "________",
    "start": "0xc1a000",
    "end": "0xe92000",
    "size": 2588672,
    "perm": 7
  },
  {
    "name": ".mackt",
    "start": "0xe92000",
    "end": "0xe94000",
    "size": 8192,
    "perm": 7
  },
  {
    "name": "seg006",
    "start": "0xe94000",
    "end": "0xe95000",
    "size": 4096,
    "perm": 6
  }
]
```

含 `.mackt`（Themida/加壳相关残留段名特征），与历史 Angel 描述一致。
