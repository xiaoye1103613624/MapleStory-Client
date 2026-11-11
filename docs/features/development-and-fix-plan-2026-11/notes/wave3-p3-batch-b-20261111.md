# round98 · P3 B 批 5 条三向对拍（零源码改动）

## 1. 本轮范围

`P3-ANGEL-REBUILD-PLAN.md:39-43` 的 B 批 5 条：`skill/D9`、`social-economy/D12`、`pet-summon/D2`、`login-flow/D13`、`social-economy/D9`。

## 2. 取证口径

每条必须凑齐四项才落判定：

1. **083 活 IDB**（endpoint `http://127.0.0.1:13339/mcp`）：`entity_query` 取符号族 → `decompile` 取正文，落 `tools/_ida_r98/`。
2. **orange-wz 128B 门禁**：`load_files`（`tools/_gms_key_128.b64`，172 字符，iv `TSPHKw==`，带 `MCP-Protocol-Version: 2025-03-26` 头）→ `search`/`list_children` → `unload_all`；成功判定看 `loadedCount` 而非 `isError`。
3. **s9 服务端源码**（唯一副本 `E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/`）。
4. **当前端字面量 + DECISIONS 原裁定**。

## 3. 五条结论

| 决策点 | 判定 | 一句话 |
| --- | --- | --- |
| `skill/D9` | `KEEP` | 083 单一布局，`Skill` 无 `main` → classic 恒 true = 083 路径；双布局是资源降级（E13） |
| `social-economy/D12` | `P2` | 实为 27 键（非 26）；083 仅 4 组 SP_，须逐键对照 `String.img` |
| `pet-summon/D2` | `LOCAL_TYPE` | wire 同宽 8B；截断在本端 int32 三处；发包 int32 与 s9 一致不能改 |
| `login-flow/D13` | `DOC_ONLY` | 槽数=尾部 int32 容量值，两端均按 charcount 循环 |
| `social-economy/D9` | `三分` | FriendGroup 不做 / guildmark 有节点无发包 / MapleTV 壳层已对齐 |

## 4. 本轮发现的两处「Plan 措辞与事实不符」

1. `social-economy/D12` Plan/DECISIONS 写「26 键」，现场实数 **27 键**。
2. `login-flow/D13` Plan 写「改则只改本地槽数组」，但两端都没有槽数组定长结构（槽数只是尾部 int32），前提不成立。

两处只补事实层，不改算术、不注销条目。

## 5. 验证与边界

- 静态守卫 + 场景：`tools/test_round98_p3_batch_b.py`（全绿）
- `python tools/gen_opcode_matrix.py --check` → `GATE OK … (s9 a78ba681a4)`
- 零源码改动 → 只做构建门禁回归，**不声称真重编覆盖**
- 零署名提交；产物不含任何 commit hash
