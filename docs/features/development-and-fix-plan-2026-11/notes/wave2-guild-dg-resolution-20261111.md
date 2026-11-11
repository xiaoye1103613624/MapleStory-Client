# round85 · D-G 公会浮窗入口收敛 + orange-wz key 规格翻案（2026-11-11）

> 依据：用户 m13177「需要，把需要对齐Angel的都修复一下」+ m09859「若碰到ui布局、动画一定要确认是否修复了，如果不是按照 Angel 布局修复过的，可以重新修复」+ 用户 m13030「E:\project\orange-wz 项目是可以启动了，难道mcp功能用不了？程序内置了083密钥的呀！！为什么img解析失败了？还是说重名的问题，没有深入解析？」

## 1. 现场基线

- `HEAD 187e2bc1` / `BRANCH feature/dev_0.0.1` / `AHEAD 0`（「18 commit 未 push」为旧口径，现场 0）/ `STATUS_COUNT 451`
- `GATE OK: opcode matrix matches both sources (s9 a78ba681a4)` `GATE_RC=0`

## 2. D-G 收口：4 处用户可触达旁路 → 083 共用窗

round76 `187e2bc1` 已判 UIGuild = 第 8 例同族病（虚构根 `GuildUI.img` 五证为 0 + 全文字自绘 + 083 全库无独立 `CUIGuild` 类），主路径 `IO/UIStateGame.cpp:361-363` G(17) 早已走 `UIUserList::Tab::GUILD`。但底栏/菜单/快捷键仍留 4 处旁路继续实例化虚构根浮窗：

| # | 位置（修复前） | 修复后 |
| --- | --- | --- |
| 1 | `IO/UITypes/UIStatusBar.cpp:961-965` `BT_MENU_UNION` → `emplace<UIGuild>()` | `get_element<UIUserList>()` + `Tab::UNION` |
| 2 | `IO/UITypes/UIStatusBar.cpp:1081-1089` `BT_COMMUNITY_GUILD` → `get_element<UIGuild>()` / `emplace<UIGuild>()` | 与 `:1047-1079` FRIENDS/PARTY **完全相同的 toggle 模板**（`Tab::GUILD`） |
| 3 | `IO/UITypes/UIMiscShells.cpp:3962-3963` `case '7'` → `emplace<UIGuild>()` | `emplace<UIUserList>(UIUserList::Tab::GUILD)`（与 `case '6'` 同窗） |
| 4 | `IO/UITypes/UIMiscShells.cpp:5899-5903` `'U'\|'u'` → `get_element<UIGuild>()` / `emplace<UIGuild>()` | `get_element<UIUserList>()` + `makeactive()` + `change_tab(Tab::GUILD)` |

083 公会主体本身功能齐备（不用重绘）：`UIUserList.cpp:268-301` 真资源 `Main["Guild"]["GuildInfo"]`（guildinfo6 公告白条 282x20 @(12,78) + 12 个 gi_btn）、`:447-517 draw_guild_tab(float)`、`:813-931 button_pressed` 12 按钮全实现、`:1121-1129` 三张互斥底 312x389、`:1265-1319 bind_runtime_assets()`、`:1441-1455 tab_button()` Tab::GUILD→BT_TAB_GUILD。

**保留不动（架构级）**：`UIGuild.cpp/.h`、`UIElement::Type::GUILD` 枚举保留为 dead-code 兼容占位；删类会动 `UIElement::Type` 序列化、`GuildModel`、底栏入口、s9 handler 与 `dimension`，本轮不做。零协议 / 零 GuildModel / 零 s9 handler / 零 dimension 改动。

`tools/_dg_fix.py` 执行 `DG_FIX_ALL_OK`（5 块替换、EOL 保留）：`UIStatusBar.cpp 63+/27-`、`UIMiscShells.cpp 15+3-`、`UIGuild.cpp 3+`（round85 注释）。`git diff --check` 干净，`RESIDUAL emplace<UIGuild>` = `NONE`（仅 `UIGuild.cpp:60` 注释说明一处）。签名核对：`UIUserList.h:34 UIUserList(uint16_t tab)`、`:60 change_tab(uint8_t tabid)`、`:61 uint16_t get_tab()`，两个 cpp 均 `#include "UIUserList.h"`。

## 3. orange-wz key 规格翻案（三次实验）

