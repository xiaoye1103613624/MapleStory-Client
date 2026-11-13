# inventory — 功能点事实（13337 / v83.i64）

> 采集：2026-11-12 live IDA MCP  
> 当前实例：`port=13337` → `E:\MXD\反编译IDA\v83\083\v83.i64`  
> 对照 Angel：`port=13339` → `E:\MXD\反编译IDA\v83\`Angel.i64`  
> **禁止**把 13337 当成历史 BeiDou IDB（端口复用）。

## 一句话结论

背包装备：CItemInfo/CUIItem/CUIEquip 等 6 类 / 92 方法。与 Angel IDB（13339）该类索引 **完全一致**。

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
| class_count | 6 | 6 |
| method_count | 92 | 92 |

### 命中类列表（13337）

| 类 | 具名方法数 |
|---|---:|
| `CItemInfo` | 48 |
| `CUIEquip` | 5 |
| `CUIItem` | 13 |
| `CUIItemMaker` | 24 |
| `VCUIItem` | 1 |
| `VCUIItemMaker` | 1 |

## 字符串锚点（抽样）

```json
{}
```

## 与 Angel 差异

- 本功能点 feature_index：**equal**
- 两端输入 exe MD5 相同 → 业务语义以任一侧具名符号为准均可；本轮两侧符号量同级（3122 mangled / 399 classes）。

## Evidence

- `evidence/feature-inventory.json`
- `evidence/live-pyeval-inventory-parsed.json`
- `evidence/identity-live-health.json`
