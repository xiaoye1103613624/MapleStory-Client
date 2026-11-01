# r727 — Clock makeactive + WeddingPhoto ensure + BuffList cancel

## Ensure-open
- Dojo / Pyramid / SetQuestTime Clock：emplace 后补 `makeactive`（对齐 ContiMove）
- WeddingPhoto：缺 Wedding 则 emplace，再 `set_invitation`

## BuffList sync
- `Player::get_buff_skillid` + `UIBuffList::remove_buff`
- `CancelBuffHandler` 取消前取 skillid，刷掉图标

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
