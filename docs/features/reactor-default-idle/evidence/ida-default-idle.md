# IDA / IMG 证据摘要 — Reactor idle / default

日期：2026-11-05  
IDB：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`

## IDA

| 地址 | 符号 | 要点 |
| --- | --- | --- |
| `0x712213` | `sub_71165A` | `push 3C0h` → string id **960** = `default`（info 属性表之一） |
| `0x739A92` | reactor img reader | 读 `info` / `event` / 状态帧；path `Reactor/%s.img`（id 3076） |
| `0x989ADD` | hit sound | event code 51 → `Sound/Reactor.img/%s/%s` + `Hit`（既有音效闭环，本轮未改） |

## IMG / XML

- 路径：`E:\MXD\20dalu_079MS\20dalu\wz\Reactor.wz\`、`BeiDou-Server_s9\gms-server\wz\Reactor.wz\`
- 全量检索 `name="default"`：**0 命中**
- `2201004.img`：`0/{0..6}` canvas + delay 120；`1/` 占位；`action=boss2`

## 规则（落地）

1. 有 `state/default` 则播之；否则播 `state` 下数字 canvas。  
2. 空闲每逻辑帧 `Animation::update()`（末帧 rewind，忽略 `ended`）。  
3. hit 转场结束后按 **新** `state` 重载 idle。
