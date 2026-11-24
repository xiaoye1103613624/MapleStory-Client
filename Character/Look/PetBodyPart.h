//////////////////////////////////////////////////////////////////////////////////
//	Pet equip body-part table (Angel 083 CUIPetEquip / CPet::UpdatePetAbility)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstddef>
#include <cstdint>

namespace ms
{
	// 宠物装备走 EQUIPPED 容器的官方 BodyPart 绝对值（wire 负槽取反后）。
	// 不并入 EquipSlot::Id，避免与角色装 21..48 空洞冲突（见 EquipSlot.h 不变量）。
	namespace PetBodyPart
	{
		// Angel CPet::UpdatePetAbility：页 0/1/2 → 区间 + 特例位。
		// 2026-11-23：TAB1 剔除 BP33 —— 角色口袋官方位（EquipSlot::POCKET / wire −33）；
		// cash −133（2 号宠拾取袋）仍由 is_pet_bodypart(133) 识别，不与口袋串槽。
		inline constexpr int16_t TAB0[] = { 21, 22, 23, 24, 25, 26, 27, 28, 29, 46 };
		inline constexpr int16_t TAB1[] = { 31, 32, 34, 35, 36, 37, 47 };
		inline constexpr int16_t TAB2[] = { 39, 40, 41, 42, 43, 44, 45, 48 };

		// Equip/pet canvas 局部格（D7 网格：列 step33、行 step33，格 32×32）。
		// 截图绝对列 185/218/251/284/317 − 面板 x172 → 13/46/79/112/145；
		// 行 134/167/200/233 − 面板 y123 → 11/44/77/110。
		struct Cell
		{
			int16_t x;
			int16_t y;
		};

		inline constexpr Cell CELLS[] = {
			{ 13, 11 }, { 46, 11 }, { 79, 11 }, { 112, 11 }, { 145, 11 },
			{ 13, 44 }, { 46, 44 }, { 79, 44 }, { 112, 44 }, { 145, 44 },
			{ 13, 77 }, { 46, 77 }, { 79, 77 }, { 112, 77 }, { 145, 77 },
			{ 13, 110 }, { 46, 110 }, { 79, 110 }, { 112, 110 }, { 145, 110 }
		};

		inline constexpr size_t CELL_COUNT = sizeof(CELLS) / sizeof(CELLS[0]);

		inline bool is_pet_bodypart(int16_t bp)
		{
			// 133 = cash 2 号宠拾取袋（S10）；勿与角色口袋 BP33 混淆。
			if (bp == 133) return true;
			if (bp >= 21 && bp <= 29) return true;
			// 31–37 含缺口：33 已划给角色 EquipSlot::POCKET。
			if (bp == 31 || bp == 32) return true;
			if (bp >= 34 && bp <= 37) return true;
			if (bp >= 39 && bp <= 45) return true;
			return bp == 46 || bp == 47 || bp == 48;
		}

		// 现金宠装镜像位（Angel GetItem a4:-100-v6 → 入库 key = bp+100）。
		inline int16_t cash_mirror(int16_t bp)
		{
			return static_cast<int16_t>(bp + 100);
		}

		inline const int16_t* tab_parts(uint8_t tab, size_t& out_count)
		{
			switch (tab)
			{
			case 0: out_count = sizeof(TAB0) / sizeof(TAB0[0]); return TAB0;
			case 1: out_count = sizeof(TAB1) / sizeof(TAB1[0]); return TAB1;
			case 2: out_count = sizeof(TAB2) / sizeof(TAB2[0]); return TAB2;
			default: out_count = 0; return nullptr;
			}
		}

		// 页内第 index 个 bodypart；越界返回 0。
		inline int16_t bodypart_at(uint8_t tab, size_t index)
		{
			size_t n = 0;
			const int16_t* parts = tab_parts(tab, n);
			if (!parts || index >= n)
				return 0;
			return parts[index];
		}
	}
}
