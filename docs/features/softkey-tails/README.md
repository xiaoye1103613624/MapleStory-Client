# SoftKey 旧波尾巴（计划穿插项）

## 状态（2026-11-05）

| 项 | 代码 | 实机 |
| --- | --- | --- |
| 登录遮罩叠图 | knobs 已锁（r201 playbook） | 需 `evidence/r20/current_shots/` 截图对照 |
| PersonalShop CREATE FM | SoftKey 店名 → `HiredMerchantCreatePacket`（514/503） | 需自由市场图持有许可证点测 |
| Messenger 立绘 | NameBar 脚点 + scale 0.70（r495） | 需实机截图再微调 |
| Guild BBS | SoftKey `B` 全包出站；s2c 写入公会窗 `[BBS]` 摘要栏 | 有公会后 `B`→List 验收多页签（GuildBBS.img 独立窗仍预留） |

## 本轮代码补强
- `UIGuild::set_bbs_summary`：列表/帖摘要与公告分栏绘制
- `GuildBbsHandler`：解析标题进摘要栏并激活公会窗；修正文拼接乱码

## 真机步骤

见 [pending-live-tests §3–6](../pending-live-tests/README.md)。
