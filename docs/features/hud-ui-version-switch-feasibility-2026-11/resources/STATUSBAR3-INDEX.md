# StatusBar3 资源索引（192 → xy）

## 来源

| 项 | 值 |
|---|---|
| 源包 | `E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\Data\UI\UI_002.wz` |
| 节点 | `StatusBar3.img` |
| 源 Key | 新版本客户端：`ivBase64=AAAAAA==` + 128B `DEFAULT_KEY`（`tools/_gms_key_128.b64`） |
| 目标 Key | GMS：`ivBase64=TSPHKw==` + 同 128B key（与 xy `StatusBar.img` 一致） |

## 密钥转换门禁（强制）

跨客户端 / 跨 IV 的 `.img` **禁止裸 Copy-Item / robocopy / 移动**。正确闭环：

```text
load(源=新版本零IV) → create_img(同钥) → copy_paste → save_as(staging 异路径)
  → change_key|batch_convert_key(目标=GMS, verify:true)
  → save_as(归档) → unload_all → 原子替换 live
  → --resource-probe 须列出 StatusBar3；短启无 0xc0000374
```

仓库硬规则见根目录 [`AGENTS.md` §3.1](../../../../AGENTS.md)。

## 最终采用路径（2026-11-18）

**路径 C：修本端 `imgdirtonx` 解析器 + 装回全量 GMS StatusBar3.img**

### 根因

`tools/vendor/NoLifeWzToNx-master/NoLifeWzToNx.cpp` `extended_property` 原先：

```cpp
auto & st = strings[read_prop_string(p_offset)];
```

`read_prop_string` → `add_string` 可能 **扩容 `strings` 向量**，使 `st` 悬空。StatusBar3 字符串量远大于 StatusBar.img（约 1.2 万 vs 600），极易触发 UB → 堆损坏 `0xC0000374`。  
orange-wz 可读、本端 probe 跳过/闪退，**不是**「忘了转 key」：`batch_convert_key` 与首次 `create_img(GMS)+paste` SHA256 均为 `93D4AF56…`。

附加防护：

- UTF-16 解密 keystream 上界改为 `65536/2`（避免 `u16key[i]` 越界）
- `WzRuntime::decode_bitmap` 对坏 Canvas **填透明** 而非 throw

### 验证

| 检查 | 结果 |
|---|---|
| live `Data/UI/StatusBar3.img` | 已装回全量包 1,866,425 B（`93D4AF56…`） |
| min_probe `--resource-probe`（修复后 exe） | `ui root children : 25`，含 `[StatusBar3.img] children=10`（mainBar/chat/…），`probe result : OK` |
| canvas 格式（orange-wz `verify_canvas_formats`） | 仅 ARGB4444/ARGB8888，无 flag |
| 短启 | **须先结束所有残留 MapleStory**（含权限拒绝的 PID）；残留进程会导致二次启动误报 `0xC0000374`/`0xC0000005` |

### 用户侧验收（现在）

1. 任务管理器（管理员）结束全部 `MapleStory.exe`
2. 启动 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
3. 确认 ≥5s 无闪退；游戏内 ThemePack classic ↔ modern 切换底栏

## 产物与隔离件

| 路径 | 说明 |
|---|---|
| live `Data/UI/StatusBar3.img` | **已激活**全量 GMS 包 |
| `Data/UI/StatusBar3.img.bad-20261118` | 同哈希备份 |
| `resources/staging_v2/*` | 实验包（mainbar/envelope/gms） |
| 修复后 exe | live + `hud_sidedock_out/MapleStory.exe`（含悬空引用修复） |

## mainBar 消费核对

存在：`mainBar/{status,status800,EXPBar,menu,submenu,quickSlot}` ✅

## 过程教训

1. MCP 可读 ≠ 本端可加载；须 probe + 短启。
2. 跨 key 仍须显式 `batch_convert_key`；本包另踩解析器悬空引用。
3. 短启验收前必须清掉残留客户端进程，否则结论不可信。
