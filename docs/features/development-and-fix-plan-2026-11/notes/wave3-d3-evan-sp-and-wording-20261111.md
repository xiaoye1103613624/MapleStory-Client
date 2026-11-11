# round95 · D3 命名纠偏 + SP 表分支 + 11 处措辞 + P3 转重做

## 1. 用户三问

1. 「11 处待排版」→ 指 `DECISIONS.md` 里 11 行「待裁决/待拍板」遗留措辞。
2. 「P3（16）按 Angel 全部重做」→ 推翻 round94 的「保持现状」，转 A/B/C 分批重做。
3. 「骑士团条件解码 你登录 admin 账户有骑士团，自己抓，服务端一直启动的，是否可以直接对协议？」
   → **能，而且不用抓包**：s9 服务端源码就是协议权威，直接读 `PacketCreator.addCharStats` 即可。

## 2. 无需抓包的三个理由

- s9 是「本客户端唯一对接的服务端」，`E:/project/BeiDou-Server_s9/gms-server` 全仓只有一份
  `PacketCreator.java`，`addCharStats` 就是 CHARLIST/SET_FIELD 里 stats 段的写方。
- 写方逐字段可读（`:190-218` 全段），比抓包更强：抓包只能看到字节，看不到「哪个分支写的」。
- 抓包在本机不可行：客户端无封包日志能力（`grep packet_log|log_packet|LOG_PACKET|dump_packet|packet_trace`
  在 `Net/` `IO/` `Configuration.*` 全空），`tools/capture/` 只有 D3D8 注入与截图脚本，无协议代理。

## 3. 关键纠偏：这不是「骑士团」

083 `GW_CharacterStat::Decode @0x4e2a84` 尾段：

```c
v24 = *(this + 61);
*(this + 133) = v16;
v17 = _ZtlSecureFuse<short>(a1: this + 57, a2: v24);   // JOB
if ( v17 / 100 == 22 || v17 == 2001 )
{
  LOWORD(v17) = 0;
  *(this + 141) = sub_4E80EB(a1: v17, a2: this + 137);  // SP = 0
  sub_4E2745(a1: a2);                                   // 读 1 + 2*count
}
else
{
  LOWORD(v18) = CInPacket::Decode2(this: a2);           // 读 2B SP
  ...
}
```

`JOB / 100 == 22` = **2200–2299 = Evan（龙神）系**；骑士团 Cygnus 是 1000–1599。
s9 `GameConstants.hasSPTable` 枚举的正是 `EVAN(2001)` + `EVAN1..EVAN10(2200,2210..2218)`。
两端集合一致 → 客户端 `parse_stats` 无条件 `read_short()` 是真缺陷，Evan 角色会少读 2*count 字节。

## 4. 补丁要点

- `parse_stats`：`job` 存局部变量；`has_sp_table(job)` → 读 `1 + 2*count` 并把 SP 槽置 0；
  否则原样 `read_short()`。
- 零 wire / 零协议 / 零 WZ / 不碰 6 个 s9 改造字段（level 2B / HP-MP int / exp 8B /
  GachaExp 4B / spawn tail int4 / removeItemFromMap 4B cid）。
- 表内逐职业 SP 只消费不落库：当前端技能窗只有单一 SP 槽，多职业 SP 模型不发明。

## 5. 算术不变

`168 = 76 + 38 + 14 + 40`；`168 = W 38 + L 130`；`27 = 已注销 10 + 残余 17`；
`17 = P1 0 + P2 0 + P3 16`（本轮 D3 从 P2 解掉，P3 16 条不变，合计仍 17）。
