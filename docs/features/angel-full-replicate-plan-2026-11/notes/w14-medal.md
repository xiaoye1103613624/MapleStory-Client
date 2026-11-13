# W14 medal — 执行记录

## 对照

- IDA r18 / 工作区 `UIMedal`

## 本波

- Medal 窗落地已入账；详情窗 `CUIMedalQuestInfoDetail` 内容显式预留

## 勋章图标 — 为何不能做（资源）

- 缺口是 **114xxxx 等具体勋章图标素材**，非窗体逻辑
- W1：无 orange-wz 证据证明「缺节点且可按面最小补齐」前禁止盲改 WZ
- 缺图标时保持文本态，避免用错误占位图污染道具表

**结论**：不能做（缺资源且无法安全补齐）。

## DamageSkin 同类备注

- 生产 `Data/Effect/DamageSkin.img` 已存在（~65MB）；客户端 `DamageNumber::apply_skin` + `DAMAGE_SKIN` s2c handler 已接线
- 旧「DamageSkin 节点缺口」对**当前 live 包**不再成立；若个别 skin id 缺子节点，属按 id 资源面，不在本阻塞清扫强改
