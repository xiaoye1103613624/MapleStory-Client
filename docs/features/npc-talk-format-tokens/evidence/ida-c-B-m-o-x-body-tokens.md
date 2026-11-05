# IDA：`#c` / `#B` / `#m` / `#o` / `#x` 带 body 令牌

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）

## 分类（`sub_9A2DE0` @ `0x9A2DE0`）

| `token[1]` | enum | 元素类型 | 语义 |
| --- | --- | --- | --- |
| `'B'` | **12** | type **2**（`*elem=2`） | 进度条 canvas |
| `'c'`/`'m'`/`'o'`/`'x'` 等 | **17**（default） | 正文名/数 | 插入文本元素 |

## case 17 正文分发（`sub_99DD54`）

`atoi(token+2)` 后读 `token[1]`：

| tag | 证据要点 | 本仓展开 |
| --- | --- | --- |
| `'c'` | `sub_A2858C(itemid)` 扫背包汇总数量 → 插字符串 | `#c` + 十进制数（保留橙色素标） |
| `'m'` | pool **1730**=`mapName` → `sub_5CF792(mapid)` | `NxHelper::Map::get_map_info_by_id().name` |
| `'o'`（111） | 与 `#p` 同族正文名；读 Mob 名 | `String/Mob.img/<id>/name` |
| `'x'` | `sub_9A2557`→`sub_4E6A98`；命中 1942=`+%d` / 未命中 1941=`%d%%` | 见 `ida-x-quest-progress-B-dual-bar.md` |
| `'h'`/`'p'`/`'q'`/`'t'`/`'z'` | 已在前轮闭合 | （不变） |

`#c` **不是**持久色标族的独立语义；数量文本以橙色绘制。本仓把 `#c<id>#` 换成 `#c`+数字，LayoutBuilder 对 `#c` 仍 `ORANGE` + `skip+=2`。

## case 12 进度条（`'B'`）

| 步骤 | 证据 |
| --- | --- |
| 解析 | `atoi(body)`，`<10→10`，`>100→100` |
| 路径 | pool **1374** = `UI/Login.img/Notice/Loading/bar/%d` |
| 叠图 | 原生：`bar/10` + `bar/(pct/10-1)` 双层 Copy（α=255/mode3） |
| 本仓 | `Icon.underlay=bar/10` + `texture=bar/(pct/10-1)`；详见 `ida-x-quest-progress-B-dual-bar.md` |

## 打字机原子化

| 形态 | take |
| --- | --- |
| `#B…#`（及残留 `#m/#o/#x…#`） | 吃到闭合 `#` |
| `#c`（展开后色标） | **2** 字节（与 `#b/#r/…` 同） |

## 局限

- `#x` / `#B` 命中与双层合成已接线（本轮）；不真机
- 进度串非 `a/b/c` 三格式时仍走未命中支 `"0%"`（与原生解析失败一致）
