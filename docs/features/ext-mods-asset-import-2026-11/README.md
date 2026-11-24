# 缺失资源批量导入 + 侧栏图标入口 · 交付记录（2026-11-20）

## 用户指令
"缺失的资源也全部导入，快捷入口可以加到左侧竖栏图标按钮中：E:\MXD\扩展改动\左侧小图标"
决策（ask_user）：#12 15个wz全导(含1.08GB) / #7 卡片名称走XML / 图标给现有功能挑对应。

## 点到面差集（导入前精确计算）
| 类 | 总数 | 缺失 | 处理 |
|---|---|---|---|
| #5 动漫皮肤 | 96 img | 96 全缺 | ✅ 已导入 |
| #6 阿尔泰皮肤 | 112 img | 0 | 无需动（早前已导） |
| #7 卡片名称 | Ins.img | 1（非标准头、明文XML） | ✅ 走XML合并 |
| #12 083时装 wz | 15 | 15 全缺 | ❌ 阻塞（见下） |
| #13 NameTag | 1 img | 1 | ✅ 已导入 |

## 已完成（有证据）
### #13 NameTag（修复名片错位）
- 备份 live `UI/NameTag.img` → staging `NameTag.live.bak`
- 源 5.68MB 转 key（batch_convert_key verify:true, 1/1 success）
- probe：374 根子节点 / 3470 节点（maxDepth2）结构完整
- 原子替换回 live：SHA `136FDC87…` → **`84B988ED92775F19409DE25722EFC22992B8DEFF9AA6E05EE47E4F7D1B00D76C`**（与产物一致）

### #5 动漫皮肤（96 img）
- batch_convert_key 96/96 success、failure:0、verify:true
- probe 抽样 01076000.img：36 根子节点 / 145 节点
- 96 个 img 全部复制到 live `Data/Character/Shoes/`（计数验证 = 96）

### #7 卡片名称（XML 合并）
- 需求包 Ins.img.xml 是服务端现有 294 条的**超集**（3766 条）
- 合并脚本 `merge_ins_xml.py`：以服务端为底、只补缺失，备份 `.bak-cardname-20261120`
- 两个文件均从 294 → **3816 条**（wz/String.wz 与 wz-zh-CN/String.wz）
- XML 结构验证：有效、根=Ins.img、3816 直接子=3816 数字条目、首=3010000

### 侧栏图标入口（图标包 E:\MXD\扩展改动\左侧小图标）
- 图标包结构：ICON/1..21 + icon2/，8842 png，命名 `Name.Default/Appear/Notify.Frame.png`
- live `UI/SideIcons/` 原有 book/bulb/star 3 对（不动）
- 从原生素材新增 3 对：`checkin`(dailyGift.Default/Notify.0) / `damagerank`(Achieve) / `colorprism`(GalaxyStar)
- ini 追加 slot.3/4/5（备份 `.bak-before-exticons-20261120`），6 个 slot 与 6 对图标一一对应
- arg(DailyCheckin/DamageRank/ColorPrism) 与 `UISideIconBar.cpp:423/428/433` 的 open_ui 分支名匹配
- 未做 EventList/itemCollection 图标：Event 已有入口、装备对比是悬停 tooltip 无独立窗口——不造"点了没反应"的图标

## #12 解包路线追查（2026-11-20 续，用户选"找解包工具继续导"）
穷尽三条路径，全部有证据：
1. `batch_convert_key` 直接转 15 wz：1 成功（1.8MB）/13 失败（`BufferUnderflow`/`null`）——工具大小上限。
2. **关键修正**：`load_files` 直接加载失败样本（6MB gms083戒指.wz）**成功**，1531 个根节点全是 IMAGE（01112000.img 等）——说明读取路径没问题，是 batch_convert_key 的问题。
3. `save_as(nodePath="01112000.img")` 导出单个 img → isError:true、文件未生成——工具不支持从 wz 子节点导出独立 img。
4. 本机 WzComparerR2 有完整 WzLib 源码（Wz_File/Wz_Image.OpenRead/ExtractImg）+ dotnet 10 可用，但 **PKG1（Package file v1.0, GMS083 老格式）兼容性未验证**，写 C# 解包器是重工程且前景不明。

