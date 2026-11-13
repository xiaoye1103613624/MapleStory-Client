# 本轮用到的 Angel 地址

IDB：`E:\MXD\反编译IDA\v83\`Angel.i64`，模块 `` `Angel.exe ``，imagebase `0x400000`。

| 地址 | 符号 | 用法 |
| --- | --- | --- |
| `0x67961d` | `CMobPool::OnMobLeaveField` | 动画字节 0 停用，非 0 死亡 |
| `0x663995` | `CMob::OnDie` | 死亡动作与层淡出；本端用帧计时兜底 |
| `0x8acf1a` | `CUISkill::OnSkillLevelUpButton` | 500ms 门闩与技能点前置 |
| `0x81d01f` | `CUIItem::OnChildNotify` | `0x7D4`–`0x7D7` 与槽位合并只是悬停文案 |
| `0x8c4c7f` | `CUIStat::OnCreate` | 不创建 BtAuto |
| `0x8d4a6d` | `CUIStatusBar::SetChatType` | 本端已有三态 |
| `0x84056b` | `CUIMedalQuestInfo::OnCreate` | 列表已画；详情窗仍预留 |
| `0x867be6` | `CUIMonsterBook::UpdateCheckList` | 勾选翻转在；筛选预留 |
