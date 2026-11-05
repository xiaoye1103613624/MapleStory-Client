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

#include "../Template/Point.h"

#include <vector>

namespace ms
{
	// A packet to be sent to the server
	// Used as a base class to create specific packets
	class OutPacket
	{
	public:
		// Construct a packet by writing its opcode
		OutPacket(int16_t opcode);

		void dispatch();

		// Opcodes for OutPackets associated with version 83 of the game
		enum Opcode : uint16_t
		{
			/// Login
			LOGIN = 1,
			LOGIN_EMAIL = 3,
			//（RELOG=0x1C 已裁决"明确不做"，不占枚举位：s9 RelogRequestHandler
			// 仅回 byte 1 无重登逻辑，发送只会触发本端垃圾 tip。分析见
			// docs/features/parity-gap-2026-11/README.md §1.2；此位维持原
			// 门禁不变量——OutPacket 枚举全部有构造点。）
			CHARLIST_REQUEST = 5,
			SERVERSTATUS_REQUEST = 6,
			ACCEPT_TOS = 7,
			SET_GENDER = 8,
			SERVERLIST_REQUEST = 11,
			SELECT_CHAR = 19,
			PLAYER_LOGIN = 20,
			NAME_CHAR = 21,
			CREATE_CHAR = 22,
			DELETE_CHAR = 23,
			PONG = 24,
			REGISTER_PIC = 29,
			SELECT_CHAR_PIC = 30,
			VIEW_ALL_CHAR = 13,
			/// s9 RecvOpcode.PICK_ALL_CHAR = 0x0E —— ViewAllChar 选角（相对
			/// CHAR_SELECT 多一个 world int；服务端读后丢弃，以 charId 定世界）。
			PICK_ALL_CHAR = 14,
			/// s9 RecvOpcode.VIEW_ALL_WITH_PIC = 0x20 —— VAC + PIC
			VIEW_ALL_WITH_PIC = 32,

			/// Gameplay 1
			CHANGEMAP = 38,
			/// s9 RecvOpcode.CHANGE_CHANNEL = 0x27
			CHANGE_CHANNEL = 39,
			ENTER_CASHSHOP = 40,
			MOVE_PLAYER = 41,
			CLOSE_ATTACK = 44,
			RANGED_ATTACK = 45,
			MAGIC_ATTACK = 46,
			/// s9 RecvOpcode.TOUCH_MONSTER_ATTACK = 0x2F：身体压力 / 能量条满
			/// 时触怪伤害；body 与 CLOSE_ATTACK 同形（parseDamage）。
			TOUCH_MONSTER_ATTACK = 47,
			TAKE_DAMAGE = 48,
			/// s9 RecvOpcode.CANCEL_CHAIR = 0x2A / USE_CHAIR = 0x2B
			CANCEL_CHAIR = 42,
			USE_CHAIR = 43,
			/// s9 RecvOpcode.CLOSE_CHALKBOARD = 0x32
			CLOSE_CHALKBOARD = 50,
			/// s9 RecvOpcode.FACE_EXPRESSION = 0x33
			FACE_EXPRESSION = 51,
			/// s9 RecvOpcode.USE_ITEMEFFECT = 0x34
			USE_ITEMEFFECT = 52,
			/// s9 RecvOpcode.USE_DEATHITEM = 0x35
			USE_DEATHITEM = 53,

			/// Messaging
			GENERAL_CHAT = 49,
			MULTI_CHAT = 119,
			/// s9 RecvOpcode.WHISPER = 0x78
			WHISPER = 120,
			SPOUSE_CHAT = 121,

			/// NPC Interaction
			TALK_TO_NPC = 58,
			NPC_TALK_MORE = 60,
			NPC_SHOP_ACTION = 61,

			/// Player Interaction
			CHAR_INFO_REQUEST = 97,

