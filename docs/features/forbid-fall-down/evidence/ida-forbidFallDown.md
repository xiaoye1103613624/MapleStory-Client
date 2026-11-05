# IDA 取证：`forbidFallDown`（BeiDou.exe / V17.7）

> IDB：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`  
> 解密串池：`docs/features/native-map-object-re/evidence/beidou_v177_decrypted_strings.tsv`

## 1. 字符串

| string id | VA（密文槽） | 明文 |
| --- | --- | --- |
| **4417** (`0x1141`) | `0xB0D644` | `forbidFallDown:` |
| 2972 | `0xB2232C` | `force` |
| 1911 (`0x777`) | — | `prev` |
| 1912 (`0x778`) | — | `next` |

明文串在 IDA `find_regex` 不可见（运行时解密）；以 `push 1141h` 即时数定位消费点。

## 2. 加载点

- 函数：`sub_A43E7B` @ `0xA43E7B`（4997 B）— foothold / portal 属性装载族
- 唯一即时数命中：`0xA4449F` `push 1141h`
- 反编译片段（同函数内紧邻 `force=2972` 之后）：

```c
v133 = 4417;                         // "forbidFallDown:"
sub_79E805(...); sub_406292(&v134, v133);
...
v48 = sub_414D40(v47, ...) != 0;     // 缺键/0 → false；非 0 → true
...
v189 = (void *)v48;
v191 = sub_4A27D8(&v189, 4, 0, 0, 0);
*(this + 44) ^= ... ^ v191 ^ ...;    // 写入 foothold 对象混淆槽
```

结论：**原生把该键读成 foothold 布尔成员**，与 `force` 同属落脚点叶子属性。

## 3. 行为语义（对照实现，非想象）

本 IDB 未再出现第二处 `4417` 即时数（消费走对象字段，XOR 混淆难直接 xref）。行为对齐开源对照：

| 来源 | 行为 |
| --- | --- |
| `sdlMS` `character_logic_system::run_jump`（prone） | `if (!c_fh.forbidFallDown)` 才执行下跳穿板 |
| `SoloMapling` `BotPhysicsEngine::canStartDownJump` | `!foothold.isForbidFallDown()` |

与本端既有 `PhysicsObject::enablejd`（下方 600px 内有板才允许 ↓+跳）合流：  
**当前 fh 带 `forbidFallDown` → `enablejd = false`**。

## 4. 局限

- 未在本轮反汇编出「下跳 grace 内 forbid 板仍固体」的独立分支；Journey 下跳是 `y = groundbelow` 瞬移 1px，无 grace 穿板窗口，无需额外落地特例。
- 服务端 BeiDou-Server_s9 无 `forbidFallDown` Java 引用 → **纯客户端闭合**。
