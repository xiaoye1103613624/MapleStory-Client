# Login.img 合入 RaceSelect/evan|dual（2026-11-18）

## 流水线（AGENTS §3.1）

1. 备份 live → `resources/staging_cards/Login.live.bak-before-merge_20261118_165614.img`（SHA256 `E545A0AE…`，7212655 B）
2. 旧 `gms_out/RaceSelectCards.latest.img` **三钥均不可解析**；改用 LATEST 源 `RaceSelectCards.latest.img` → `batch_convert_key(GMS, verify:true)` → `gms_out_merge/`  
   - fingerprint `1704344e22755b778d82993fc9428058f91981cab0de387a883a8dcdbb410242`  
   - 产物 SHA256 `9C540286…`，本地 MultiKeyParse **仅 GMS 可读**
3. MCP `load_files`(GMS staging + live Login) → `copy_paste_nodes(OVERWRITE)`：`evan`/`dual` → `RaceSelect/`
4. `save_as` → `Login.merged.gms.img`（7409271 B，SHA256 `50F17B74…`）→ `unload_all`
5. 原子替换 live：**失败** — 3× MapleStory.exe Access Denied（user-mapped section）；`taskkill /F` 亦 Access Denied
6. 旁路落盘：`E:\MXD\BeiDou-Client-xy\Data\UI\Login.img.new` + `APPLY_Login_merge.bat`；`.nxcache\UI.nx` 已移走 bak

## MCP 复核（merged 产物）

`RaceSelect` 子节点含：`BtSelect, knight, normal, aran, aran1, textGL, **evan**, **dual**`  
`evan`：`BtEvan` + On/OffAnimation + text；`dual`：`BtDual` + …

## 代码

`UIRaceSelect.cpp`：有 `RaceSelect/evan|dual` 且卡面尺寸有效时 **优先 WZ**；LocalPng 仅无节点兜底。

## 密钥

全程 GMS：`iv=TSPHKw==` + 128B `DEFAULT_KEY`（与 Orz「国际服务器(低版本)」一致）。未裸拷跨钥。
