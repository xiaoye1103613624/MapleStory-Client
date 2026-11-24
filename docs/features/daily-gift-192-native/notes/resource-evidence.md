# dailyGift 192 原生资源取证 + 192零IV → GMS 转 key 流程验证

> 任务性质：**只读取证 + 限定产物**。未注入任何 live `Data`，未改动任何仓库既有文件。
> 执行日期：2026-11-23（Asia/Shanghai）
> 工具：orange-wz MCP `http://127.0.0.1:10012/mcp`，经 `tools/_orange_wz.py`（init → tools/call，Mcp-Session-Id 复用）。
> 门禁依据：`AGENTS.md §3.1` 跨客户端资源迁移硬门禁（禁止裸拷 `.img`；必须 `load_files → save_as(异路径staging) → batch_convert_key(目标GMS key, verify:true) → probe/短启`）。

---

## 1. 源与密钥规格

| 项 | 值 |
| --- | --- |
| 源 WZ | `E:/MXD/【2023.8.8江浩生日福利】怀旧岛192整合版/怀旧岛192客户端/冒险岛online192/Data/UI/UI_005.wz`（96,635,180 B，mtime 2026-09-14 18:14，**未改动**） |
| 入口 WZ | `…/Data/UI/UI_002.wz`（89,347,302 B，mtime 同上，**未改动**） |
| 源 key（192 零 IV） | `tools/_wz192_spec.json` → name=`gms192`, ivBase64=`AAAAAA==`, userKeyBase64 完整（首页 HEAD=`EwAAAFIAAAAqAAAAWwAAAAgAA…`） |
| 目标 key（GMS） | `tools/_gms_key_128.b64`（128B / B64_LEN 172 / HEAD `EwAAAFIAAAAqAAAAWwAAAAgA`）+ IV `TSPHKw==` |
| `exclusive` | `false`（全程遵守 AGENTS.md §3） |

**MCP 会话关键陷阱**：MCP 会话按进程隔离。`load_files` 与后续 `find_node/get_node_tree_json` 必须在**同一 python 进程 / 同一 MCP session** 内，否则节点查询返回「未找到节点」（GBK 原文，UTF-8 解码呈乱码）。本轮所有多步流程均单进程内顺序执行；每轮结束 `unload_all(hintGc:true)` 收口（返回 `{"ok":true,"rootCount":0}`）。

---

## 2. `UIWindow4.img/dailyGift` 节点清单

`load_files(UI_005.wz, gms192, exclusive:false)` → `{"rootCount":1,"exclusive":false,"loadedCount":1}`。
`UIWindow4.img` 共 **124** 个直接子节点，`dailyGift` 为第 92 个（LIST_PROPERTY）。
`get_node_tree_json(nodePath="UIWindow4.img/dailyGift", maxDepth=30)` 导出完整子树：**279 行节点，其中 CANVAS/UOL 节点 63 个**，全部 CANVAS 均为 `pngFormat=ARGB4444`，节点均带 `_hash`（SHA256-of-pixels，便于后续比对）。

### 2.1 根节点与背景（关键尺寸）

| 节点 | type | 尺寸 | origin (x,y) | 备注 |
| --- | --- | --- | --- | --- |
| `dailyGift` | LIST_PROPERTY | — | — | 子树根 |
| `dailyGift/backgrnd` | CANVAS_PROPERTY | **741×647** | (0, 9) | `_hash=8c603f98…75dd` |
| `dailyGift/backgrnd2` | CANVAS_PROPERTY | **741×677** | (0, 9) | `_hash=16b56f70…69e`；**导出 PNG 并视觉确认** |
| `dailyGift/complete` | CANVAS_PROPERTY | 100×100 | (0, 0) | z=0，`_hash=62efdd2e…7cb8` |

### 2.2 位置向量（VECTOR_PROPERTY，`get_node_detail` 读 x/y）

