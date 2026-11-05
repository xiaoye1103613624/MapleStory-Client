# IDA：`#x` 任务进度命中支 + `#B` 双层 bar 合成

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）

## `#x`（case17 → `sub_9A2557` → `sub_4E6A98`）

| 步骤 | 地址 / 证据 | 语义 |
| --- | --- | --- |
| 查表 | `sub_4E6F6C` → `sub_4E901B`（questId → 进度串） | 未接任务 → 全 0 |
| 解析 | `sub_4E6ED1`：按 `/` 切 **3** 段 `a/b/c` | 不足 3 段视为失败 |
| 格式 | `v6 = (b != 0) + 1941` | 命中 1942 / 未命中 1941 |
| pool | `tools/beidou_stringpool.py` | **1941**=`%d%%`，**1942**=`+%d` |
| sprintf | `sub_445B4B(format, c)` | 命中 `+{c}`；未命中 `{c}%`（失败时 `c=0` → `0%`） |

本仓：`QuestLog::get_progress(qid)` + 同构三段解析 → `format_text` 展开。

## `#B`（case12 @ `sub_99DD54`）

| 步骤 | asm / 证据 | 语义 |
| --- | --- | --- |
| 钳制 | `cmp 0Ah` / `64h` | pct ∈ **[10, 100]** |
| 路径 | pool **1374** | `UI/Login.img/Notice/Loading/bar/%d` |
| 底轨 | `push 0Ah` → sprintf | 恒载 **`bar/10`** |
| 分段 | `idiv 10`；`dec eax` | 帧 = **`pct/10 - 1`**（0..9） |
| 余数 | `edx = pct%10` **未存** | 无像素级细分 |
| 合成 | `IWzCanvas::Create` + `Copy`×2（α=255, mode=3） | 先底后填 |
| 资源 | `Login.img/Notice/Loading/bar/0..10` | 皆 **109×8**；`10` 无 delay |

本仓：`Icon.underlay = bar/10`，`Icon.texture = bar/(pct/10-1)`；`Text` 绘制先 underlay 后 texture。
