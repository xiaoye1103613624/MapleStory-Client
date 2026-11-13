# r05 · P2 实现纪要（2026-11-12）

## IDA（Angel 13339）

- `CUtilDlgEx::Draw` @0x9a4256 → `SetButtonBlink(idx,1)`，idx∈[0,3]
- `CNoticeQuestProgress::{OnQuestProgressUpdated_Item/Mob/Script,NoticeProgressChange}`
- `UpdateCheckList` @0x867be6 只同步 LeftTab 亮灭；分类来自卡表字段（本包无 Etc）

## WZ / 资源

- `Character/Accessory/0114*.img` 153 个 → ItemData 可出 icon
- s9 BuddyList 无 Ignore → 拒绝列表阻塞

## 落地与降级

见 [P2.md](../P2.md)。