| 节点 | x | y | 节点 | x | y |
| --- | --- | --- | --- | --- | --- |
| termPos0 | 505 | 47 | termPos4 | 505 | 65 |
| termPos1 | 515 | 47 | termPos5 | 515 | 65 |
| termPos2 | 547 | 47 | termPos6 | 547 | 65 |
| termPos3 | 557 | 47 | termPos7 | 557 | 65 |
| countPos0 | 677 | 537 | countPos1 | 637 | 537 |

> 解读：termPos* 两行四列（y=47/65，x=505/515/547/557）对应日期区间数字；countPos0/1（y=537，x=677/637）对应「等级范围怪物」计数显示。

### 2.3 数字帧：`term/0..9` 与 `count/0..9`

| 组 | 帧数 | 单帧尺寸 | origin | 说明 |
| --- | --- | --- | --- | --- |
| `dailyGift/term/0..9` | 10 | 7–9 × 14（0:9×14, 1:7×14, 2:9×14, 3:8×14, 4–9:9×14） | (0,0)，帧3 为 (-1,0) | 十位数字 0–9 |
| `dailyGift/count/0..9` | 10 | 8×14 | (0,0) | 个位数字 0–9 |

### 2.4 按钮：`button:Close` / `button:Reward`（四态）

| 按钮 | 态 | 尺寸 | origin | 链接 / id |
| --- | --- | --- | --- | --- |
| `button:Close` | normal | 1×1 | (-720, -4) | `_outlink=UI/Basic.img/BtClose3/normal/0` |
| | pressed | 1×1 | (-720, -4) | `_outlink=UI/Basic.img/BtClose3/pressed/0` |
| | disabled | 13×13 | (-720, -4) | 实体像素 |
| | mouseOver | 1×1 | (-720, -4) | `_outlink=UI/Basic.img/BtClose3/mouseOver/0` |
| | — | `id=1000` (INT_PROPERTY) | | |
| `button:Reward` | normal | 198×70 | (-521, -563) | `_hash=70e74827…e2a` |
| | pressed | 198×70 | (-521, -563) | `_hash=2ae91798…734` |
| | disabled | 198×70 | (-521, -563) | z=0，`_hash=0b4f1190…3e2` |
| | mouseOver | 198×70 | (-521, -563) | `_hash=c63f1c2a…f0784ba` |
| | — | `id=1001`；`ToolTip_Disabled='只有今日狩猎的等级范围怪物达到300只以上的角色才能领取礼物。'`；`useDisableToolTip=1` | | |

> `button:Close` 三态走 `_outlink` 到 `UI/Basic.img/BtClose3/*`，仅 disabled 是本地实体像素；迁移/裁剪时必须保留 UOL 语义或补齐 Basic.img 依赖。

### 2.5 MVP 奖励：`mvpMedal/1..7`、`mvpLv/*`、`mvpBonus/*`

| 组 | 帧/节点 | 尺寸 | origin |
| --- | --- | --- | --- |
| `mvpMedal/1..4` | 4 | 28×36 | (-1, 2) |
| `mvpMedal/5..7` | 3 | 32×42 | (-1, 2) |
| `mvpLv/backgrnd` | 1 | 171×88 | (-1, 1) |
| `mvpLv/backgrnd2` | 1 | 172×304 | (0, 6) |
| `mvpLv/1..4` | 4 | 80×81 | (-5, 6) |
| `mvpLv/5` | 1 | 82×78 | (-4, 4) |
| `mvpLv/6` | 1 | 82×79 | (-4, 5) |
| `mvpLv/7` | 1 | 82×81 | (-3, 5) |
| `mvpLv/day_21` / `day_23` / `day_28` / `day_11_01` / `day_11_02` | 5 | 172×304 | (0, 6) |
| `mvpLv/day_11_03` | 1 | **172×174** | (0, 6) |
| `mvpLv/day_07_01` | 1 | 172×304 | (0, 6) |
| `mvpLv/day_07_02` | 1 | 1×1 | (0, 6)，`_inlink=dailyGift/mvpLv/day_11_03` |
| `mvpLv/day_07_03` | 1 | 172×304 | (0, 6) |
| `mvpBonus/1/2` / `2/2` | 2 | 86×34 | (-72, -23) |
| `mvpBonus/3/2` | 1 | 88×34 | (-72, -23) |
| `mvpBonus/4/2` | 1 | 87×34 | (-72, -23) |
| `mvpBonus/5/2` / `6/2` | 2 | 81×34 | (-72, -23) / (-72, -23) |
| `mvpBonus/7/2` | 1 | 81×34 | (-74, -22) |

