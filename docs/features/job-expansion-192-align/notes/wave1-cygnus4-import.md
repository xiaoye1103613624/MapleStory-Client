# 波次 1 预备：骑士团四转填空

## 本端现状

`BeiDou-Client-xy` / `BeiDou-ClientV17.7` 的 `1112`–`1512`：

- 有 `info/icon`
- `skill` **空**（V17.7 散 img 仅 ~414B）

## 源

- **优先**：`MapleServer192\wz\Skill.wz\{1112,1212,1312,1412,1512}.img.xml`（已有实质节点）
- **位图**：192 客户端 `Data\Skill\Skill_*.wz`（LATEST IV）；需定位含 `1112.img` 的分卷后 `copy_paste_nodes`
- **否**：20dalu 占用方案；V17.7 空壳无内容可抄

## 阻塞 / 设计点（填空前）

1. 本端运行时读 **Skill.nx**（PKG4），不是散 img → 需 XML/WZ → img → 合入 `.nxcache/Skill.nx` 或重建 nx 流水线  
2. 192 四转技能 ID 可能带高版本字段；导入后 `SkillData.cpp` 需能容忍未知子节点  
3. s9 `isCygnus()?120:200` 等级上限 → **仅填资源不够**，四转职业玩法还要服务端闸门（文档预留）  
4. String 名（希纳斯骑士等）需同步 `String.nx` / `String/Skill.img`

## 下一动作

1. 用 orange-wz + LATEST IV 定位并抽出 `1112.img` 整树到 staging  
2. 与本端 GMS 密钥目标格式对齐（`batch_convert_key` 如需）  
3. 合入 Skill.nx / 部署包原子替换  
4. 抽检：`11121000` 名+icon；确认 `11111006` 仍为「灵魂突刺」  
