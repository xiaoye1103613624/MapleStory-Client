# r23 · PartySearch 协议接线（自动匹配）

> 承接 [`r22-partysearch-shell.md`](r22-partysearch-shell.md)。  
> 证据：[`evidence/r23/partysearch_protocol.txt`](evidence/r23/partysearch_protocol.txt)  
> 权威：s9 `PartySearchStartHandler` / `PartySearchUpdateHandler` / `PacketCreator.partySearchInvite`

## 结论（重要）

s9 **没有** PartySearch「列表板」回包。实现是**队长自动匹配**：

| 方向 | Opcode | 用途 |
|------|--------|------|
| C→S | **222** `PARTY_SEARCH_START` | `int min,max,members,jobs` |
| C→S | **223** `PARTY_SEARCH_UPDATE` | 空体 → Stop |
| C→S | 220 `PARTY_SEARCH_REGISTER` | s9 stub，客户端忽略 |
| S→C | **62** `PARTY_OPERATION` mode **4** | 邀请；名带 `"PS: "` 前缀 |

## 客户端改动

- [`PartySearchPackets.h`](../../../Net/Packets/PartySearchPackets.h)：Start / Stop / DenyPartyRequest(125)
- [`UIPartySearch`](../../../IO/UITypes/UIPartySearch.cpp)：BtStart/Stop/Pause 发包；状态文案（无列表数据可填）
- [`PartyHandlers`](../../../Net/Handlers/PartyHandlers.cpp)：mode `0x04` → `UIYesNo`；PS 邀请可识别
- **修正**：`PLAYER_UPDATE(223)` 误名 → 与 `PARTY_SEARCH_UPDATE` 对齐；`PlayerUpdatePacket::dispatch` 改为 **no-op**（进图空 223 会误停搜索）

## 验收

1. 队长开 PartySearch → Start → 状态 Searching  
2. 符合条件玩家收到带 PS 的组队邀请 YesNo → 接受进队 / 拒绝 Deny  
3. Stop 后不再匹配；换图不因旧 PlayerUpdate 误停（已 no-op）

## 预留

- 等级/职业勾选 UI（现用默认 lv±15、全职业 mask）
- REA CreateCtrl 按钮坐标
- Register stub 若服务端日后实现再接线
