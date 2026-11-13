# 保留项 R1–R4（非字段类型，但不得按「纯 Angel」拆掉）

| ID | 内容 | 权威文档 / 锚点 |
|---|---|---|
| R1 | 多分辨率：登录恒 1280×720；进图 9 档；游戏内即时生效 | `docs/features/multi-resolution/README.md` D1–D4 |
| R2 | i18n + 运行时切语言 | `locale/zh-CN.ini` / `en-US.ini`；`Util/Locale` |
| R3 | IME / 中文输入 | `IO/Window.cpp` `SDL_StartTextInput`；`Textfield` unicode |
| R4 | s9 协议对齐 + meso long + level/stat 放宽 | `docs/features/s9-protocol-align/`；level-cap-999；meso commit |

与 D 矩阵 T1 一致：**s9 联机优先，Angel 仅语义校准源**。