			/// Inventory
			GATHER_ITEMS = 69,
			SORT_ITEMS = 70,
			MOVE_ITEM = 71,
			USE_ITEM = 72,
			/// s9 RecvOpcode.USE_SUMMON_BAG = 0x4B；body 与 USE_ITEM 同形
			/// （readInt ts + short slot + int itemId，UseSummonBagHandler 坐实；
			/// 服务端扣 1 个后按 ItemData.mob 子表概率刷怪）。Wave 2a。
			USE_SUMMON_BAG = 75,
			/// s9 RecvOpcode.PET_FOOD = 0x4C
			PET_FOOD = 76,
			/// s9 RecvOpcode.USE_MOUNT_FOOD = 0x4D
			USE_MOUNT_FOOD = 77,
			/// s9 RecvOpcode.SCRIPTED_ITEM = 0x4E
			SCRIPTED_ITEM = 78,
			/// s9 RecvOpcode.USE_CATCH_ITEM = 0x51
			USE_CATCH_ITEM = 81,
			/// s9 RecvOpcode.CANCEL_ITEM_EFFECT = 0x49
			CANCEL_ITEM_EFFECT = 73,
			/// s9 RecvOpcode.USE_REMOTE = 0x74
			USE_REMOTE = 116,
			/// s9 RecvOpcode.GRENADE_EFFECT = 0x6D（Wave 4）：int x, int y,
			/// int keyDown, int skillId —— 手雷/毒弹投掷点上报，服务端回
			/// THROW_GRENADE(204) 广播落点。
			GRENADE_EFFECT = 109,
			USE_CASH_ITEM = 79,
			/// s9 RecvOpcode.SPAWN_PET = 0x62
			SPAWN_PET = 98,
			/// s9 RecvOpcode.USE_RETURN_SCROLL = 0x55（回城/传送卷）。
			/// 原生 CWvsContext::SendPortalScrollUseRequest（v95@0x9fca70）发本 opcode；
			/// body 与 USE_ITEM 同形（int ts / short slot / int itemid）。
			/// s9 PacketProcessor 将本 opcode 与 USE_ITEM 同挂 UseItemHandler，
			/// 其中 ItemConstants.isTownScroll(2030000..2030099) 走回城结算。
			USE_RETURN_SCROLL = 85,
			SCROLL_EQUIP = 86,

			/// Player
			SPEND_AP = 87,
			SPEND_SP = 90,
			/// s9 RecvOpcode.CANCEL_BUFF = 0x5C（body = int sourceid 技能id）
			CANCEL_BUFF = 92,
			/// s9 RecvOpcode.HEAL_OVER_TIME = 0x59：自然回血/回蓝上报。
			/// body = skip(8) + short healHP + short healMP（HealOvertimeHandler）。
			HEAL_OVER_TIME = 89,
			/// s9 RecvOpcode.CANCEL_DEBUFF = 0x63：临时减益到期通知。
			/// s9 CancelDebuffHandler 本体已注释（空实现）；本端仍按原生
			/// CheckTemporaryStatDuration 语义发空包，便于日后解禁或对照。
			CANCEL_DEBUFF = 99,
			/// s9 RecvOpcode.AUTO_DISTRIBUTE_AP = 0x58。标准分支
			/// （use_server_auto_assigner=false，DB 种子默认）body：
			/// 时间戳后 2×{int type,int val}，type 用 Stat.getBy5ByteEncoding
			/// （STR=64/DEX=128/INT=256/LUK=512）
			AUTO_DISTRIBUTE_AP = 88,
			/// s9 RecvOpcode.AUTO_AGGRO = 0xBD（Wave 4）：int oid —— 触怪碰撞
			/// 时上报该怪 oid，服务端 aggroAutoAggroUpdate 驱其进战斗状态。
			AUTO_AGGRO = 189,
			/// s9 RecvOpcode.USE_SKILL_BOOK = 0x52；body 与 USE_ITEM 完全同形
			/// （readInt 时间戳 + short slot + int itemId，SkillBookHandler.java 坐实）
			USE_SKILL_BOOK = 82,
			/// s9 RecvOpcode.MESO_DROP = 0x5E
			MESO_DROP = 94,
			/// s9 RecvOpcode.GIVE_FAME = 0x5F
			GIVE_FAME = 95,
			CHANGE_KEYMAP = 135,
			/// s9 RecvOpcode.USE_DOOR = 0x85
			USE_DOOR = 133,
			/// s9 RecvOpcode 地图特殊交互（W4）
			CHANGE_MAP_SPECIAL = 100,
			USE_INNER_PORTAL = 101,
			USE_ITEM_REWARD = 112,
			USE_TREASUER_CHEST = 115,
			TOUCHING_REACTOR = 206,

			/// Skill
			USE_SKILL = 91,
			SKILL_MACRO = 110,
			RPS_ACTION = 136,

			/// Quest (BeiDou RecvOpcode.QUEST_ACTION)
			QUEST_ACTION = 107,

