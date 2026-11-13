# build_smoke · 2026-11-12

首轮：`GATE OK` 但 `EXITCODE=1`（既有 `UIAddonEquip.cpp` 缺 `UIItemInventory.h`，与本面无关，补 include 解锁）。

复跑：

```
GATE OK: opcode matrix matches both sources (s9 a0fa1926b6)
GATE OK
EXITCODE=0
```

`tools/build_smoke.bat` 通过。

## 续作（IMG 接线后）

- 源码编译通过；链接 `LNK1104`：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` 被占用（客户端在跑）。
- 关客户端后再跑 `tools/build_smoke.bat` 即可链出。