**追加追查（用户"能用就用它、任务全部闭环"后，2026-11-20 续）**：
- 尝试用 orange-wz 自带 Java API 解包：`WzFile.parse()` + `exportFileToImg()` + `WzImage.save()`（照搬历史范例 MergeDropItemAuraFromWz.java:40-55），编译并运行成功（javac 21 + target/classes 作 classpath；注意 classpath 必须用 `E:/` 盘符而非 MSYS `/e/`，且导出类不要声明 package 否则 ClassNotFound）。戒指.wz 日志显示 `exported=1531 failed=0`。
- **但 probe 证伪**：解出的 1531 个"img"全部是 6454394 字节、头 `PKG1`——即整个 wz 的副本，不是单个 img。`image.save()` 对这批 PKG1 未真正写出独立 img（parse/save 短路或回退复制）。**不算成功，不导入**。
- **最终定论（源码级+文件级双证）**：读 `WzImage.save(Path)`（WzImage.java:111-157）——它用 `reader.getWzMutableKey()` 写 BinaryWriter；对这批 PKG1 wz，parse 出的 WzImage 的 reader 仍指向整个 wz 流，save 结果是**整 wz 原样复制**。文件级验证：连只含 1 个 img 的小 wz（gms083全部的名字.wz）解出的 `Eqp.img` 仍是 1884837B、头 `504b4731`（PKG1）= 整个 wz；1531 个导出文件全部 6454394B PKG1 头、probe 子节点=0、无 .bak 残留。
- 至此 orange-wz 全部路径（batch_convert_key / save_as / copy_paste_nodes / exportFileToImg API / .bak 回收）均无法把该 PKG1 wz 拆成独立 .img。**这是 orange-wz 对该批 PKG1 资源的能力边界，非密钥、非资源损坏、非 API 用法错**，证据链完整闭合。

## 【最终结论 · 2026-11-20 复盘纠错】15 个 wz 全部不需要导入——差集口径错了

**根因**：之前判定"15 个 wz 全缺"是按 **wz 文件名**在 live 查（live 里 0 个 wz → 全缺）。但 live 是**散装 .img 结构**，正确口径应按 wz **内部的 img 名**查。

**正确差集（wz_diff_audit.py，MCP load 15 个 wz 读 img 名 + live 121733 个 img 文件名索引）**：
| 统计 | 值 |
|---|---|
| 15 个 wz 内 img 总数 | **35954** |
| live 已存在 | **35954** |
| 真缺 | **0** |

逐 wz 全部 `缺=0`（头发5729/套服1562/脸型9668/戒指1531/帽子3000/披风1330/武器1207+54/鞋1349…）。抽样 probe 3 个（01112000/01008900/01076000）全部 load OK、节点树完整。

**定性**：这批 wz 的内容早在历史插件时代就全导进 live 了（跟天气系统 lamp2"物理缺席"误判同一类：只搜包没搜 live）。前面所有"解包/转换/Java API"工作是基于错误差集的**无效功**，wz 本身**无需任何处理，任务闭环**。

**教训（SOP）**：判定资源是否缺失，必须按目标端真实结构（live 散装 img → 按内部 img 名查），禁止按分发包名（wz）查——文件名口径错会制造整个虚假任务。

## 阻塞交接（#12 15 个 PKG1 wz）
- 用 orange-wz `batch_convert_key` 转 15 个 wz：**成功 1（1.8MB gms083全部的名字.wz）、失败 13**（`java.nio.BufferUnderflow` / `null`）；
- 根因（实锤、非猜测）：**live Data 里 0 个 .wz 文件**——该客户端用散装 .img 结构，PKG1 wz 是分发打包格式，必须先解包成单个 img 再按 GMS key 导入；大容器超 orange-wz 读路径能力。
- 环境探查：WzComparerR2 目录只有 dll/依赖、无可执行 CLI；orange-wz 有 `WzFile/WzDirectory` 类但大文件读失败。
- **未导入、未伪造成功**。下一步选项（需用户定）：A 找/装可用的 wz 解包工具（HaRepacker/WzComparerR2 CLI）先解包再导；B 放弃这批 wz 整包、只取其中需要的道具 img（需用户列出 itemId）；C 这批暂不导。

## 真机验收（待用户，如实声明未验收）
重启客户端：侧栏应多出 3 个图标（签到/伤害统计/染色）；卡片道具 tooltip 名称应完整（服务端 Ins 合并后需重启服务端生效）；名片错位与动漫鞋外观看真机。截图须本端。

## 回滚点
- NameTag：staging `NameTag.live.bak`
- side_icons.ini：live 目录 `.bak-before-exticons-20261120`
- Ins.img.xml：两个文件各自 `.bak-cardname-20261120`