			/// Gameplay 2
			/// Messenger 出站（s9 RecvOpcode.MESSENGER = 0x7A）；入站为 PacketSwitch 313
			MESSENGER = 122,
			PARTY_OPERATION = 124,
			ADMIN_COMMAND = 128,
			MOVE_MONSTER = 188,
			/// s9 RecvOpcode.CHANGE_QUICKSLOT = 0xB7（快捷栏 8 个触发键上行）。
			/// 存储/下行已闭环（Wave 1-1.2）；改键 UI 面需 v83 KeyConfig 快捷页
			/// 实体化（Wave 6 素材线），本枚举+包类先行登记，暂不接线。
			CHANGE_QUICKSLOT = 183,
			/// s9 RecvOpcode 宠物族上行（W1）
			MOVE_PET = 167,
			PET_CHAT = 168,
			PET_COMMAND = 169,
			PET_LOOT = 170,
			PET_AUTO_POT = 171,
			PET_EXCLUDE_ITEMS = 172,
			/// s9 RecvOpcode 召唤兽/龙上行（W2）
			MOVE_SUMMON = 175,
			SUMMON_ATTACK = 176,
			DAMAGE_SUMMON = 177,
			BEHOLDER = 178,
			MOVE_DRAGON = 181,
			PICKUP_ITEM = 202,
			DAMAGE_REACTOR = 205,
			PLAYER_MAP_TRANSFER = 207,
			/// s9 RecvOpcode.PARTY_SEARCH_START / UPDATE（自动匹配，非列表板）
			PARTY_SEARCH_START = 222,
			PARTY_SEARCH_UPDATE = 223,
			/// @deprecated 误名；实际为 PARTY_SEARCH_UPDATE(223)。保留别名以免旧引用崩编。
			PLAYER_UPDATE = PARTY_SEARCH_UPDATE,
			DENY_PARTY_REQUEST = 125,
			/// s9 RecvOpcode.DENY_GUILD_REQUEST = 0x7F
			DENY_GUILD_REQUEST = 127,
			/// F5-1：出站必须用 s9 RecvOpcode.STORAGE=0x3E=62（RecvOpcode.java:78 → StorageHandler）。
			/// 309 是 s9 **Send**Opcode.STORAGE 的值（服务端→客户端），抄错了方向，s9 侧收不到任何 handler。
			STORAGE = 62,
			/// 好友列表操作：必须用 s9 RecvOpcode.BUDDYLIST_MODIFY=0x82=130（RecvOpcode.java → BuddylistModifyHandler）。
			/// 63 是 continued-journey 模板的经典 v83 值，但 s9 把 63 号让给了 HIRED_MERCHANT_REQUEST（玩家商店），
			/// 好友 handler 已重映射到 130。原生 BeiDou.exe 连 s9 也必须用 130，故 63 是 opcode 赋值漂移（与同文件
			/// STORAGE/PLAYER_INTERACTION/CASHSHOP_OPERATION 三处同类修正一致）。字段结构已与 BuddylistModifyHandler 逐项对齐。
			BUDDYLIST = 130,
			/// s9 RecvOpcode：FREDRICK_ACTION=0x40 / DUEY_ACTION=0x41；GUILD 出站为 0x7E=126
			FREDRICK_ACTION = 64,
			/// s9 RecvOpcode.OWL_ACTION = 0x42 → UseOwlOfMinerva（空包）
			OWL_ACTION = 66,
			/// s9 RecvOpcode.OWL_WARP = 0x43 → OwlWarpHandler（ownerid + mapid）
			OWL_WARP = 67,
			/// s9 RecvOpcode.USE_HAMMER = 0x104 → ViciousHammer
			USE_HAMMER = 260,
			/// Family / Memo
			NOTE_ACTION = 131,
			OPEN_FAMILY_PEDIGREE = 145, // 0x91
			OPEN_FAMILY = 146,          // 0x92
			ADD_FAMILY = 147,           // 0x93
			/// s9 SEPARATE_FAMILY_BY_SENIOR = 0x94（带 junior cid）
			SEPARATE_FAMILY_BY_SENIOR = 148,
			/// s9 SEPARATE_FAMILY_BY_JUNIOR = 0x95（无 payload）
			SEPARATE_FAMILY_BY_JUNIOR = 149,
			ACCEPT_FAMILY = 150,        // 0x96
			USE_FAMILY = 151,           // 0x97
			CHANGE_FAMILY_MESSAGE = 152,// 0x98
			/// s9 RecvOpcode.FAMILY_SUMMON_RESPONSE = 0x99
			FAMILY_SUMMON_RESPONSE = 153,
			DUEY_ACTION = 65,
			GUILD_OPERATION = 126,
			/// F5-2：出站必须用 s9 RecvOpcode.PLAYER_INTERACTION=0x7B=123（RecvOpcode.java:131）。
			/// 314 同样是抄了 Send 侧值；本枚举里 6 个构造点（交易/雇佣商人/迷你游戏）此前全部进不了服务端。
			PLAYER_INTERACTION = 123,
			/// s9 RecvOpcode.USE_MAPLELIFE=0x100
			USE_MAPLELIFE = 256,
			/// s9 RecvOpcode.CHECK_CASH=0xE4 → TouchingCashShopHandler → showCash
			CHECK_CASH = 228,
			/// F5-3：出站必须用 s9 RecvOpcode.CASHSHOP_OPERATION=0xE5=229（RecvOpcode.java:199）。
			/// 325 是 Send 侧值（服务端→客户端），历史客户端误用；修正后商城请求才会进 CashShopOperationHandler。
			CASHSHOP_OPERATION = 229,
			/// s9 RecvOpcode.COUPON_CODE = 0xE6
			COUPON_CODE = 230,
			/// s9 RecvOpcode.TROCK_ADD_MAP = 0x66（缩地石收藏）
			TROCK_ADD_MAP = 102,
			/// s9 RecvOpcode.REPORT = 0x6A
			REPORT = 106,
			/// s9 RecvOpcode.NEW_YEAR_CARD_REQUEST = 0x9F
			NEW_YEAR_CARD_REQUEST = 159,
			MONSTER_BOOK_COVER = 57,
			/// s9 RecvOpcode.REMOTE_STORE = 0x3B → 远程仓库/雇佣店
			REMOTE_STORE = 59,
			MAKER_SKILL = 113,
			/// s9 RecvOpcode.WATER_OF_LIFE = 0x75
			WATER_OF_LIFE = 117,
			ALLIANCE_OPERATION = 143,
			/// s9 RecvOpcode.DENY_ALLIANCE_REQUEST = 0x90
			DENY_ALLIANCE_REQUEST = 144,
			/// s9 RecvOpcode.BBS_OPERATION = 0x9B → BBSOperationHandler
			BBS_OPERATION = 155,
			/// s9 RecvOpcode.ENTER_MTS = 0x9C
			ENTER_MTS = 156,
			/// s9 RecvOpcode.USE_SOLOMON_ITEM = 0x9D
			USE_SOLOMON_ITEM = 157,
			/// s9 RecvOpcode.USE_GACHA_EXP = 0x9E
			USE_GACHA_EXP = 158,
			/// s9 RecvOpcode.ARAN_COMBO_COUNTER = 0xA3（空包）
			ARAN_COMBO_COUNTER = 163,
			/// s9 RecvOpcode.OPEN_ITEMUI = 0xEC（Raise/Plant infoNumber）
			OPEN_ITEMUI = 236,
			/// s9 RecvOpcode.WORLD_MAP_PLAYERS = 0x115
			WORLD_MAP_PLAYERS = 277,
			/// s9 RecvOpcode.WORLD_MAP_DROP_REQUEST = 0x116
			WORLD_MAP_DROP_REQUEST = 278,
			/// s9 RecvOpcode.SET_HPMPALERT = 0x1000（扩展通道）：
			/// body = byte hp_step + byte mp_step（0..19 挡位，每挡 5%；服务端限幅）
			SET_HPMPALERT = 4096
		};

	protected:
		// Skip a number of bytes (filled with zeros)
		void skip(size_t count);
		// Write a byte
		void write_byte(int8_t ch);
		// Write a short
		void write_short(int16_t sh);
		// Write an int
		void write_int(int32_t in);
		// Write a long
		void write_long(int64_t lg);

		// Write a point
		// One short for x and one for y
		void write_point(Point<int16_t> point);
		// Write a timestamp as an integer
		void write_time();
		// Write a string
		// Writes the length as a short and then each individual character as a byte
		void write_string(const std::string& str);
		// Write a random int
		void write_random();

		// Write the MACS and then write the HWID of the computer
		void write_hardware_info();
		// Function to convert hexadecimal to decimal
		int32_t hex_to_dec(std::string hexVal);

	private:
		std::vector<int8_t> bytes;
		int16_t opcode;
	};
}