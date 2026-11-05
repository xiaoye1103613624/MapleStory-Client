# IDA：healer / pulley

- 字符串 `healer` @ `0xAF4A68`、`pulley` @ `0xAF4A58`、`yMin` @ `0xAF4A60`
- 唯一 xref → `sub_5587B3`（地图物件属性读取）
- 调用 `sub_558455(healer…)` / `sub_558612(pulley…)` 做物件初始化
- 当前 IDB 无独立「GuildBoss HUD 窗」符号；s2c 287/288 在私服侧由 `GuildPackets` 发出
