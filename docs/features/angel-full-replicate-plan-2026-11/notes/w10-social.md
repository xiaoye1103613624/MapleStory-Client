# W10 social — 执行记录

## 对照

- 已有：[social-economy-angel-calibration-2026-11](../../social-economy-angel-calibration-2026-11/README.md)

## 本波

- `UIUserList` 工作区续作已入账

## PARTY_SEARCH_REGISTER(220) — 为何不能做

- s9 `RecvOpcode.PARTY_SEARCH_REGISTER(0xDC=220)` **有 enum**，且 `PacketProcessor` 注册了 `PartySearchRegisterHandler`
- 但 handler 体为空：`public void handlePacket(InPacket p, Client c) {}`（`BeiDou-Server_s9/.../PartySearchRegisterHandler.java`）
- 当前 UI：`UIPartySearch::BtReg` → `start_search()` 代理 `PARTY_SEARCH_START(222)`（有用）
- 若改发 220：服务端空 stub → 注册无效果，反而不如 Start 代理

**结论**：客户端单独改发包无收益；需服务端实现 Register 语义后再接。维持「Start 代理」。

## 验收

- 无本轮代码
