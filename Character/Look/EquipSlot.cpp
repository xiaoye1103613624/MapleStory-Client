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
#include "EquipSlot.h"

#include "../../MapleStory.h"

#include <iostream>

namespace ms
{
	namespace EquipSlot
	{
		Id by_id(size_t id)
		{
			// by_id 收的是「枚举/数组下标」，不是 wire BodyPart。
			// 下行包里若拿到 -51 这类官方编号，必须先经 from_wire_bodypart() 转换。
			if (id >= Id::LENGTH)
			{
				LOG(LOG_DEBUG, "Unknown EquipSlot::Id id: [" << id << "]");

				return Id::NONE;
			}

			return static_cast<Id>(id);
		}

		int16_t wire_bodypart(Id slot)
		{
			// 枚举值与 wire 编号对齐（见 EquipSlot.h 的不变量说明），因此这里
			// 是「白名单 + 恒等」：只有真的能上线穿戴的槽位列进表，其余返回 0。
			// 不写 default: return (int16_t)slot 是为了让 21..48 宠物座位 BP、
			// 以及 NONE / BOOK / TOP_DEFAULT / BOTTOM_DEFAULT 这些非穿戴位
			// 不可能被误发成 -33 这类宠物位（用户「不与宠物装备冲突」的硬约束）。
			switch (slot)
			{
			case Id::HAT:
			case Id::FACE:
			case Id::EYEACC:
			case Id::EARACC:
			case Id::TOP:
			case Id::BOTTOM:
			case Id::SHOES:
			case Id::GLOVES:
			case Id::CAPE:
			case Id::SHIELD:	// == SUBWEAPON（双手武器与盾牌共用 -10，原生语义）
			case Id::WEAPON:
			case Id::RING1:
			case Id::RING2:
			case Id::RING3:
			case Id::RING4:
			case Id::RING5:		// ⑤⑥ 六戒 sidecar -52/-53
			case Id::RING6:
			case Id::PENDANT1:
			case Id::TAMEDMOB:
			case Id::SADDLE:
			case Id::SHOULDER:	// ② 护肩 -20
			case Id::POCKET:	// 口袋 -33（非 cash −133）
			case Id::MEDAL:
			case Id::BELT:
			case Id::PENDANT2:	// ③ 第二项链 -51
			case Id::BADGE:		// ④ 徽章 -54
			case Id::TOTEM1:	// ④ 图腾 -55..-57；TOTEM4 仅兼容存量
			case Id::TOTEM2:
			case Id::TOTEM3:
			case Id::TOTEM4:
			case Id::EMBLEM:		// ④ 纹章 -59
			case Id::ANDROID_SLOT:	// ④ 旧 Addon 机器人 -60（过渡）
			case Id::HEART:		// ④ 心脏 -61
			case Id::MYSTIC1:	// ⑤ 神秘徽章 -70..-75
			case Id::MYSTIC2:
			case Id::MYSTIC3:
			case Id::MYSTIC4:
			case Id::MYSTIC5:
			case Id::MYSTIC6:
			case Id::ANDROID1:	// ⑥ 原生 Android -1200..-1206
			case Id::ANDROID2:
			case Id::ANDROID3:
			case Id::ANDROID4:
			case Id::ANDROID5:
			case Id::ANDROID6:
			case Id::ANDROID7:
				return static_cast<int16_t>(slot);
			default:
				// NONE / BOOK / TOP_DEFAULT / BOTTOM_DEFAULT，以及宠物座位
				// （除已占用的 POCKET=33）与辅助武器等越界值：不上 wire。
				return 0;
			}
		}

		Id from_wire_bodypart(int16_t bodypart)
		{
			// 符号无关：调用点存在两套口径 ——
			//   InventoryHandlers.cpp:210/212/222/224 传 -mod.pos（绝对值，正）
			//   InventoryHandlers.cpp:184/188          传 mod.pos（wire 原值，负）
			// 旧实现只认正数，收到 -20 落 default → NONE，
			// 护肩/新槽图标就不刷新（本轮回退时发现的第二处暗雷）。
			if (bodypart < 0)
				bodypart = static_cast<int16_t>(-bodypart);

			// cash 镜像 -1xx / -15x 归一化到正常位。
			// 例外：133 = 2 号宠拾取袋（S10）；若减 100 会撞角色口袋 BP33 → 不归一，交 PetBodyPart。
			if (bodypart >= 101 && bodypart <= 199 && bodypart != 133)
				bodypart -= 100;

			// 原生 Android cash 镜像 1300–1306 → 1200–1206。
			if (bodypart >= 1300 && bodypart <= 1306)
				bodypart = static_cast<int16_t>(bodypart - 100);

			// 与 wire_bodypart 白名单互逆；133 / 62 / 其它宠装 BP → NONE。
			switch (bodypart)
			{
			case 1: return Id::HAT;
			case 2: return Id::FACE;
			case 3: return Id::EYEACC;
			case 4: return Id::EARACC;
			case 5: return Id::TOP;
			case 6: return Id::BOTTOM;
			case 7: return Id::SHOES;
			case 8: return Id::GLOVES;
			case 9: return Id::CAPE;
			case 10: return Id::SHIELD;	// 双手武器同值别名
			case 11: return Id::WEAPON;
			case 12: return Id::RING1;
			case 13: return Id::RING2;
			case 15: return Id::RING3;
			case 16: return Id::RING4;
			case 17: return Id::PENDANT1;
			case 18: return Id::TAMEDMOB;
			case 19: return Id::SADDLE;
			case 20: return Id::SHOULDER;
			case 33: return Id::POCKET;
			case 49: return Id::MEDAL;
			case 50: return Id::BELT;
			case 51: return Id::PENDANT2;
			case 52: return Id::RING5;
			case 53: return Id::RING6;
			case 54: return Id::BADGE;
			case 55: return Id::TOTEM1;
			case 56: return Id::TOTEM2;
			case 57: return Id::TOTEM3;
			case 58: return Id::TOTEM4;
			case 59: return Id::EMBLEM;
			case 60: return Id::ANDROID_SLOT;
			case 61: return Id::HEART;
			case 70: return Id::MYSTIC1;
			case 71: return Id::MYSTIC2;
			case 72: return Id::MYSTIC3;
			case 73: return Id::MYSTIC4;
			case 74: return Id::MYSTIC5;
			case 75: return Id::MYSTIC6;
			case 1200: return Id::ANDROID1;
			case 1201: return Id::ANDROID2;
			case 1202: return Id::ANDROID3;
			case 1203: return Id::ANDROID4;
			case 1204: return Id::ANDROID5;
			case 1205: return Id::ANDROID6;
			case 1206: return Id::ANDROID7;
			default: return Id::NONE;
			}
		}

