//////////////////////////////////////////////////////////////////////////////////
//	SetItemData — 套装花名册（2026-11-12 装备槽位扩展 ⑤）
//
//	数据源：Data/SetItem.txt（极简行格式，零新依赖）。
//	为什么不是 WZ SetItemInfo.img：live Etc/ 目录实测无该节点（MCP 取证），
//	且 s9 服务端无 xy 分支的 combat.stat 框架；本端按「本地花名册 + 已穿戴
//	件数」独立结算，参考 E:\MXD\扩展改动\套装系统\README.md 的数据契约
//	（setItemName / completeCount / ItemID / Effect/{n} 档位）。
//
//	行格式（# 注释，空行忽略）：
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

		/// 从 Data/SetItem.txt 加载；文件缺失时保持空表（优雅降级，不崩）。
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
