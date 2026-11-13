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
#pragma once

#include "../../Template/Enumeration.h"
#include "../../Template/EnumMap.h"

#include <string>

namespace ms
{
	namespace EquipSlot
	{
		// ⚠️ 不变量（2026-11-12 回退并锁死）：**枚举值 == 官方 BodyPart 绝对值**。
		// 依据链（逐字读原文，不是推测）：
		//   · Net/Handlers/Helpers/ItemParser.cpp:110-113 —— 下行负 slot 取反后
		//     作为 EQUIPPED 容器 key 入库（key 就是 wire 绝对值）；
		//   · Net/Handlers/InventoryHandlers.cpp:182/204/214 —— 把 -mod.pos / -mod.arg
		//     （wire 绝对值）直接喂给 UIEquipInventory::modify 与 change_equip；
		//   · Character/Player.cpp:229-232 —— get_item_id(EQUIPPED, slot) 与
		//     by_id(slot) 都用同一个数当 key；
		//   · Character/Inventory/Inventory.cpp:270-273 has_equipped() 直接
		//     inventories[EQUIPPED].count(slot)，find_equipslot() 用
		//     has_equipped(RING2/RING3/RING4) 判戒指空位。
		// 所以枚举值必须等于 wire 编号。曾把它改成连续索引（MEDAL=20 而容器
		// key=49），会让**原始装备**（MEDAL/BELT/RING3/RING4/TAMEDMOB/SADDLE）的
		// 图标显示、卸下、戒指空位判断整体回归 —— 这正是用户 2026-11-12
		// 「不影响原始装备的逻辑」所指向的事故。
		//
		// 中间的空洞（21..48）是设计的一部分，不是遗留错误：
		//   · EnumMap<K,V> / Enumeration<E> 按 0..LENGTH-1 建数组
		//     （Template/EnumMap.h:35-40、Template/Enumeration.h:35），空洞只多占
		//     几个空数组项；iconpositions 为 (0,0) 时 UIEquipInventory.cpp:544-545
		//     跳过，update_slot 查不到 itemid 即留空。
		//   · 21..48 恰是 v083 的宠物座位 BP（s9 EquipSlot.java 的
		//     PET_BODY_PARTS）。本端**从不给它们分配角色装备槽**，宠物装备走
		//     InventoryType::PETEQ + Equip/pet 面板，与角色 EQUIPPED 容器天然分账。
		//     这条正是用户「新增装备不要和宠物装备冲突」的落实点。
		enum Id : int16_t
		{
			NONE = 0,
			HAT = 1,			// 100xxxx  Cp
			FACE = 2,			// 101xxxx  Af
			EYEACC = 3,			// 102xxxx  Ay
			EARACC = 4,			// 103xxxx  Ae
			TOP = 5,			// 104/105xxxx  Ma/MaPn
			BOTTOM = 6,			// 106xxxx  Pn
			SHOES = 7,			// 107xxxx  So
			GLOVES = 8,			// 108xxxx  Gl/Gv
			CAPE = 9,			// 110xxxx  Sr
			SHIELD = 10,		// 109xxxx  Si
			// 双手武器与盾牌共用 (137,131) 格（UIEquipInventory.cpp:559 原生语义）。
			// 做成**同值别名**而不是独立 BP：v083 里 -10 就是 shield/secondary 位；
			// 旧值 58 是 Journey 的 TODO 占位（服务端从不发 -58），且本轮 58 已
			// 正式归属 TOTEM4，绝不能让两个语义抢同一个 key。
			// 绝不用 -62 —— 134/135 是 live 里 111 个 .img 的原始武器号段。
			SUBWEAPON = SHIELD,
			WEAPON = 11,		// 13x/14x/1x 武器  Wp/WpSi/WpSp
			RING1 = 12,			// 111xxxx  Ri
			RING2 = 13,			// 111xxxx  Ri
			RING3 = 15,			// ① 右上第 3 戒（111xxxx Ri）
			RING4 = 16,			// ① 右上第 4 戒（111xxxx Ri）
			PENDANT1 = 17,		// 112xxxx  Pe
			TAMEDMOB = 18,		// 190xxxx  Tm
			SADDLE = 19,		// 191xxxx  Sd
			SHOULDER = 20,		// ② 护肩 115xxxx Sh（参考文档与 S10 同为 -20）
			MEDAL = 49,			// 114xxxx  Me
			BELT = 50,			// 113xxxx  Be
			PENDANT2 = 51,		// ③ 第二项链（v083 原生 BP51，112xxxx Pe）
			// 不引入 POCKET(-33)：-33 / cash -133 归一化后是 BP33，落在
			// PET_BODY_PARTS 宠物座位里（S10 原文注 -133 = 2 号宠拾取袋），
			// 会让口袋装备与宠物装备互相冒名。口袋也不在用户 7 项内。
			// 不引入六戒 -52/-53：用户 ① 只要右上 2 格，且这两个位是 sidecar。
			BADGE = 54,			// ④ 徽章 Ba
			TOTEM1 = 55,		// ④ 图腾 To
			TOTEM2 = 56,
			TOTEM3 = 57,
			TOTEM4 = 58,
			EMBLEM = 59,		// ④ 纹章 119xxxx Em
			ANDROID_SLOT = 60,	// ④ 机器人 166xxxx Dr
			HEART = 61,			// ④ 心脏（用户文案「新增」）167xxxx Ht
			// 以下三项不是官方穿戴位，只在本端做占位/外观键，wire_bodypart 返回 0。
			// 必须排在 61 之后，避免与上面的官方编号抢 key（旧值 52/60/61 会
			// 撞进 sidecar 位与机器人/心脏格，导致图标串台）。
			BOOK = 62,
			TOP_DEFAULT = 63,
			BOTTOM_DEFAULT = 64,
			LENGTH = 65
		};

		Id by_id(size_t id);

		// 逻辑槽位 → wire BodyPart 绝对值（穿戴包发送 -value）。
		// 枚举值与 wire 对齐后这是恒等映射；保留函数的意义是：
		//   1) 让 NONE / BOOK / TOP_DEFAULT / BOTTOM_DEFAULT 显式返回 0（不上线）；
		//   2) 把「枚举值 == wire 编号」这条不变量集中一处，可被对账脚本机器校验。
		int16_t wire_bodypart(Id slot);

		// wire BodyPart 绝对值 → 逻辑槽位（下行包解析用）。
		// cash 镜像 -1xx / -15x 归一化后按正常位取；未识别返回 NONE。
		// 比 by_id() 更安全：宠物座位 BP(21..48)、六戒 sidecar(52/53)、
		// 辅助武器(62) 都不在本表，收到即 NONE，不会被当成角色装备槽。
		Id from_wire_bodypart(int16_t bodypart);

		// WZ `info/islot` 字符串 → 逻辑槽位（2026-11-12 Round 7 新增）。
		// 为什么需要它：号段与槽位**不是 1:1** —— 实测 119xxxx 里混有
		// islot=Si（盾/副武器，41 件）、120xxxx 里混有 islot=Hd（帽类）。
		// 按号段一刀切会把这些装备判错槽（穿不上/穿错位），
		// 所以 EquipData 改为「islot 优先、号段兜底」。
		// 未识别返回 NONE（调用方回退到号段映射）。
		Id from_islot(const std::string& islot);

		// 槽位 → 装备类型名（EquipData::type 字段用，与 islot 表同源）。
		const char* type_name(Id slot);

		constexpr Enumeration<Id> values;
	};
}
