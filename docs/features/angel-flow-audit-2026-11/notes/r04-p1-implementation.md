# r04 · P1 实现纪要（2026-11-12）

## 证据

- String/MonsterBook.img：键=mobId；reward 含 238xxxx；342 条 `card%10000` 无碰撞（`_tools/_mb_card_map.py`）
- Angel：`CUIMedalQuestInfoDetail` ctor @0x83d255 305×373；`CNoticeQuestProgress` 有类；UtilDlgEx 指引 blink ≠ 菜单 BT_MENU_QUEST
- s9：无 Ignore / 拒绝列表专用 opcode → 不发明灌数

## 决策落地

| 项 | 做法 |
| --- | --- |
| BossHP | oid map 叠画 |
| 图鉴 | reward map → Mob 名 + episode/map/reward |
| 勋章 | 文本 Detail Toggle；Eqp 名 |
| T4 | QuestAlarm progress_tip + `blink_menu_quest()`；KEY_ANI 链预留 |
| UserList | 行选中驱动 Whisper/Where/Delete；黑名单代理保留 |

## 红线未触

T3/T5/T6；095；ExpeditionIntermediary。
