# W14 medal — 执行记录

## 对照

- IDA r18 / 工作区 `UIMedal`

## 本波 / W2 续（2026-11-20）

- Medal 窗落地已入账；Detail ToggleDetail 文本浮层已接。
- W2：`UIWindow.img/Title` 真材（backgrnd 260×374 / Tab / backgrnd3 Detail）优先于 ColorBox；见 `userlist-five-tabs/notes/w2-medal-family-2026-11-20.md`。

## 勋章图标 — 为何不能做（资源）

- 缺口是 **114xxxx 等具体勋章图标素材**，非窗体逻辑
- 无 orange-wz 证据证明「缺节点且可按面最小补齐」前禁止盲改 WZ
- 缺图标时保持文本态，避免用错误占位图污染道具表

**结论**：图标列仍预留文本态（缺资源且无法安全补齐）。

## DamageSkin 同类备注

- 生产 `Data/Effect/DamageSkin.img` 已存在（~65MB）；客户端 `DamageNumber::apply_skin` + `DAMAGE_SKIN` s2c handler 已接线
- 旧「DamageSkin 节点缺口」对**当前 live 包**不再成立；若个别 skin id 缺子节点，属按 id 资源面，不在本阻塞清扫强改
