# r122 — Admin / Quiz / SkillUp / ItemSearch

- `UIAdmin` SoftKey：`users|show` → `AdminEnterMapPacket(SHOW_USERS)`；`alert` → `ALERT_ADMINS`；其它 → 喊话
- SpeedQuiz / InitialQuiz SoftKey → `GeneralChatPacket`（无独立 Quiz opcode 证据）
- `UISkillUp` SoftKey skillid → `SpendSpPacket`
- `UIItemSearch` SoftKey → `GeneralChatPacket`

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