完整逐节点清单（含 `_hash`）见 `tools/_dg_inventory.json`、`tools/_dg_tree_inventory.txt`（本轮临时产物，位于 `tools/`，非仓库既有文件）。

---

## 3. `backgrnd2` PNG 导出与视觉确认

`get_node_tree_json(nodePath="UIWindow4.img/dailyGift/backgrnd2", includePng:true, maxDepth=1)` → 返回 `png` base64 字段（346,708 字符），落盘：

| 项 | 值 |
| --- | --- |
| 临时 PNG | `C:/Users/11036/AppData/Local/Temp/dg_backgrnd2.png` |
| 尺寸（读 PNG IHDR） | **741×677**（与 MCP 报告 canvas 一致） |
| 字节数 | 260,029 |
| SHA256 | `1808131b00140536ba39f10354123babe4c789a0c64ace128d531369f363be0c` |

**视觉确认（read_image 直读）**：图像为「妖精布洛的每日礼物」原生 UI —— 顶部绿色标题横幅 + 右侧卡通人物；中部 7×4 共 **28 个日期格**（第1天…第28天）；底部左侧规则说明（含「仅今日狩猎等级范围怪物数量达到300只以上的角色能领取礼物」「LV.33以上…」「同一账号中的角色，每天仅有一次领取礼物的机会」「当天的奖励在23:45为止可以领取，第二天的奖励可在00:15开始领取」等中文文案）；底部右侧「等级范围怪物 /」输入框。与用户所述 192 原生 dailyGift 截图一致。
> modlens 无 vision provider（`No vision provider is set up on this machine`），改用 DSH 内置 `read_image` 完成视觉确认。

---

## 4. Statusbar3 入口按钮四态（UI_002.wz）

`find_node(UI_002.wz, "StatusCode")` 无关；实际入口：
`StatusBar3.img/mainBar/submenu/event/button:dailyGift`（LIST_PROPERTY，`search_node(keyword="dailyGift")` 命中）。

| 态 | 尺寸 | pngFormat | origin | z | 说明 |
| --- | --- | --- | --- | --- | --- |
| normal/0 | **52×14** | ARGB8888 | (-32, -67) | 1 | 实体像素，`_hash=88b6c43a…bae2` |
| pressed/0 | **60×16** | ARGB8888 | (-28, -65) | 1 | 实体像素，`_hash=e87880c0…793ea` |
| disabled/0 | 1×1 | ARGB4444 | (-32, -67) | 1 | `_inlink=mainBar/submenu/event/button:dailyGift/normal/0` |
| mouseOver/0 | 1×1 | ARGB4444 | (-28, -65) | 1 | `_inlink=mainBar/submenu/event/button:dailyGift/pressed/0` |
| `id` | — | INT_PROPERTY | — | — | `value=2` |

> 入口四态全部为**本地节点 + img 内 inlink**，不依赖外部 Basic.img；但 disabled/mouseOver 是指针态，迁移时若裁剪须保证 inlink 目标同在。

---

## 5. 192零IV → GMS 转 key 流程验证（重点）

### 5.1 精确 MCP 调用序列（单会话内，已实测跑通）

