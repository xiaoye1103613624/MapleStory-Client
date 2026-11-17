//////////////////////////////////////////////////////////////////////////////////
//	SetItemData — 套装花名册（2026-11-12 装备槽位扩展 ⑤）
//
//	数据源（2026-11-17 用户裁决升级）：
//	  ① 主源 = live WZ Etc/SetItemInfo.img（真实存在，orange-wz MCP 取证；
//	     233KB / 625 个 set 节点）——load_from_wz() 运行时经 nlnx 读取，
//	     setItemName 由 nlnx 后端按 GBK→UTF-8 解析为中文。
//	  ② 兜底 = Data/SetItem.txt（load()），仅当 WZ 根不可用时降级。
//	只映射本端 EquipStat 支持属性子集的 Effect 键，其余（activeSkill/Option/
//	incMHPr/incMMPr/nbdR 等）按用户裁决跳过并在可行性文档标注。
//	加成权威仍在客户端本地结算（Player::apply_set_item_bonuses），不做协议。
//
//	txt 行格式（# 注释，空行忽略）：
//	  SET <setId> <name> <completeCount>
//	  ITEM <itemId>                  → 归属最近的 SET
//	  TIER <count> <statKey> <value>  → 达到 count 件时生效；可重复
//	statKey 取值 = EquipStat 名（STR/DEX/INT/LUK/WATK/MAGIC/WDEF/MDEF/HP/MP/
//	              ACC/AVOID/HANDS/SPEED/JUMP）或 ALLSTAT / DAMAGE / BOSS / IGNOREDEF。
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Character/EquipStat.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ms
{
	class SetItemData
	{
	public:
		struct Tier
		{
			int16_t count;
			std::string stat_key;
			int32_t value;
		};

		struct Definition
		{
			int32_t set_id;
			std::string name;
			int16_t complete_count;
			std::vector<int32_t> item_ids;
			std::vector<Tier> tiers;
		};

		/// 从 live WZ Etc/SetItemInfo.img 加载（主源）。只映射本端支持的
		/// Effect 属性键，其余跳过。成功返回 true（至少 1 个 set）。
		static bool load_from_wz();

		/// 从 Data/SetItem.txt 加载（兜底）；文件缺失时保持空表（优雅降级）。
		static bool load(const std::string& data_dir);

		/// itemId → setId（0 = 不属于任何套装）。
		static int32_t set_of(int32_t item_id);

		/// 套装定义；未找到返回 nullptr。
		static const Definition* get(int32_t set_id);

		/// 统计某套装在「已穿戴 itemid 列表」中的件数（去重）。
		static int16_t count_equipped(int32_t set_id, const std::vector<int32_t>& equipped_item_ids);

		/// 达到 count 件时该档位的全部加成（供 Player::recalc_stats 注入）。
		static std::vector<Tier> active_tiers(int32_t set_id, int16_t equipped_count);

		static size_t definition_count();

	private:
		static std::unordered_map<int32_t, Definition> definitions;
		static std::unordered_map<int32_t, int32_t> item_to_set;
		static bool loaded;
	};
}
