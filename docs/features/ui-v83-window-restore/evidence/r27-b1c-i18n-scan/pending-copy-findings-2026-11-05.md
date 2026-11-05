# pending-copy 取证结果（R27-B1c 续 · 2026-11-05）

> 通道：本机 `.nxcache`（String.nx / UI.nx），与 orange-wz 同源数据、同一 nx_inspect 工具链；根节点枚举为**全量非抽样**。
> 结论先说：台账里 ~44 条「待取证」的**取证路径在本包数据里不存在**——它们和 237 条「待用户定措辞」其实是同一桶：自造 UI，无 v83 原生文案可取。据此收口，避免未来会话重跑同样的探针。

## 一、String.nx 顶层全清单（20 根，权威）
`Cash.img Consume.img EULA.img Eqp.img Etc.img GLcloneC.img GuestEULA.img Ins.img Map.img Mob.img MonsterBook.img NameChange.img Npc.img Pet.img PetDialog.img Skill.img TestEULA.img ToolTipHelp.img TransferWorld.img TrialEULA.img`

**缺失**（台账设想的取证源）：`UIWindow.img`、`Trade.img`、`Warehouse.img/Trunk`、`Region`、`CharSelect`。
⇒ 凡"某文案可能在 String.img/UIWindow(或 Trade/Warehouse) 有现成词"的条目，**该源不存在**，取证失败→改判"无原生依据，自拟"。
（Etc.img 实为道具名、Mob.img 实为怪名 name、Map.img 是分区名如 MasteriaGL/china——都不是这些 UI 标签。）

## 二、UI.nx 烘焙位图证据（决定"文案 vs 贴图"）
| 取证点 | UI.nx 节点 | 子节点 | 判 |
| --- | --- | --- | --- |
| 版本号 `version.prefix` | `Login.img/Common` | 无 `version` 子节点（仅 frame/step/Bt*） | 非烘焙位图 ⇒ 现自绘 `Ver.{}` 成立，**键保留、值维持英文原样、行为不变** |
| 好友/组/黑名单空态 `userlist.none`、标题 `buddylist.title`/`party.title` | `UIWindow.img/UserList` | 全 backgrnd/Tab/子面板，无文字节点 | 窗体是烘焙美术 ⇒ "标题用不用文字"是**改渲染决策**（要贴烘焙字还是留自绘），非措辞 ⇒ 见下用户决策 |
| 仓库金币 `storage.meso` | `UIWindow.img/Trunk` | BtCoin/BtInCoin/BtOutCoin…，无"金币"文字节点 | 原生金币数是贴图/自绘混排 ⇒ 无现成词可取 ⇒ 自拟 |
| 交易 `trade.title/locked/waiting` | `UIWindow.img/TradingRoom` | 仅 Bt*/backgrnd/icon1 | 无文字源 ⇒ 自拟 |
| 手柄 `joypad.none` | UI.nx 无任何 joypad 根 | — | v83 无手柄窗 ⇒ 纯自造 ⇒ 自拟 |

## 三、收口动作（已执行 / 待用户）
- **已执行（纯文档，零行为变更）**：把上面"取证失败/烘焙位图"证据落到本文件；pending-copy.md 顶部加一行指针，标注这些条目"原生源不存在，维持现值（键在、值=英文字面量、行为不变）"，**不翻译、不编词**（门禁：无 String.img/IDA 依据不编条目）。
- **待用户决策（两类，都不是我能定的）**：
  1. **措辞类**（237+~30）：自造 UI 无原生文案，要中文化须你给措辞口径（如「金币/未选择地图/排行不可用」这些本包已有的中文，其余英文占位是否统一中译？给一份措辞表我可批量落 ini，代码不动）。
  2. **渲染决策类**：UserList/Party/Trade/Storage 标题——原生是烘焙位图，"要不要弃自绘 Text 改贴原生图"属版式还原（Wave 6 素材线），不是 i18n。

## 四、净结论
i18n pending-copy 在"取证 → 只改 ini 值"这条路上**已见底**：能靠取证的（少数）已判，剩下的要么是措辞决策（等用户），要么是渲染决策（Wave 6）。**本桶无可再自助推进的代码活**——继续硬推就是编词条，违反门禁。转下一项（真机冒烟自扫 / Wave 6 骰子）。
