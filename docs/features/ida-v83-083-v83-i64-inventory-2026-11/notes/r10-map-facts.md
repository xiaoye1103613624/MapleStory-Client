# map — 功能点事实（13337 / v83.i64）

> 采集：2026-11-12 live IDA MCP  
> 当前实例：`port=13337` → `E:\MXD\反编译IDA\v83\083\v83.i64`  
> 对照 Angel：`port=13339` → `E:\MXD\反编译IDA\v83\`Angel.i64`  
> **禁止**把 13337 当成历史 BeiDou IDB（端口复用）。

## 一句话结论

地图/副本 Field：CField* 等 32 类 / 207 方法。与 Angel IDB（13339）该类索引 **完全一致**。

## 身份

| 项 | 值 |
|---|---|
| 当前 IDB | `E:\MXD\反编译IDA\v83\083\v83.i64` |
| module / input | ``Angel.exe` / `C:\Users\Admin\Desktop\v83 Server\`Angel.exe` |
| input_md5 | `cbe5ccba04cbe2760d8eabc537926bd1` |
| 对照 Angel IDB | `E:\MXD\反编译IDA\v83\`Angel.i64`（同 MD5） |

## 类与方法统计

| 指标 | 13337 (v83.i64) | 13339 (Angel.i64) |
|---|---:|---:|
| class_count | 32 | 32 |
| method_count | 207 | 207 |

### 命中类列表（13337）

| 类 | 具名方法数 |
|---|---:|
| `CField` | 67 |
| `CField_AranTutorial` | 1 |
| `CField_AriantArena` | 1 |
| `CField_Balrog` | 1 |
| `CField_Battlefield` | 9 |
| `CField_Coconut` | 7 |
| `CField_ContiMove` | 5 |
| `CField_CookieHouse` | 1 |
| `CField_Dojang` | 5 |
| `CField_GuildBoss` | 3 |
| `CField_LimitedView` | 4 |
| `CField_Massacre` | 2 |
| `CField_MassacreResult` | 4 |
| `CField_MonsterCarnival` | 10 |
| `CField_MonsterCarnivalRevive` | 1 |
| `CField_MonsterCarnivalS2_Game` | 1 |
| `CField_MonsterCarnivalWaitingRoom` | 1 |
| `CField_PartyRaid` | 2 |
| `CField_PartyRaidBoss` | 2 |
| `CField_PartyRaidResult` | 2 |
| `CField_ShowaBath` | 1 |
| `CField_SnowBall` | 2 |
| `CField_SpaceGAGA` | 2 |
| `CField_Tournament` | 1 |
| `CField_Tutorial` | 1 |
| `CField_Wedding` | 5 |
| `CField_WeddingPhoto` | 1 |
| `CField_Witchtower` | 2 |
| `CLife` | 3 |
| `CMapLoadable` | 36 |
| `CPortalList` | 3 |
| `CWorldMapDlg` | 21 |

## 字符串锚点（抽样）

```json
{
  "patch.mapleglobal.com": {
    "ea": "0xaf7ce8",
    "text": "patch.mapleglobal.com"
  },
  "MapleStory_Global:%s": {
    "ea": "0xb3f2f4",
    "text": "MapleStory_Global:%s"
  }
}
```

## 与 Angel 差异

- 本功能点 feature_index：**equal**
- 两端输入 exe MD5 相同 → 业务语义以任一侧具名符号为准均可；本轮两侧符号量同级（3122 mangled / 399 classes）。

## Evidence

- `evidence/feature-map.json`
- `evidence/live-pyeval-inventory-parsed.json`
- `evidence/identity-live-health.json`
