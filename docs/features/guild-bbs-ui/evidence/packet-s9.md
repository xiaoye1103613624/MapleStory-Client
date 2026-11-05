# s9 Guild BBS 封包摘要

## s2c `GUILD_BBS_PACKET` (59 / 0x3B)

### mode 0x06 列表 — `GuildPackets.BBSThreadList`

```
byte 0x06
byte hasNotice
[if hasNotice] addThread(notice)   // localthreadid==0 行
int totalThreadCount               // 不含 notice
int pageCount                      // min(10, total-start)
×pageCount addThread
```

`addThread`：`int localid, int postercid, string name, long time, int icon, int replycount`

### mode 0x07 读帖 — `GuildPackets.showThread`

```
byte 0x07
int localthreadid
int postercid
long time
string name
string startpost
int icon
int replycount
×reply: int replyid, int postercid, long time, string content
```

## c2s `BBS_OPERATION` (155 / 0x9B)

| mode | 含义 | 本端包 |
| --- | --- | --- |
| 0 | 发/编辑 | `BbsPostPacket` / `BbsEditPacket` |
| 1 | 删帖 | `BbsDeletePacket` |
| 2 | 列表 start | `BbsListPacket` |
| 3 | 打开 | `BbsOpenPacket` |
| 4 | 回复 | `BbsReplyPacket` |
| 5 | 删回复 | `BbsDeleteReplyPacket` |