```text
init()  # initialize + notifications/initialized
1) load_files(paths=["…/UI_005.wz"], key=gms192{AAAAAA==}, exclusive=false)
   → {"rootCount":1,"exclusive":false,"loadedCount":1}
2) create_img_file(fileName="E:/project/MapleStory-Client/tools/_staging_dg/DailyGiftStage.img",
                   key=gms192)                       # 建议先与源同 key，避免跨钥粘贴歧义
3) load_files(paths=[STAGE_IMG], key=gms192, exclusive=false)
   → 注意：create_img_file 返回的 IMAGE 根虽在 list 中，粘贴需显式 load_files 使其成为可写 root
   （本轮第 3 步返回「根已加载」提示但 copy_paste 仍 ok，说明 create 已建立可写根；保留此步以确保加载态一致）
4) copy_paste_nodes(
     sources=[{rootPath:UI_005.wz, nodePath:"UIWindow4.img/dailyGift"}],
     targets=[{rootPath:STAGE_IMG, nodePath:""}],
     strategy="overwrite", autoParse=true, clearClipboard=true)
   → {"ok":true,"pasted":[{"name":"dailyGift",…}],"generation":2}
5) save_as(rootPath=STAGE_IMG, filePath=STAGE_IMG, autoParse=true,
           clearCache=true, unloadAfterSave=false)
   → {"ok":true,"rootCount":2}
   # ⚠ 若直接 save_as(rootPath=UI_005.wz, nodePath="UIWindow4.img/dailyGift", filePath=…)
   #   会报「该节点不支持另存为: WzListProperty」——LIST_PROPERTY 必须经 create_img_file 落为 IMAGE 根
6) batch_convert_key(paths=[STAGE_IMG], sourceKey=gms192,
                     targetKey=gms{iv=TSPHKw==,128B}, outputDir="…/_staging_dg/gms",
                     verify=true, overwrite=true, parallelism=2)
   → {"total":1,"success":1,"failure":0,"verify":true,"ok":true,
      "results":[{"path":"DailyGiftStage.img","success":true,"message":"ok+verified",
                  "fingerprint":"c03fe26c4a00be85a960045e5d69c7592bcee02d0a64d5dbaa94a11aa8c26423"}]}
7) unload_all(hintGc=true) → {"ok":true,"rootCount":0}
```

### 5.2 产物与哈希

| 文件 | 路径 | 字节 | SHA256 |
| --- | --- | --- | --- |
| 192 源 key staging IMG | `tools/_staging_dg/DailyGiftStage.img` | 764,391 | `f31e030a7d3ef3b6e145f9cfa893b78e476f971f73072794365d11098bb71715` |
| **GMS 转换产物（verify:true）** | `tools/_staging_dg/gms/DailyGiftStage.img` | 764,391 | `42060bbaab9e05bf5dfad4e6033620fd859276587dd69223aee56e5a65677250` |
| 视觉确认 PNG | `%TEMP%/dg_backgrnd2.png` | 260,029 | `1808131b00140536ba39f10354123babe4c789a0c64ace128d531369f363be0c` |

- `batch_convert_key` 指纹：`c03fe26c4a00be85a960045e5d69c7592bcee02d0a64d5dbaa94a11aa8c26423`（服务端 `ok+verified`）。
- 两 IMG 等长但 SHA256 不同 → 确实发生重加密，非裸拷。
- 暂存根 `tools/_staging_dg/` **与 loaded 根不同**，符合 AGENTS.md §3.1「save_as 异路径」要求。

### 5.3 双向 key 解析验证（证明转换语义真实生效）

| 加载方式 | `load_files` 结果 | 节点解析（`find_node/get_node_tree_json dailyGift/backgrnd2`） |
| --- | --- | --- |
| GMS 转换产物 + **GMS key**（128B + TSPHKw==） | `rootCount:1` | ✅ `find_node` 返回 CANVAS_PROPERTY；`get_node_tree_json` 返回完整 dailyGift 子树，`backgrnd=741×647`、`backgrnd2=741×677`，`_hash` 与源逐字节一致（像素内容未损） |
| GMS 转换产物 + **192 零IV key** | `rootCount:1`（仅头部装载成功） | ❌ `find_node`/`get_node_tree_json` 抛 `IMG 解析失败: DailyGiftStage.img`（GBK 原文，UTF-8 呈乱码） |