| 实验 | key | `load_files` | `query_nodes(op:search,keyword:"Equip")` |
| --- | --- | --- | --- |
| 1（历史） | 65536B 全零 | 成功 | `IMG 解析失败: UIWindow.img` |
| 2（本轮 m09008） | 真 65536B `key_gms`（`tools/_gms_key.b64`，b64_len 87384，sha256_16 `268a8407a47ee76c`） | **成功** `loadedCount=1` | **仍** `IMG 解析失败: UIWindow.img` → 证伪「零 key 是唯一真因 / 内置密钥无故障」 |
| 3（本轮 m09055） | **128B `WzAESConstant.DEFAULT_KEY`**（`tools/_gms_key_128.b64`，`B64_LEN 172 HEAD EwAAAFIAAAAqAAAAWwAAAAgA`） | 成功 | **`isError:false`**，返回 `Equip`(LIST_PROPERTY) / `Equip/BtPetEquipShow` / `Equip/BtPetEquipHide` / `ToolTip/Equip` |

### 源码流向（真因）

1. `ToolParamHelper.java:161-165` `getWzKey` 解 `ivBase64`/`userKeyBase64` → `new WzKey(-1,name,iv,userKey)`，**无长度校验**。
2. `LoadFilesTool.java:38-41` → `service.loadFiles(session,files,key,exclusive)`。
3. `DefaultMcpWorkspaceService.java:84` `new WzImageFile(f.getName(), f.getAbsolutePath(), key.getName(), key.getIv(), key.getUserKey())` —— **只构造、不 parse**（`exclusive:false` 跳过租约）。
4. `WzImageFile.java:36-46` `parse()` 才 `new BinaryReader(filePath, iv, key)`。
5. `WzMutableKey.java:18-22` `this.aesUserKey = getTrimmedUserKey();`、`:84-90` 逐字：

```java
byte[] key = new byte[32];
for (int i = 0; i < 128; i += 16) { key[i / 4] = userKey[i]; }
return key;
```

**只吃前 128B、每 16B 取 1B 拼 32B AES keystream**。`WzAESConstant.java:14-23` `DEFAULT_KEY` 正是这套 128B 经典 WzLib GMS 密钥；`:6` `WZ_GMS_IV={0x4D,0x23,0xC7,0x2B}`=`TSPHKw==`。65536B NoLife `key_gms` 是**全量 XOR 表**，前 128B 布局与 128B 派生密钥不同 → AES keystream 全错 → `parse()` 失败。`IMG 解析失败` 抛出 4 处：`DefaultMcpWorkspaceService.java:1035/:1122-1126/:1214-1218` + `NodePathResolver.java:104-107`。

### 结论与补正

- **orange-wz MCP 路径**：传 **128B `WzAESConstant.DEFAULT_KEY`** base64。
- **`.nx` / nlnx / `tools/_wz.py` 回退路径**：才用 **65536B `key_gms` 全量表**（`tools/_wz.py:32 return [grab("key_bms",65536), grab("key_gms",65536), grab("key_kms",0x20000)]` 佐证）。
- `AGENTS.md:23` 旧「必须 65536B」一刀切口径**错误**，本轮按路径分两套规格补正；真因是**规格错配**，不是零值、不是会话过期（`-32001` 由 `mcp_connector_reconnect` 修）、不是重名、不是服务端故障。
- 历史「orange-wz 内置 083 密钥无故障」结论保留：服务端 `keys.dat` 916B `WZKY` magic + 内置条目确实真实，故障在客户端传参规格。

## 4. 交付与验收

- `tools/test_round85_dg_guild_083.py`：静态守卫（4 处无 `emplace<UIGuild>` / UIStateGame G 键仍 `Tab::GUILD` / UIGuild 类保留 / AGENTS.md 双规格 / 128B 实证）+ 场景（4 入口路由真值表 + toggle 语义 6 组）。
- `python tools/gen_opcode_matrix.py --check` → GATE OK。
- `MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c tools\build_smoke.bat` → `EXITCODE=0` 真重编覆盖 3 文件。
- 四文档：`evidence/guild-window-083-round85.json` / 本 notes / dev-plan §4 **2-32** / `pending-live-tests` **§43**（并更新 §42 D-G 状态）。
- 精确 `git add` + 零署名原子提交（git-commit-standard）。

## 5. 仍 open

真机五屏截图观感、D1–D14 其余裁决（D-A/D-B/D-C/D-C2/D-D/D-E/D-F/D7-A）、`InPacket::available()` 393 处、跨面矩阵残差 28、L-B 销案口径追认、round78 UIEquipInventory 收口、独立 CUIPartyHP / 五页签 / SP_4433 / 9 钮 / 名牌像素 / HUD 闪烁启动链、goal-11388c42 active。
