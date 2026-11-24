# Orz「国际服务器(低版本)」vs orange-wz MCP — Login.img 对照（2026-11-18）

## 0. 截图确认

用户截图（`...XiaoYe_2026-11-18_16-47-28-....png`）：

- 顶部下拉为 **「国际服务器(低版本)」**
- 左侧打开 `Data\UI` 目录；`Login.img` 选中，类型 **IMAGE**，已展开 `BtCancel` / `BtOk` / `CharSelect` 等

## 1. Orz 密钥档位实际字节

| 来源 | 结论 |
| --- | --- |
| 源码 `WzKeyStorage.initialFile()`（`E:/project/orange-wz/.../WzKeyStorage.java:49`） | `"国际服务器(低版本)"` → `WZ_GMS_IV` + `DEFAULT_KEY` |
| 安装目录 `D:\software\OrzRepacker-v1.162.50\keys.dat` 解密实测 | 该档 `ivBase64=TSPHKw==`，`userKeyLen=128`，`userKeyBase64` 与 `WzAESConstant.DEFAULT_KEY` **逐字节相等**（`matchesDEFAULT_KEY=true`，`matchesGMS_IV=true`） |
| `E:\project\orange-wz\keys.dat` | 同上三档，与 Orz 安装 keys.dat 规格一致 |

三档对照（Orz / orange-wz keys.dat 一致）：

| name（源码） | ivBase64 | userKey |
| --- | --- | --- |
| 国际服务器(低版本) | `TSPHKw==`（GMS `4D 23 C7 2B`） | 128B `DEFAULT_KEY` |
| 亚洲服务器(低版本) | `uX1j6Q==`（CMS） | 同 128B |
| 新版本客户端 | `AAAAAA==` | 同 128B |

**Orz「低版本」没有用 65536 `key_gms`，也没有另一套 DEFAULT_KEY。**

## 2. MCP 常用 GMS128

| 项 | 值 |
| --- | --- |
| ivBase64 | `TSPHKw==` |
| userKeyBase64 | `tools/_gms_key_128.b64`（LEN 172，HEAD `EwAAAFIAAAAqAAAAWwAAAAgA`） |
| 与 Orz 低版本 | **完全一致**（与 `DEFAULT_KEY` / keys.dat 第一档相等） |

## 3. 复现对照（本轮实测）

目标文件：`E:\MXD\BeiDou-Client-xy\Data\UI\Login.img`  
size **7212655**；SHA256 **E545A0AE3FE4AC842DFF47DAF2AA88850D884220E0023032289899ABA030F21C**

### 3a. 本地 orange-wz 库（同 MCP 同源类）`MultiKeyParse`

| key | parse | RaceSelect |
| --- | --- | --- |
| GMS (`TSPHKw==`+128B) | **true**，children=19 | **true** |
| CMS | false | — |
| LATEST (`AAAAAA==`) | false | — |

### 3b. MCP `load_files` 单文件 + GMS128

```text
load_files(Login.img, exclusive:false, key={name:GMS128, iv:TSPHKw==, userKey:128B})
→ loadedCount:1
query_nodes(search, keyword=RaceSelect) → LIST_PROPERTY RaceSelect
list_children(nodePath=RaceSelect) → BtSelect, knight, normal, aran, aran1, textGL
```

### 3c. MCP `load_files` 整目录（对齐 Orz 入口）

```text
load_files(Data\UI, exclusive:false, key={name:国际服务器(低版本), iv:TSPHKw==, userKey:128B})
→ loadedCount:1
list_children(Login.img/RaceSelect) → 同上 6 子节点
```

收口：`unload_all` → `rootCount:0`。

## 4. img 单文件 vs 目录 / wz 路径

`DefaultMcpWorkspaceService.loadFiles`：

- `.img` → `WzImageFile`（懒解析；`list`/`search` 时 `autoParse`）
- 目录 → `WzFolder`（与 Orz 打开 UI 文件夹同类入口）
- `.wz` → `WzFile` / `WzDirectory`

本端 `Login.img` 是独立 img；**单文件与 UI 目录两种入口在本轮均成功**。无「仅 Orz 能开、MCP 因入口不同永久不能开」的差异。

`WzImage.parse` 错钥时走 `ERROR_KEY` / `ERROR_SPECIAL_ENCODE` → MCP 抛 `IMG 解析失败`（`NodePathResolver`）。CMS/LATEST 本地复现即此路径。

## 5. 与上一轮 `login-img-gms-retest` 的关系

上一轮同 SHA256、同 GMS128 曾报 `IMG 解析失败`。本轮 **同文件同钥成功**。因此：

- **不是**「Orz 低版本钥 ≠ MCP GMS128」
- **不是**「本端 Login.img 对 orange-wz 永久不可解析」
- 上一轮失败更可能是会话/服务端瞬时状态（根锁、进程态、未收口 load 等），**不能**再当作解析器兼容缺口的定论

## 6. 结论（二选一坐实后的实测裁定）

**钥一致，且正确 MCP 参数下可打开：Orz「国际服务器(低版本)」≡ MCP GMS128（`TSPHKw==` + 128B `DEFAULT_KEY`）。**

正确参数示例：

```json
{
  "name": "GMS128",
  "ivBase64": "TSPHKw==",
  "userKeyBase64": "<tools/_gms_key_128.b64 全文>"
}
```

本轮已成功 `list RaceSelect`（单文件与 `Data\UI` 目录两种入口）。  
LocalPng 绕行仍可作运行时兜底；**Login.img mutate 合入不再被「解析失败」阻塞**，可按 AGENTS §3.1 走 staging → 合入。

## 7. 建议跟进

1. 用 GMS128 `load_files(Login.img)` → mutate 写入 `RaceSelect/evan|dual`（来自已转钥 staging）→ `save_as` 异路径 → 原子替换。
2. 若再遇 `IMG 解析失败`：先 `unload_all` + 核对 `iv/userKey` + 本地 `MultiKeyParse`；勿直接改判为解析器缺口。
