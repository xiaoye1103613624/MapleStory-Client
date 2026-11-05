# 构建 / 验证产物索引（2026-11-04）

## 客户端

- 命令：`tools/build_smoke.bat`（先 `taskkill /F /IM MapleStory.exe`）。
- 前置门禁：`python tools/gen_opcode_matrix.py` 重生成矩阵 → `--check` 输出 `GATE OK: opcode matrix matches both sources (s9 44d0d5d9ca)`；`notes/opcode-diff.md` 已补复核记录。
- 结果：`GATE OK` / `EXITCODE=0`，构建日志 `build_log.txt` 无 `error C` / `error LNK` / `fatal error`。
- 产物：`E:/MXD/BeiDou-Client-xy/MapleStory.exe`，2026-11-04 20:05，4,679,168 字节（OutDir 即部署目录，未做 robocopy）。
- 摘要：`../evidence/client_build_tail.txt`。

## s9 服务端

- 命令：`JAVA_HOME=C:/Users/11036/.jdks/ms-21.0.12.1`，`D:/software/apache-maven-3.9.16/bin/mvn.cmd -o -f pom.xml -pl gms-server -am compile -DskipTests`。
- 为确保不是增量缓存误判，刷新源码时间戳后强制全量重编译：`Compiling 1128 source files with javac [debug target 21] to target\classes`。
- 结果：`BUILD SUCCESS`，Reactor `BeiDou` / `gms-server` 均 SUCCESS。
- **注意**：只刷新了 `target/classes`，**未重新打包 `BeiDou.jar`**；真机启用前需用户用 Java 21 + MySQL 重新 package 并启动服务。
- 摘要：`../evidence/s9_maven_compile.txt`。

## 自动化对拍

| 脚本 | 命令 | 结果 |
| --- | --- | --- |
| `tools/attack_layout_check.py` | 默认 | legacy 4B/行 13/13 `ALL LAYOUTS ALIGN` |
| 同上 | `--long` | 12B/行 9/9 `ALL LAYOUTS ALIGN` |
| 同上 | `--legacy` | 修复前反向对照 9/13 FAIL（预期） |
| `tools/damage_formula_check.py` | `--out ...` | `ALL DAMAGE CHECKS ALIGN：0 项异常` |

归档：`../evidence/attack_layout_legacy.txt`、`attack_layout_long.txt`、`attack_layout_reverse.txt`、`damage_formula_check.txt`。

## 真机联调（未闭环）

- 检查时 `netstat -ano | findstr ":8484"` 无监听 → s9 未启动。
- 阻塞原因：需要 Java 21 + MySQL，且必须双端同时把 `long_lines` 打开、服务端重新打包重启。
- 待用户执行 `README.md` §5 的启用步骤后补：登录抓包（`debug.packet_dump`）、超 INT_MAX 伤害样例、旁观者同值截图、Boss 血条不回绕检查。

## 关键补丁

- `../patches/client.patch`：客户端 26 个已跟踪文件的 diff。
- `../patches/s9.patch`：s9 服务端 12 个已跟踪文件的 diff。
- 新增文件不在 git diff 内：`Gameplay/Combat/Damage.h`、`tools/damage_formula_check.py`、s9 `org/gms/config/DamageLinesConfig.java`。
