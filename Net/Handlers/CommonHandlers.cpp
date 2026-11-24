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
#include "CommonHandlers.h"

#include "../Packets/CommonPackets.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIAdmin.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UISideIconBar.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Util/SideMenuConfig.h"
#include "../../Util/DamageSkinState.h"
#include "../../Util/BeautyState.h"
#include "../../IO/Window.h"
#include "../../Graphics/Color.h"
#include "../../Gameplay/Stage.h"
#include "../../Gameplay/Combat/Damage.h"
#include "../../Character/CombatPanelStats.h"
#include "../../Character/SkillBonusCache.h"
#include "../../Character/SkillStatOverrideCache.h"
#include "../../Data/SkillData.h"
#include "../../Character/Player.h"
#include "../../Data/EquipGrowthData.h"
#include "../../Data/SetItemData.h"
#include "../../Weather/WeatherState.h"
#include "../../Weather/WeatherConfig.h"
#include "../../Weather/WeatherLamps.h"
#include "../../Util/PerfLog.h"

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace ms
{
	void PingHandler::handle(InPacket&) const
	{
		PongPacket().dispatch();
	}

	void LimitBreakSyncHandler::handle(InPacket& recv) const
	{
		int64_t lb = recv.read_long();
		while (recv.available())
			recv.skip_byte();
		Window::get().set_limit_break(lb);
		// 破功值不再只用于标题：同步给伤害 clamp（min(配置 cap, 破功值, 2^53)）。
		// 负值/0 表示服务端未配置上限，effective_cap 会忽略该项。
		damage::set_server_limit_break(lb);
		std::string tip = "LimitBreak " + std::to_string(lb);
		if (auto ui = UI::get().get_element<UIAdmin>())
		{
			if (ui->is_active())
			{
				ui->set_message(tip);
				return;
			}
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(tip.c_str(), UIChatBar::MessageType::WHITE);
	}

	void SetItemFinalDamageHandler::handle(InPacket& recv) const
	{
		// 布局对齐 BeiDou PacketCreator.setItemFinalDamageBonus / ijl15 SetItemBridge
		CombatPanelStats& s = CombatPanelStats::get();
		s.final_damage_percent = recv.read_short();
		s.damage_skin_id = recv.read_int();
		s.dam_r = recv.read_short();
		s.boss_dam_r = recv.read_short();
		s.normal_dam_r = recv.read_short();
		s.ignore_pdr = recv.read_short();
		s.ignore_mdr = recv.read_short();
		s.crit_rate = recv.read_short();
		s.crit_dam = recv.read_short();
		s.pad_r = recv.read_short();
		s.mad_r = recv.read_short();

		const uint16_t fd_count = static_cast<uint16_t>(recv.read_short());
		for (uint16_t i = 0; i < fd_count; i++)
			recv.skip_short();

		s.item_drop_prop = recv.read_short();
		s.meso_drop_prop = recv.read_short();
		s.damage_reduce = recv.read_short();

		if (recv.available() >= 6)
		{
			s.asr_r = recv.read_short();
			s.buff_time_r = recv.read_short();
			s.stance_prop = recv.read_short();
		}

		while (recv.available())
			recv.skip_byte();

		s.synced = true;
		s.notify_ui();
		LOG(LOG_DEBUG, "CombatPanel 0x175 synced damR=" << s.dam_r
			<< " fdR=" << s.final_damage_percent
			<< " bdR=" << s.boss_dam_r);
	}

	void SetItemSkillBonusHandler::handle(InPacket& recv) const
	{
		// 布局（对接 s9 PacketCreator.setItemSkillBonusAll/Single）：
		//   short count + { int setId, byte enabled, string text, short itemCount, int itemId×N } × count
		const int16_t count = recv.read_short();

		for (int16_t i = 0; i < count; ++i)
		{
			if (recv.available() < 4)
				break;

			const int32_t set_id = recv.read_int();
			const uint8_t enabled = static_cast<uint8_t>(recv.read_byte());
			const std::string text = recv.read_string();
			const int16_t item_count = recv.read_short();

			std::vector<int32_t> item_ids;
			item_ids.reserve(item_count > 0 ? item_count : 0);

			for (int16_t j = 0; j < item_count; ++j)
			{
				if (recv.available() < 4)
					break;
				item_ids.push_back(recv.read_int());
			}

			// tier 列表：short tierCount + {short need, string statKey, int value}×N
			std::vector<std::tuple<int16_t, std::string, int32_t>> tiers;
			if (recv.available() >= 2)
			{
				const int16_t tier_count = recv.read_short();
				for (int16_t k = 0; k < tier_count; ++k)
				{
					if (recv.available() < 2)
						break;
					const int16_t need = recv.read_short();
					const std::string stat_key = recv.read_string();
					const int32_t value = recv.read_int();
					tiers.emplace_back(need, stat_key, value);
				}
			}

			if (set_id <= 0)
				continue;

			// 服务端权威：建映射 + 文案 + tier（tier 让本地 Player 注入链路复活）。
			if (!item_ids.empty())
				SetItemData::set_server_mapping(set_id, item_ids);
			for (const auto& t : tiers)
				SetItemData::set_server_tier(set_id, std::get<0>(t), std::get<1>(t), std::get<2>(t));
			if (enabled && !text.empty())
				SetItemData::set_server_text(set_id, text);
		}

		// 套装 tier 注入后重算玩家属性，让伤害/属性即时生效（与换装同口径）
		Stage::get().get_player().recalc_stats(true);
	}

	void SetSkillBonusHandler::handle(InPacket& recv) const
	{
		// wire: short count + { int skillId, int addLevel } × count
		if (recv.available() < 2)
			return;

		const int16_t count = recv.read_short();
		std::unordered_map<int32_t, int32_t> levels;

		for (int16_t i = 0; i < count; ++i)
		{
			if (recv.available() < 8)
				break;

			const int32_t skill_id = recv.read_int();
			const int32_t add = recv.read_int();

			if (skill_id > 0 && add != 0)
				levels[skill_id] = add;
		}

		SkillBonusCache::get().replace_all(levels);
		LOG(LOG_DEBUG, "SET_SKILL_BONUS 0x17A entries=" << levels.size());
	}

	void SkillStatOverrideHandler::handle(InPacket& recv) const
	{
		// wire: short skillCount + { int skillId, byte levelCount,
		//       每级 { byte level, byte fieldCount, 每字段 { byte keyIdx, int value } } }
		if (recv.available() < 2)
			return;

		const int16_t count = recv.read_short();
		auto& touched = SkillStatOverrideCache::get();

		if (count <= 0)
		{
			// 清覆盖：对本包之前登记过的技能清 overlay，然后清空登记表 → 全部回退 NX。
			for (int32_t skill_id : touched.touched())
			{
				const SkillData& sd = SkillData::get(skill_id);
				const_cast<SkillData&>(sd).clear_overlay();
			}
			touched.clear();
			LOG(LOG_DEBUG, "SKILL_STAT_OVERRIDE 0x3745 clear all");
			return;
		}

		for (int16_t i = 0; i < count; ++i)
		{
			if (recv.available() < 5)
				break;
			const int32_t skill_id = recv.read_int();
			const int32_t level_count = recv.read_byte();
			if (skill_id <= 0)
				continue;
			touched.touch(skill_id);
			const SkillData& sd = SkillData::get(skill_id);
			SkillData& mutable_sd = const_cast<SkillData&>(sd);

			for (int32_t j = 0; j < level_count; ++j)
			{
				if (recv.available() < 2)
					break;
				const int32_t level = recv.read_byte();
				const int32_t field_count = recv.read_byte();
				std::unordered_map<int32_t, int32_t> fields;
				for (int32_t k = 0; k < field_count; ++k)
				{
					if (recv.available() < 5)
						break;
					const int32_t key_idx = recv.read_byte();
					const int32_t value = recv.read_int();
					fields[key_idx] = value;
				}
				mutable_sd.apply_overlay(level, fields);
			}
		}
		LOG(LOG_DEBUG, "SKILL_STAT_OVERRIDE 0x3745 skills=" << count);
	}

	void EquipGrowthTipHandler::handle(InPacket& recv) const
	{
		// wire 对齐 ijl15 EquipGrowthBridge / PacketCreator.equipGrowthTip：
		// int itemId | byte hasData | string text | byte hasBonus + short[15]? | byte hasFlame + short[15]?
		if (recv.available() < 5)
			return;

		const int32_t item_id = recv.read_int();
		const bool has_data = recv.read_byte() != 0;
		const std::string text = recv.read_string();

		int16_t bonus[EquipGrowthData::BONUS_SLOTS] = {};
		bool has_bonus = false;
		int16_t flame[EquipGrowthData::BONUS_SLOTS] = {};
		bool has_flame = false;

		if (recv.available() >= 1)
		{
			const uint8_t flag = static_cast<uint8_t>(recv.read_byte());

			if (flag != 0 && recv.available() >= EquipGrowthData::BONUS_SLOTS * 2)
			{
				has_bonus = true;

				for (int i = 0; i < EquipGrowthData::BONUS_SLOTS; ++i)
					bonus[i] = recv.read_short();
			}
		}

		if (recv.available() >= 1)
		{
			const uint8_t flag = static_cast<uint8_t>(recv.read_byte());

			if (flag != 0 && recv.available() >= EquipGrowthData::BONUS_SLOTS * 2)
			{
				has_flame = true;

				for (int i = 0; i < EquipGrowthData::BONUS_SLOTS; ++i)
					flame[i] = recv.read_short();
			}
		}

		EquipGrowthData::TipEntry existing;
		int32_t item_level = 0;
		int32_t scroll = 0;

		if (EquipGrowthData::try_get(item_id, existing))
		{
			item_level = existing.item_level;
			scroll = existing.scroll;
		}

		EquipGrowthData::set_server_tip(item_id, has_data, text,
			has_bonus ? bonus : nullptr, has_bonus,
			has_flame ? flame : nullptr, has_flame,
			item_level, scroll);

		LOG(LOG_DEBUG, "EQUIP_GROWTH_TIP 0x17B item=" << item_id
			<< " has=" << (has_data ? 1 : 0) << " textLen=" << text.size());
	}

	void WeatherSyncHandler::handle(InPacket& recv) const
	{
		// wire 逐字段对齐 BeiDou WeatherPackets.java:82-109（opcode 已被 PacketSwitch 读掉）：
		//   short minute | int msPerMin | byte sky | byte flags | int skyElapsedSec(legacy)
		//   byte tintR/G/B | short rainbowSecs | byte palette | int skyElapsedMs | int skyToken
		// 宽容解码：头 8 字节必读，其余用 available() 逐段判定，兼容老 19B 服务端包。
		if (recv.available() < 8)
			return;  // 短包直接丢弃，绝不猜字段（避免错位静默渲染错天空）

		const int minute = recv.read_short();
		const int ms_per_min = recv.read_int();
		const uint8_t sky = static_cast<uint8_t>(recv.read_byte());
		const uint8_t flags = static_cast<uint8_t>(recv.read_byte());

		// legacy skyElapsedSec 4B 占位：本端不消费，但必须读掉，否则 tint 之后全偏移。
		int sky_elapsed_sec = 0;
		if (recv.available() >= 4)
			sky_elapsed_sec = recv.read_int();

		uint8_t tint_r = 0x4A, tint_g = 0x5A, tint_b = 0x8C;
		if (recv.available() >= 3)
		{
			tint_r = static_cast<uint8_t>(recv.read_byte());
			tint_g = static_cast<uint8_t>(recv.read_byte());
			tint_b = static_cast<uint8_t>(recv.read_byte());
		}

		int rainbow_secs = 0;
		if (recv.available() >= 2)
			rainbow_secs = recv.read_short();

		uint8_t palette = static_cast<uint8_t>(weather::PaletteId::DEFAULT);
		if (recv.available() >= 1)
			palette = static_cast<uint8_t>(recv.read_byte());

		int sky_elapsed_ms = sky_elapsed_sec * 1000;
		if (recv.available() >= 4)
			sky_elapsed_ms = recv.read_int();

		uint32_t token = 0;
		if (recv.available() >= 4)
			token = static_cast<uint32_t>(recv.read_int());

		weather::State::get().apply(minute, ms_per_min, sky, flags, sky_elapsed_sec,
			rainbow_secs, tint_r, tint_g, tint_b, palette, sky_elapsed_ms, token);

		LOG(LOG_DEBUG, "[Weather] 0x373D minute=" << minute
			<< " sky=" << static_cast<int>(sky) << " palette=" << static_cast<int>(palette)
			<< " flags=" << static_cast<int>(flags)
			<< " tint=" << static_cast<int>(tint_r) << ',' << static_cast<int>(tint_g) << ',' << static_cast<int>(tint_b));
	}

	void WeatherIntensityHandler::handle(InPacket& recv) const
	{
		// wire: short version | byte[9] intensity(0轻/1中/2强)，共 11 字节（opcode 已读）。
		// 2026-11-22：原 <10 会让 10B 截断包在第 11 次 read_byte 越界抛 PacketError。
		if (recv.available() < 11)
			return;
		const uint16_t version = static_cast<uint16_t>(recv.read_short());
		uint8_t rows[9] = {1, 1, 1, 1, 2, 2, 1, 1, 2};
		for (int i = 0; i < 9; ++i)
			rows[i] = static_cast<uint8_t>(recv.read_byte());
		weather::Config::get().apply_intensities(version, rows, 9);
		LOG(LOG_DEBUG, "[Weather] 0x3741 intensity v=" << version
			<< " rows=" << static_cast<int>(rows[1]) << ',' << static_cast<int>(rows[2])
			<< ',' << static_cast<int>(rows[4]) << ',' << static_cast<int>(rows[5]));
	}

	void WeatherElementHandler::handle(InPacket& recv) const
	{
		// 2026-11-23 V2 0x3742：short version | short gameDate(MMdd) | byte[16] elements。
		if (recv.available() < 4)
			return;
		const uint16_t version = static_cast<uint16_t>(recv.read_short());
		const int game_date = recv.read_short();
		uint8_t rows[weather::EL_COUNT] = {};
		const size_t want = recv.available() < weather::EL_COUNT
			? static_cast<size_t>(recv.available()) : static_cast<size_t>(weather::EL_COUNT);
		for (size_t i = 0; i < want; ++i)
			rows[i] = static_cast<uint8_t>(recv.read_byte());
		weather::State::get().apply_elements(static_cast<uint8_t>(version), game_date, rows, want);
		LOG(LOG_DEBUG, "[Weather] 0x3742 v=" << version << " date=" << game_date
			<< " fog=" << static_cast<int>(rows[weather::EL_FOG]));
	}

	void LampPreviewHandler::handle(InPacket& recv) const
	{
		// wire: byte mode(0 place/1 clear) | byte lampId | int mapId | int x | int y
		// 16 字节含 opcode，opcode 已读掉 → 这里 14 字节定长。
		if (recv.available() < 14)
			return;
		const uint8_t mode = static_cast<uint8_t>(recv.read_byte());
		const uint8_t lamp_id = static_cast<uint8_t>(recv.read_byte());
		const int32_t map_id = recv.read_int();
		const int32_t x = recv.read_int();
		const int32_t y = recv.read_int();

		weather::Lamps::get().apply_preview(mode, lamp_id, map_id, x, y);
		LOG(LOG_DEBUG, "[Weather] 0x373F lamp preview mode=" << static_cast<int>(mode)
			<< " lamp=" << static_cast<int>(lamp_id) << " map=" << map_id << " @(" << x << ',' << y << ')');
	}

	void SideMenuSyncHandler::handle(InPacket& recv) const
	{
		// wire: int revision | maple string (UTF-8 JSON body)
		if (recv.available() < 6)
			return;
		const int32_t revision = recv.read_int();
		const std::string json = recv.read_string();
		LOG(LOG_INFO, "SideMenuSync: recv revision=" << revision
			<< " json_bytes=" << json.size());

		if (!SideMenuConfig::apply_remote_json(json))
			return;

		if (auto bar = UI::get().get_element<UISideIconBar>())
			bar->reload_config();
	}

	// ---- 伤害皮肤 picker 下行 ----
	void DamageSkinCatalogHandler::handle(InPacket& recv) const
	{
		if (recv.available() < 2)
			return;
		const int16_t count = recv.read_short();
		std::map<int32_t, int64_t> prices;
		for (int16_t i = 0; i < count && recv.available() >= 12; ++i)
		{
			const int32_t id = recv.read_int();
			const int64_t price = recv.read_long();
			prices[id] = price;
		}
		DamageSkinState::set_catalog(prices);
		LOG(LOG_INFO, "DamageSkinCatalog: entries=" << prices.size());
	}

	void DamageSkinInventoryHandler::handle(InPacket& recv) const
	{
		if (recv.available() < 6)
			return;
		const int32_t active = recv.read_int();
		const int16_t count = recv.read_short();
		std::set<int32_t> owned;
		for (int16_t i = 0; i < count && recv.available() >= 4; ++i)
			owned.insert(recv.read_int());
		DamageSkinState::set_inventory(active, owned);
		LOG(LOG_INFO, "DamageSkinInventory: active=" << active << " owned=" << owned.size());
	}

	void DamageSkinResultHandler::handle(InPacket& recv) const
	{
		if (recv.available() < 10)
			return;
		const int32_t op = recv.read_byte();
		const bool ok = recv.read_byte() != 0;
		const int32_t skin = recv.read_int();
		const int32_t mesos = recv.read_int();
		DamageSkinState::set_result(op, ok, skin, mesos);
		LOG(LOG_INFO, "DamageSkinResult: op=" << op << " ok=" << ok
			<< " skin=" << skin << " mesos=" << mesos);
	}

	void DamageSkinBroadcastHandler::handle(InPacket& recv) const
	{
		if (recv.available() < 8)
			return;
		// 他人皮肤 id 由 DAMAGE_MONSTER 包逐次携带；这里仅记录日志，不改全局状态。
		const int32_t cid = recv.read_int();
		const int32_t skin = recv.read_int();
		LOG(LOG_DEBUG, "[DamageSkin] broadcast cid=" << cid << " skin=" << skin);
	}

	void BeautyResultHandler::handle(InPacket& recv) const
	{
		if (recv.available() < 1) return;
		const int32_t resp = recv.read_byte();
		if (resp == 0)
		{
			BeautyState::mark_opened();
			return;
		}
		if (resp != 1 || recv.available() < 43) return;
		const int32_t unlocked = recv.read_byte();
		std::array<int32_t, 6> hair{}, face{}, skin{};
		std::array<bool, 6> hs{}, fs{}, ss{};
		for (int i = 0; i < 6; ++i) hair[i] = recv.read_int();
		for (int i = 0; i < 6; ++i) face[i] = recv.read_int();
		for (int i = 0; i < 6; ++i) skin[i] = recv.read_int();
		for (int i = 0; i < 6; ++i) hs[i] = recv.read_byte() != 0;
		for (int i = 0; i < 6; ++i) fs[i] = recv.read_byte() != 0;
		for (int i = 0; i < 6; ++i) ss[i] = recv.read_byte() != 0;
		// 跳过 rows 明细（每 row: byte type,slot,gender + 10×int = 43 字节）
		if (recv.available() >= 1)
		{
			const int rows = recv.read_byte();
			for (int r = 0; r < rows && recv.available() >= 43; ++r)
			{
				recv.skip(3);          // type, slot, gender
				for (int f = 0; f < 10; ++f) recv.read_int();
			}
		}
		BeautyState::apply(unlocked, hair, face, skin, hs, fs, ss);
		LOG(LOG_INFO, "[Beauty] data unlocked=" << unlocked);
	}
}