		Id from_islot(const std::string& islot)
		{
			// islot → 槽位。表项与 v083 WZ 的 info/islot 实际取值一一对应
			// （实测样本：Cp/Af/Ay/Ae/Ma/MaPn/Pn/So/Gl/Gv/Sr/Si/Wp/Ri/Pe/Tm/Sd/
			//  Sh/Ba/To/Em/Dr/Ht/Me/Be）。
			//
			// 刻意不映射：
			//   · 任何宠物 islot（180..183 / 1812000..1812007）→ NONE，
			//     宠物装备走 InventoryType::PETEQ，不得进角色 EQUIPPED（用户硬约束）；
			//   · Hd（帽子历史别名，本端帽子用 Cp）→ NONE，回退号段兜底；
			//   · 未列出的新值 → NONE，回退号段兜底（不猜）。
			if (islot.empty())
				return Id::NONE;

			if (islot == "Cp" || islot == "HrCp")	return Id::HAT;
			if (islot == "Af")	return Id::FACE;
			if (islot == "Ay")	return Id::EYEACC;
			if (islot == "Ae")	return Id::EARACC;
			if (islot == "Ma" || islot == "MaPn")	return Id::TOP;
			if (islot == "Pn")	return Id::BOTTOM;
			if (islot == "So")	return Id::SHOES;
			if (islot == "Gl" || islot == "GlGw" || islot == "Gv")	return Id::GLOVES;
			if (islot == "Sr")	return Id::CAPE;
			if (islot == "Si")	return Id::SHIELD;
			if (islot == "Wp" || islot == "WpSi" || islot == "WpSp")	return Id::WEAPON;
			if (islot == "Ri")	return Id::RING1;
			if (islot == "Pe")	return Id::PENDANT1;
			if (islot == "Tm")	return Id::TAMEDMOB;
			if (islot == "Sd")	return Id::SADDLE;
			if (islot == "Sh")	return Id::SHOULDER;
			if (islot == "Po")	return Id::POCKET;
			if (islot == "Me")	return Id::MEDAL;
			if (islot == "Be")	return Id::BELT;
			if (islot == "Ba")	return Id::BADGE;
			if (islot == "To")	return Id::TOTEM1;
			if (islot == "Em")	return Id::EMBLEM;
			if (islot == "Dr")	return Id::ANDROID1;	// 原生首槽 1200（旧 Addon 60 仅兼容下行）
			if (islot == "Ht")	return Id::HEART;
			if (islot == "Mb")	return Id::MYSTIC1;

			return Id::NONE;
		}

		const char* type_name(Id slot)
		{
			// 与 EquipData::type 的既有字符串保持同一套写法
			// （tooltip 用 Locale::enum_text("equip.type.", type) 取词条）。
			switch (slot)
			{
			case Id::HAT: return "HAT";
			case Id::FACE: return "FACE ACCESSORY";
			case Id::EYEACC: return "EYE ACCESSORY";
			case Id::EARACC: return "EARRINGS";
			case Id::TOP: return "TOP";
			case Id::BOTTOM: return "BOTTOM";
			case Id::SHOES: return "SHOES";
			case Id::GLOVES: return "GLOVES";
			case Id::CAPE: return "CAPE";
			case Id::SHIELD: return "SHIELD";
			case Id::WEAPON: return "ONE-HANDED SWORD";
			case Id::RING1:
			case Id::RING2:
			case Id::RING3:
			case Id::RING4:
			case Id::RING5:
			case Id::RING6: return "RING";
			case Id::PENDANT1:
			case Id::PENDANT2: return "PENDANT";
			case Id::TAMEDMOB: return "TAMING";
			case Id::SADDLE: return "SADDLE";
			case Id::SHOULDER: return "SHOULDER";
			case Id::POCKET: return "POCKET";
			case Id::MEDAL: return "MEDAL";
			case Id::BELT: return "BELT";
			case Id::BADGE: return "BADGE";
			case Id::TOTEM1:
			case Id::TOTEM2:
			case Id::TOTEM3:
			case Id::TOTEM4: return "TOTEM";
			case Id::EMBLEM: return "EMBLEM";
			case Id::ANDROID_SLOT:
			case Id::ANDROID1:
			case Id::ANDROID2:
			case Id::ANDROID3:
			case Id::ANDROID4:
			case Id::ANDROID5:
			case Id::ANDROID6:
			case Id::ANDROID7: return "ANDROID";
			case Id::HEART: return "HEART";
			case Id::MYSTIC1:
			case Id::MYSTIC2:
			case Id::MYSTIC3:
			case Id::MYSTIC4:
			case Id::MYSTIC5:
			case Id::MYSTIC6: return "MYSTIC";
			default: return "CASH";
			}
		}

	}
}
