# r725 — Carnival marker silence + GameMenu tip

## Tip silence
- Carnival Summon / Died：UI ensure-open 并 `set_message` 后 **return**，不再叠 field marker

## Shell tip
- GameMenu tip 同步 SoftKey hub（`1-9` / Esc / X=face），不再写过时 `/=Map` 文案

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
