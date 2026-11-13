//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "EquipData.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	EquipData::EquipData(int32_t id) : itemdata(ItemData::get(id))
	{
		std::string strid = "0" + std::to_string(id);
		std::string category = itemdata.get_category();
		nl::node src = nl::nx::Character[category][strid + ".img"]["info"];

		cash = src["cash"].get_bool();
		tradeblock = src["tradeBlock"].get_bool();
		slots = src["tuc"].get_integer();

		reqstats[MapleStat::Id::LEVEL] = src["reqLevel"].get_integer();
		reqstats[MapleStat::Id::JOB] = src["reqJob"].get_integer();
		reqstats[MapleStat::Id::STR] = src["reqSTR"].get_integer();
		reqstats[MapleStat::Id::DEX] = src["reqDEX"].get_integer();
		reqstats[MapleStat::Id::INT] = src["reqINT"].get_integer();
		reqstats[MapleStat::Id::LUK] = src["reqLUK"].get_integer();
		defstats[EquipStat::Id::STR] = src["incSTR"].get_integer();
		defstats[EquipStat::Id::DEX] = src["incDEX"].get_integer();
		defstats[EquipStat::Id::INT] = src["incINT"].get_integer();
		defstats[EquipStat::Id::LUK] = src["incLUK"].get_integer();
		defstats[EquipStat::Id::WATK] = src["incPAD"].get_integer();
		defstats[EquipStat::Id::WDEF] = src["incPDD"].get_integer();
		defstats[EquipStat::Id::MAGIC] = src["incMAD"].get_integer();
		defstats[EquipStat::Id::MDEF] = src["incMDD"].get_integer();
		defstats[EquipStat::Id::HP] = src["incMHP"].get_integer();
		defstats[EquipStat::Id::MP] = src["incMMP"].get_integer();
		defstats[EquipStat::Id::ACC] = src["incACC"].get_integer();
		defstats[EquipStat::Id::AVOID] = src["incEVA"].get_integer();
		defstats[EquipStat::Id::HANDS] = src["incHANDS"].get_integer();
		defstats[EquipStat::Id::SPEED] = src["incSPEED"].get_integer();
		defstats[EquipStat::Id::JUMP] = src["incJUMP"].get_integer();

		constexpr size_t NON_WEAPON_TYPES = 15;
		constexpr size_t WEAPON_OFFSET = NON_WEAPON_TYPES + 15;
		constexpr size_t WEAPON_TYPES = 20;
		size_t index = (id / 10000) - 100;

		// 2026-11-12 装备槽位扩展：高版本装备号段 → 官方槽位。
		// 权威表 docs/features/equip-slot-expansion-2026-11/official-slot-mapping.md
		// + E:\project\BeiDou-Server_S10\...\ItemInformationProvider.java:597-610
		//   （getEquipmentSlot 的 prefix override）。
		// 注意：装备 ID 号段与槽位编号是两套体系，靠 islot 映射，不要求数值一致。
		// 旧实现只认 100..114，1152xxx（护肩）会掉进 else → eqslot=NONE，
		// 这是装备穿不上的客户端真根因。
		//
		// ⚠️ 用户 2026-11-12 追加约束「新增装备不要和宠物装备冲突、不影响原始装备逻辑」：
		//   · 这里 **只覆盖用户 7 项里的新增号段**（115 护肩 / 118 徽章 / 119 纹章 /
		//     120 图腾 / 166 机器人 / 167 心脏），且先在 live Character/*.img 实证
		//     这些前缀 0 命中（7329 个 .img 全量扫描），不会碰到原始装备。
		//   · 严禁把 134/135 当辅助武器：live 里 prefix 134 = 111 个 .img，是 v083
		//     原始武器号段（index 34 落在既有 WEAPON_OFFSET..+WEAPON_TYPES 分支），
		//     覆盖它会把 111 件原始武器全部误判成 SUBWEAPON。
		//   · 严禁加 116 口袋 / POCKET(-33)：-33 与 -133 归一化后是 BP33，而
		//     BP33 ∈ PET_BODY_PARTS(21..48) 是宠物座位，S10 原文注明
		//     -133 = 2 号宠拾取袋 → 会把口袋装备判成宠物装备。口袋也不在用户 7 项内。
		//   · 109 盾牌不走这里（既有 index 9 = SHIELD 分支已正确覆盖），避免动原始链路。
		const int32_t prefix = id / 10000;

		// Round 7：**islot 优先**。号段只做兜底。
		// 依据：实测 119xxxx 混有 islot=Si（盾/副武器）、120xxxx 混有 islot=Hd（帽），
		// 按号段一刀切会把它们判错槽（穿不上或穿错位）。
		// islot 是装备自身的权威字段，官方 getEquipmentSlot 也是先读它。
		if (const EquipSlot::Id islot_slot = EquipSlot::from_islot(src["islot"].get_string()); islot_slot != EquipSlot::Id::NONE)
		{
			eqslot = islot_slot;
			type = EquipSlot::type_name(islot_slot);
			return;
		}

		// ⚠️ Round 7 修正：号段 → 槽位**不是 1:1**。
		// 实测 live（非空素材抽样解析 islot）：
		//   119xxxx 里 41 个是 islot=Si（盾/副武器）、19 个是 islot=Em（纹章）
		//   120xxxx 里有少量 islot=Hd（帽子类），落在 Character/ 与 Weapon/
		// 所以不能按号段一刀切 —— 必须**先读 WZ islot**，号段只在 islot
		// 缺失/异常时兜底。这与官方口径一致（ItemInformationProvider
		// 先取 islot，S10 的 prefix override 只是纠错补丁）。
		//
		// 下面 switch 的注释保留各号段的「主力语义」，但只在 islot 不可用时生效。
		switch (prefix)
		{
		case 115:	// 护肩 → -20
			type = "SHOULDER";
			eqslot = EquipSlot::Id::SHOULDER;
			return;
		case 118:	// 徽章 → -54
			type = "BADGE";
			eqslot = EquipSlot::Id::BADGE;
			return;
		case 119:	// 纹章为主，但同号段混有 Si（盾/副武器）
			type = "EMBLEM";
			eqslot = EquipSlot::Id::EMBLEM;
			return;
		case 120:	// 图腾为主，但同号段混有 Hd（帽）
			type = "TOTEM";
			eqslot = EquipSlot::Id::TOTEM1;
			return;
		case 166:	// 机器人 → -60
			type = "ANDROID";
			eqslot = EquipSlot::Id::ANDROID_SLOT;
			return;
		case 167:	// 心脏 → -61
			type = "HEART";
			eqslot = EquipSlot::Id::HEART;
			return;
		default:
			break;
		}

		if (index < NON_WEAPON_TYPES)
		{
			constexpr const char* types[NON_WEAPON_TYPES] =
			{
				"HAT",
				"FACE ACCESSORY",
				"EYE ACCESSORY",
				"EARRINGS",
				"TOP",
				"OVERALL",
				"BOTTOM",
				"SHOES",
				"GLOVES",
				"SHIELD",
				"CAPE",
				"RING",
				"PENDANT",
				"BELT",
				"MEDAL"
			};

			constexpr EquipSlot::Id equipslots[NON_WEAPON_TYPES] =
			{
				EquipSlot::Id::HAT,
				EquipSlot::Id::FACE,
				EquipSlot::Id::EYEACC,
				EquipSlot::Id::EARACC,
				EquipSlot::Id::TOP,
				EquipSlot::Id::TOP,
				EquipSlot::Id::BOTTOM,
				EquipSlot::Id::SHOES,
				EquipSlot::Id::GLOVES,
				EquipSlot::Id::SHIELD,
				EquipSlot::Id::CAPE,
				EquipSlot::Id::RING1,
				EquipSlot::Id::PENDANT1,
				EquipSlot::Id::BELT,
				EquipSlot::Id::MEDAL
			};

			type = types[index];
			eqslot = equipslots[index];
		}
		else if (index >= WEAPON_OFFSET && index < WEAPON_OFFSET + WEAPON_TYPES)
		{
			constexpr const char* types[WEAPON_TYPES] =
			{
				"ONE-HANDED SWORD",
				"ONE-HANDED AXE",
				"ONE-HANDED MACE",
				"DAGGER",
				"", "", "",
				"WAND",
				"STAFF",
				"",
				"TWO-HANDED SWORD",
				"TWO-HANDED AXE",
				"TWO-HANDED MACE",
				"SPEAR",
				"POLEARM",
				"BOW",
				"CROSSBOW",
				"CLAW",
				"KNUCKLE",
				"GUN"
			};

			size_t weaponindex = index - WEAPON_OFFSET;
			type = types[weaponindex];
			eqslot = EquipSlot::Id::WEAPON;
		}
		else
		{
			type = "CASH";
			eqslot = EquipSlot::Id::NONE;
		}
	}

	bool EquipData::is_valid() const
	{
		return itemdata.is_valid();
	}

	EquipData::operator bool() const
	{
		return is_valid();
	}

	bool EquipData::is_weapon() const
	{
		return eqslot == EquipSlot::Id::WEAPON;
	}

	int16_t EquipData::get_reqstat(MapleStat::Id stat) const
	{
		return reqstats[stat];
	}

	int16_t EquipData::get_defstat(EquipStat::Id stat) const
	{
		return defstats[stat];
	}

	EquipSlot::Id EquipData::get_eqslot() const
	{
		return eqslot;
	}

	const std::string& EquipData::get_type() const
	{
		return type;
	}

	const ItemData& EquipData::get_itemdata() const
	{
		return itemdata;
	}
}