**结论**：`batch_convert_key(192→GMS, verify:true)` 产物只能用目标 GMS key 解析，源 192 零IV key 已无法解析 → 转 key 流程**可行且语义正确**，可作为后续跨客户端迁移的过门证据。
> 注意：`load_files` 本身对错误 key 仍返回 `rootCount:1`（只认文件头、不做全量解密），**不能**把它当过门证据；必须用节点级 `find_node/get_node_tree_json` 或 `batch_convert_key verify:true` 判定。这与 AGENTS.md §3.1「change_key 单独不算过门证据」的教训一致。

### 5.4 `change_key` 单独路径（未单独作为证据）

本轮未调用 `change_key`；按 AGENTS.md §3.1 教训，其 `verify:true` 可能「verified:true 但落盘仍按源 IV 可解」，故**不单独作为过门证据**，本流水线仅采用 `batch_convert_key`。

---

## 6. 风险与后续

1. **尚未注入 live，未做客户端 probe / 短启**：本任务限定为取证 + staging 产物。按门禁，正式入库仍需：客户端关闭 → 原子替换进 live `Data` → `--resource-probe` 确认 UI 根含目标 `.img` → 短启无 `0xc0000374` 堆损坏。AGENTS.md §3.1 记录 StatusBar3 先例：裸 `create_img_file(GMS)+copy_paste+save_as` 未显式转 key 的包会让客户端短启闪退；本轮已走显式 `batch_convert_key(verify:true)`，但仍须以本端 probe/短启为最终验收。
2. **UOL 依赖**：`button:Close` normal/pressed/mouseOver 走 `_outlink=UI/Basic.img/BtClose3/*`，`mvpLv/day_07_02` 走 `_inlink`。裁剪/迁移时若只搬 dailyGift 子树而丢依赖，按钮/日期格会缺图。
3. **编码**：GMStarget 128B key 与 192 零IV key **不可混用**（前者 AES 派生吃 128B 每 16B 取 1B，后者 IV 全 0）；MCP 错误文本为 GBK，UTF-8 解码呈乱码，留证时按原始字节记录。
4. **橙色告警**：`create_img_file` 直接产出的 staging img 与源 key 相同；若后续流程跳过 `batch_convert_key` 而直接 `change_key(save/verify)`，须额外 probe 验证，不得宣称已过门。
5. **产物定位**：全部临时脚本 / JSON / staging IMG 均在 `tools/_dg_*` 与 `tools/_staging_dg/`，PNG 在 `%TEMP%`；**无任何 live `Data` 写入**，源 WZ mtime 保持 2026-09-14。

---

## 7. 本轮落盘产物索引

| 类别 | 路径 |
| --- | --- |
| 报告（本文） | `docs/features/daily-gift-192-native/notes/resource-evidence.md` |
| 完整节点清单（逐行） | `tools/_dg_tree_inventory.txt`（279 行） |
| 富化清单（origin/链接） | `tools/_dg_inventory.json`（63 条 CANVAS/UOL） |
| 原始树 JSON | `tools/_dg_probe3_out.json` |
| 向量 / Statusbar3 / PNG 导出原始 | `tools/_dg_probe4_out.json` |
| staging + 转 key 原始输出 | `tools/_dg_stage_convert2_out.json` |
| 双向 key 验证原始输出 | `tools/_dg_verify_out.json` |
| staging IMG | `tools/_staging_dg/DailyGiftStage.img`（192 key） |
| GMS 产物 IMG | `tools/_staging_dg/gms/DailyGiftStage.img`（verify:true） |
| 视觉 PNG | `%TEMP%/dg_backgrnd2.png` |
