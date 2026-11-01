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
#include "MessagingHandlers.h"

#include "../../Data/ItemData.h"
#include "../../Data/QuestData.h"
#include "../../Character/MapleStat.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"

#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UICharInfo.h"
#include "../../IO/UITypes/UIFamily.h"
#include "../../IO/UITypes/UIAlliance.h"
#include "../../IO/UITypes/UIGuild.h"
#include "../../IO/UITypes/UIEquipInventory.h"
#include "../../IO/UITypes/UIItemInventory.h"
#include "../../IO/UITypes/UIMonsterBook.h"
#include "../../IO/UITypes/UIQuestLog.h"
#include "../../IO/UITypes/UISkillBook.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../IO/UITypes/UIStatusBar.h"
#include "../../IO/UITypes/UIBuddyList.h"
#include "../../IO/UITypes/UIStatsInfo.h"
#include "../../IO/UITypes/UIWedding.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Graphics/Color.h"
#include "../../Util/Misc.h"

namespace ms
{
	// Modes:
	// 0 - Item(0) or Meso(1)
	// 1 - Quest progress (short questId, byte status, …)
	// 3 - Exp gain
	// 4 - Fame
	// 5 - Mesos
	// 6 - Guild points
	void ShowStatusInfoHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.read_byte();

		if (mode == 0)
		{
			int8_t mode2 = recv.read_byte();
			bool inv_open = false;
			if (auto inv = UI::get().get_element<UIItemInventory>())
				inv_open = inv->is_active();
			if (!inv_open)
				if (auto eq = UI::get().get_element<UIEquipInventory>())
					inv_open = eq->is_active();

			if (mode2 == -1)
			{
				while (recv.available())
					recv.skip_byte();
				// 背包已开：格子满自显，不叠 SM
				if (inv_open)
					return;
				show_status(Color::Name::WHITE, "You can't get anymore items.");
			}
			else if (mode2 == 0)
			{
				int32_t itemid = recv.read_int();
				int32_t qty = recv.read_int();

				const ItemData& idata = ItemData::get(itemid);

				while (recv.available())
					recv.skip_byte();
				if (!idata.is_valid())
					return;

				std::string name = idata.get_name();

				if (name.length() > 21)
					name = name.substr(0, 21) + "..";

				InventoryType::Id type = InventoryType::by_item_id(itemid);

				std::string tab = "";

				switch (type)
				{
					case InventoryType::Id::EQUIP:
						tab = "Eqp";
						break;
					case InventoryType::Id::USE:
						tab = "Use";
						break;
					case InventoryType::Id::SETUP:
						tab = "Setup";
						break;
					case InventoryType::Id::ETC:
						tab = "Etc";
						break;
					case InventoryType::Id::CASH:
						tab = "Cash";
						break;
					default:
						tab = "UNKNOWN";
						break;
				}

				// 背包已开：格子自显，不叠 SM
				if (inv_open)
					return;

				if (qty < 0)
				{
					int32_t lost = -qty;
					std::string tip = lost == 1
						? ("You have lost an item in the " + tab + " tab (" + name + ")")
						: ("You have lost items in the " + tab + " tab (" + name + " " + std::to_string(lost) + ")");
					show_status(Color::Name::RED, tip);
				}
				else if (qty == 1)
					show_status(Color::Name::WHITE, "You have gained an item in the " + tab + " tab (" + name + ")");
				else
					show_status(Color::Name::WHITE, "You have gained items in the " + tab + " tab (" + name + " " + std::to_string(qty) + ")");
			}
			else if (mode2 == 1)
			{
				recv.skip(1);

				int32_t gain = recv.read_int();
				while (recv.available())
					recv.skip_byte();
				if (inv_open)
					return;
				if (auto bar = UI::get().get_element<UIStatusBar>())
				{
					if (bar->is_active())
						return;
				}
				if (auto st = UI::get().get_element<UIStatsInfo>())
				{
					if (st->is_active())
						return;
				}
				std::string tip = gain < 0
					? ("You have lost mesos (" + std::to_string(gain) + ")")
					: ("You have gained mesos (+" + std::to_string(gain) + ")");
				show_status(gain < 0 ? Color::Name::RED : Color::Name::WHITE, tip);
			}
			else
			{
				LOG(LOG_DEBUG, "ShowStatusInfo mode0/2=" << static_cast<int>(mode2)
					<< " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
			}
		}
		else if (mode == 1)
		{
			// Quest progress: short questId, byte status
			// status 0 = forfeit, 1 = started/update, 2 = complete
			int16_t qid = recv.read_short();
			int8_t status = recv.read_byte();
			QuestLog& quests = Stage::get().get_player().get_quests();
			const QuestData& qdata = QuestData::get(qid);

			bool ql_open = false;
			if (auto questlog = UI::get().get_element<UIQuestLog>())
			{
				if (questlog->is_active())
					ql_open = true;
			}

			auto quest_tip = [&](Color::Name, const std::string& tip) {
				auto alarm = UI::get().get_element<UIQuestAlarm>();
				if (!alarm) alarm = UI::get().emplace<UIQuestAlarm>();
				else if (!alarm->is_active()) alarm->makeactive();
				if (alarm)
				{
					alarm->set_tip(tip);
					return;
				}
				if (auto chatbar = UI::get().get_element<UIChatBar>())
					chatbar->show_message(tip.c_str(), UIChatBar::MessageType::YELLOW);
			};
			// 服务端 s9 PacketCreator.java:2947-2962 对所有 status 写同一段：
			// short questId / byte status / string progress / skip(5)。
			// 旧实现把「完成时间」当成 status==2 的额外 read_long，但服务端从不序列化
			// 该字段——完成包尾只有 strlen(2)+pad(5)=7 字节，read_long 走到第 8 字节
			// 抛 Stack underflow，整个 handler 中断：已完成列表不写、UIQuestLog 不刷新、
			// 提示也不出。战神的 1→2→3→4 转全部由 quest 交付，正是被这一步卡住。
			std::string progress = recv.read_string();

			if (recv.available() >= 5)
				recv.skip(5);

			if (status == 0)
			{
				quests.remove_started(qid);
				if (!ql_open)
					quest_tip(Color::Name::WHITE, "Quest forfeited: " + qdata.get_name());
			}
			else if (status == 2)
			{
				// 完成时间服务端不下发；UIQuestLog 只用 get_completed() 的 key 列表，
				// 时间戳在客户端无消费者，故记 0。
				quests.set_completed(qid, 0);
				if (!ql_open)
					quest_tip(Color::Name::YELLOW, "Quest completed: " + qdata.get_name());
			}
			else
			{
				quests.set_started(qid, progress);
				if (!ql_open)
					quest_tip(Color::Name::WHITE, "Quest updated: " + qdata.get_name());
			}

			while (recv.available())
				recv.skip_byte();
			if (auto questlog = UI::get().get_element<UIQuestLog>())
				questlog->refresh();
		}
		else if (mode == 3)
		{
			bool white = recv.read_bool();
			int32_t gain = recv.read_int();
			bool inchat = recv.read_bool();
			int32_t bonus1 = recv.read_int();

			recv.read_short();
			recv.read_int();	// bonus 2
			recv.read_bool();	// 'event or party'
			recv.read_int();	// bonus 3
			recv.read_int();	// bonus 4
			recv.read_int();	// bonus 5

			while (recv.available())
				recv.skip_byte();

			std::string message = "You have gained experience (+" + std::to_string(gain) + ")";
			Color::Name col = white ? Color::Name::WHITE : Color::Name::YELLOW;

			// StatusBar / Stats 已显 EXP：非 chat 路径静默（不叠 SM）
			bool exp_ui = false;
			if (auto bar = UI::get().get_element<UIStatusBar>())
				exp_ui = bar->is_active();
			if (!exp_ui)
				if (auto st = UI::get().get_element<UIStatsInfo>())
					exp_ui = st->is_active();

			if (inchat)
			{
				if (auto chatbar = UI::get().get_element<UIChatBar>())
					chatbar->show_message(message.c_str(),
						white ? UIChatBar::MessageType::WHITE : UIChatBar::MessageType::YELLOW);
				else if (!exp_ui)
					show_status(col, message);
			}
			else if (!exp_ui)
			{
				show_status(col, message);
			}

			if (bonus1 > 0)
			{
				std::string bonus = "+ Bonus EXP (+" + std::to_string(bonus1) + ")";
				if (inchat)
				{
					if (auto chatbar = UI::get().get_element<UIChatBar>())
						chatbar->show_message(bonus.c_str(), UIChatBar::MessageType::YELLOW);
					else if (!exp_ui)
						show_status(Color::Name::YELLOW, bonus);
				}
				else if (!exp_ui)
					show_status(Color::Name::YELLOW, bonus);
			}
		}
		else if (mode == 4)
		{
			int32_t gain = recv.read_int();
			std::string tip = gain < 0
				? ("You have lost fame. (" + std::to_string(gain) + ")")
				: ("You have gained fame. (+" + std::to_string(gain) + ")");
			while (recv.available())
				recv.skip_byte();
			// CharInfo ensure：人气自显，刷新并跳过 tip
			int32_t self = Stage::get().get_player().get_oid();
			auto ci = UI::get().get_element<UICharInfo>();
			if (!ci) ci = UI::get().emplace<UICharInfo>(self);
			else if (!ci->is_active()) ci->makeactive();
			if (ci)
			{
				CharInfoRequestPacket(self).dispatch();
				return;
			}
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(tip.c_str(),
					gain < 0 ? UIChatBar::MessageType::RED : UIChatBar::MessageType::WHITE);
		}
		else if (mode == 5)
		{
			// SP gain（短/整均可；优先 short）
			int32_t sp = recv.available() >= 2 ? recv.read_short() : 0;
			if (recv.available() >= 2)
				recv.read_short(); // job（部分包）
			std::string tip = "You have gained SP (+" + std::to_string(sp) + ")";
			while (recv.available())
				recv.skip_byte();
			// SkillBook / SkillUp / Stats：刷 UI，不 tip
			if (auto sb = UI::get().get_element<UISkillBook>())
			{
				if (sb->is_active())
				{
					sb->update_stat(MapleStat::Id::SP,
						Stage::get().get_player().get_stats().get_stat(MapleStat::Id::SP));
					return;
				}
			}
			if (auto st = UI::get().get_element<UIStatsInfo>())
			{
				if (st->is_active())
				{
					st->update_all_stats();
					return;
				}
			}
			auto su = UI::get().get_element<UISkillUp>();
			if (!su) su = UI::get().emplace<UISkillUp>();
			else if (!su->is_active()) su->makeactive();
			if (su)
			{
				su->set_tip(tip);
				return;
			}
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(tip.c_str(), UIChatBar::MessageType::YELLOW);
		}
		else if (mode == 6)
		{
			int32_t gp = recv.available() >= 4 ? recv.read_int() : 0;
			std::string tip = gp < 0
				? ("You have lost GP (" + std::to_string(gp) + ")")
				: ("You have gained GP (+" + std::to_string(gp) + ")");
			while (recv.available())
				recv.skip_byte();
			auto guild = UI::get().get_element<UIGuild>();
			if (!guild) guild = UI::get().emplace<UIGuild>();
			else if (!guild->is_active()) guild->makeactive();
			if (guild)
			{
				guild->set_notice(tip);
				return;
			}
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(tip.c_str(),
					gp < 0 ? UIChatBar::MessageType::RED : UIChatBar::MessageType::YELLOW);
		}
		else
		{
			LOG(LOG_DEBUG, "ShowStatusInfo mode=" << static_cast<int>(mode)
				<< " rem=" << recv.available());
			while (recv.available())
				recv.skip_byte();
		}
	}

	void ShowStatusInfoHandler::show_status(Color::Name color, const std::string& message) const
	{
		auto messenger = UI::get().get_element<UIStatusMessenger>();
		if (!messenger)
			messenger = UI::get().emplace<UIStatusMessenger>();
		if (messenger)
			messenger->show_status(color, message);
	}

	void ServerMessageHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.read_byte();
		bool servermessage = recv.inspect_bool();

		if (servermessage)
			recv.skip_byte();

		const std::string& message = recv.read_string();

		auto push_chat = [&](UIChatBar::MessageType mt) {
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(message.c_str(), mt);
		};
		auto push_sm = [&](Color::Name color = Color::Name::YELLOW) {
			auto sm = UI::get().get_element<UIStatusMessenger>();
			if (!sm)
				sm = UI::get().emplace<UIStatusMessenger>();
			if (sm)
				sm->show_status(color, message);
		};
		// 仅喇叭类强制 FloatNotice；其余走 Chat/SM/Scroll
		auto push_fn = [&]() {
			auto fn = UI::get().get_element<UIFloatNotice>();
			if (!fn)
				fn = UI::get().emplace<UIFloatNotice>();
			else if (!fn->is_active())
				fn->makeactive();
			if (fn)
				fn->set_tip(message);
		};

		if (type == 3)
		{
			// Super megaphone：channel + ear
			int8_t channel = recv.read_byte();
			bool megaEar = recv.read_bool();
			std::string line = message;
			if (channel >= 0)
				line = "[Ch." + std::to_string(channel + 1) + "] " + message;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(line.c_str(), UIChatBar::MessageType::YELLOW);
			else
				push_sm();
			{
				auto fn = UI::get().get_element<UIFloatNotice>();
				if (!fn)
					fn = UI::get().emplace<UIFloatNotice>();
				else if (!fn->is_active())
					fn->makeactive();
				if (fn)
					fn->set_tip(line);
			}
			(void)megaEar;
		}
		else if (type == 4)
		{
			UI::get().set_scrollnotice(message);
			// 已有滚动条，不强制 FloatNotice
		}
		else if (type == 5)
		{
			push_chat(UIChatBar::MessageType::WHITE);
			// 长文仅 Chat，不强制 MultiLine / FloatNotice
		}
		else if (type == 6)
		{
			// 蓝字 / 道具喇叭类：chat 优先，无 ChatBar 才 SM
			recv.skip_int();
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(message.c_str(), UIChatBar::MessageType::BLUE);
			else
				push_sm();
		}
		else if (type == 7)
		{
			int32_t npc = recv.read_int();
			std::string line = "[NPC " + std::to_string(npc) + "] " + message;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(line.c_str(), UIChatBar::MessageType::YELLOW);
		}
		else
		{
			// 0 notice / 1 popup / 2 megaphone 等
			bool has_chat = false;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(message.c_str(),
					type == 1 ? UIChatBar::MessageType::RED : UIChatBar::MessageType::YELLOW);
				has_chat = true;
			}
			if (type == 2)
				push_fn(); // 普通喇叭仍走 FloatNotice
			else if (!has_chat)
				push_sm(type == 1 ? Color::Name::RED : Color::Name::YELLOW);
		}
		while (recv.available())
			recv.skip_byte();
	}

	void WeekEventMessageHandler::handle(InPacket& recv) const
	{
		if (recv.available())
			recv.read_byte(); // 常为 0xFF

		std::string message = recv.read_string();

		static const char* MAPLETIP = "[MapleTip]";
		if (message.compare(0, 10, MAPLETIP) != 0)
			message = "[Notice] " + message;

		while (recv.available())
			recv.skip_byte();  // s9 在正文尾补了一个 short（PacketCreator.java:5441）

		// s9 用这个 opcode 发 yellow tip（PacketCreator.java:5437-5443 sendYellowTip，
		// Character.java:6952 的 yellowMessage 及 7 处 GM 广播都走它），不是周活动列表；
		// UIEvent 侧没有任何从本包取数据的通路（构造函数塞的是 16 条硬编码占位），
		// 此前强行开窗既让 GM 命令的反馈文字看不见，又会在资源缺失的半成品元素上
		// 越界崩（UIEvent.cpp:123，见 docs/features/s9-protocol-align/notes/crash-chatbar.md §8）。
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(message.c_str(), UIChatBar::MessageType::YELLOW);
	}

	void ChatReceivedHandler::handle(InPacket& recv) const
	{
		int32_t charid = recv.read_int();

		recv.read_bool(); // 'gm'

		std::string message = recv.read_string();
		int8_t type = recv.read_byte();

		if (auto character = Stage::get().get_character(charid))
		{
			message = character->get_name() + " : " + message;
			character->speak(message);
		}

		auto linetype = static_cast<UIChatBar::MessageType>(type);

		while (recv.available())
			recv.skip_byte();
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(message.c_str(), linetype);
	}

	void ScrollResultHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();
		bool success = recv.read_bool();
		bool destroyed = recv.read_bool();

		recv.read_short(); // Legendary spirit if 1

		CharEffect::Id effect;
		Messages::Type message;

		if (success)
		{
			effect = CharEffect::Id::SCROLL_SUCCESS;
			message = Messages::Type::SCROLL_SUCCESS;
		}
		else
		{
			effect = CharEffect::Id::SCROLL_FAILURE;

			if (destroyed)
				message = Messages::Type::SCROLL_DESTROYED;
			else
				message = Messages::Type::SCROLL_FAILURE;
		}

		Stage::get().show_character_effect(cid, effect);

		if (Stage::get().is_player(cid))
		{
			// 装备栏已开：卷轴结果特效自显，不叠 tip
			bool equip_open = false;
			if (auto eq = UI::get().get_element<UIEquipInventory>())
				equip_open = eq->is_active();
			if (!equip_open)
			{
				const char* tip = Messages::messages[message];
				if (auto chatbar = UI::get().get_element<UIChatBar>())
					chatbar->show_message(tip, UIChatBar::MessageType::RED);
			}

			UI::get().enable();
		}
		while (recv.available())
			recv.skip_byte();
	}

	void ShowItemGainInChatHandler::handle(InPacket& recv) const
	{
		int8_t mode1 = recv.read_byte();

		if (mode1 == 3)
		{
			int8_t mode2 = recv.read_byte();

			if (mode2 == 1) // This is actually 'item gain in chat'
			{
				int32_t itemid = recv.read_int();
				int32_t qty = recv.read_int();

				const ItemData& idata = ItemData::get(itemid);

				if (!idata.is_valid())
				{
					while (recv.available())
						recv.skip_byte();
					return;
				}

				std::string name = idata.get_name();
				std::string message = qty < 0
					? ("Lost an item: " + name + " (" + std::to_string(qty) + ")")
					: ("Gained an item: " + name + " (+" + std::to_string(qty) + ")");

				// 背包已开：格子自显，不叠 chat/SM；否则 chat→SM
				bool inv_open = false;
				if (auto inv = UI::get().get_element<UIItemInventory>())
					inv_open = inv->is_active();
				if (inv_open)
				{
					while (recv.available())
						recv.skip_byte();
					return;
				}
				if (auto chatbar = UI::get().get_element<UIChatBar>())
					chatbar->show_message(message.c_str(),
						qty < 0 ? UIChatBar::MessageType::RED : UIChatBar::MessageType::BLUE);
			}
		}
		else if (mode1 == 13) // card effect
		{
			Stage::get().get_player().show_effect_id(CharEffect::Id::MONSTER_CARD);
			// MonsterBook 已开：reload，不 tip
			if (auto book = UI::get().get_element<UIMonsterBook>())
			{
				if (book->is_active())
				{
					book->reload();
					while (recv.available())
						recv.skip_byte();
					return;
				}
			}
		}
		else if (mode1 == 18) // intro effect
		{
			std::string path = recv.available() ? recv.read_string() : "";
			if (!path.empty())
				Stage::get().add_effect(path);
			LOG(LOG_DEBUG, "ShowItemGain intro path=" << path);
			// 特效已播，不 tip
		}
		else if (mode1 == 23) // info
		{
			std::string path = recv.available() ? recv.read_string() : "";
			int32_t extra = recv.available() >= 4 ? recv.read_int() : 0;
			if (!path.empty())
				Stage::get().add_effect(path);
			LOG(LOG_DEBUG, "ShowItemGain info path=" << path << " extra=" << extra);
			// 特效已播，不 tip
		}
		else // Buff effect
		{
			int32_t skillid = recv.read_int();

			// More bytes, but we don't need them.
			Stage::get().get_combat().show_player_buff(skillid);
		}
		while (recv.available())
			recv.skip_byte();
	}

	void MultiChatHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		std::string name = recv.available() ? recv.read_string() : "";
		std::string text = recv.available() ? recv.read_string() : "";
		const char* tag = "Multi";
		switch (mode)
		{
			case 0: tag = "Buddy"; break;
			case 1: tag = "Party"; break;
			case 2: tag = "Guild"; break;
			case 3: tag = "Alliance"; break;
			default: break;
		}
		std::string line = std::string("[") + tag + "] " + name + " : " + text;
		LOG(LOG_DEBUG, "MultiChat mode=" << (int)mode << " from=" << name);
		while (recv.available())
			recv.skip_byte();
		// Guild/Alliance：ensure 再 notice，不叠 ChatBar
		if (mode == 2)
		{
			auto ui = UI::get().get_element<UIGuild>();
			if (!ui) ui = UI::get().emplace<UIGuild>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui)
			{
				ui->set_notice(line);
				return;
			}
		}
		else if (mode == 3)
		{
			auto ui = UI::get().get_element<UIAlliance>();
			if (!ui) ui = UI::get().emplace<UIAlliance>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui)
			{
				ui->set_notice(line);
				return;
			}
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(line.c_str(), UIChatBar::MessageType::BLUE);
	}

	void WhisperResultHandler::handle(InPacket& recv) const
	{
		// PacketCreator::WhisperFlag
		constexpr int8_t F_LOCATION = 0x01;
		constexpr int8_t F_WHISPER = 0x02;
		constexpr int8_t F_RESULT = 0x08;
		constexpr int8_t F_RECEIVE = 0x10;
		constexpr int8_t F_LOCATION_FRIEND = 0x40;

		int8_t flag = recv.available() ? recv.read_byte() : 0;
		if ((flag & F_WHISPER) && (flag & F_RECEIVE))
		{
			std::string sender = recv.read_string();
			int8_t channel = recv.available() ? recv.read_byte() : 0;
			bool from_admin = recv.available() ? recv.read_bool() : false;
			std::string message = recv.available() ? recv.read_string() : "";
			(void)from_admin;
			std::string line = sender + " (ch" + std::to_string(channel + 1) + ") : " + message;
			LOG(LOG_DEBUG, "Whisper recv from=" << sender);
			while (recv.available())
				recv.skip_byte();
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(line.c_str(), UIChatBar::MessageType::YELLOW);
			return;
		}
		if ((flag & F_WHISPER) && (flag & F_RESULT))
		{
			std::string target = recv.read_string();
			bool ok = recv.available() ? recv.read_bool() : false;
			std::string line = ok
				? ("Whisper sent to " + target)
				: ("Unable to whisper " + target);
			while (recv.available())
				recv.skip_byte();
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(line.c_str(), ok ? UIChatBar::MessageType::WHITE : UIChatBar::MessageType::RED);
			return;
		}
		if (((flag & F_LOCATION) || (flag & F_LOCATION_FRIEND)) && (flag & F_RESULT))
		{
			std::string target = recv.read_string();
			int8_t type = recv.available() ? recv.read_byte() : 0;
			int32_t field_or_ch = recv.available() >= 4 ? recv.read_int() : 0;
			if (type == 1 && recv.available() >= 8)
			{
				recv.read_int();
				recv.read_int();
			}
			std::string line = "Find " + target + " type=" + std::to_string(type)
				+ " @" + std::to_string(field_or_ch);
			while (recv.available())
				recv.skip_byte();
			// BuddyList 无 find tip API：始终 ChatBar 显结果
			if (auto buddy = UI::get().get_element<UIBuddyList>())
			{
				if (!buddy->is_active())
					buddy->makeactive();
			}
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(line.c_str(), UIChatBar::MessageType::WHITE);
			return;
		}
		LOG(LOG_DEBUG, "Whisper unhandled flag=0x" << std::hex << (int)(uint8_t)flag << std::dec);
		while (recv.available())
			recv.skip_byte();
	}

	void SpouseChatHandler::handle(InPacket& recv) const
	{
		int8_t kind = recv.available() ? recv.read_byte() : 0;
		std::string name;
		if (kind == 5)
			name = recv.available() ? recv.read_string() : "";
		if (recv.available())
			recv.read_byte();
		std::string text = recv.available() ? recv.read_string() : "";
		std::string line = name.empty() ? text : (name + " : " + text);
		LOG(LOG_DEBUG, "SpouseChat kind=" << (int)kind);
		while (recv.available())
			recv.skip_byte();
		if (line.empty())
			return;
		auto ui = UI::get().get_element<UIWedding>();
		if (!ui) ui = UI::get().emplace<UIWedding>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
		{
			ui->set_invitation(false, line);
			return;
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(line.c_str(), UIChatBar::MessageType::YELLOW);
	}

	void ScriptProgressMessageHandler::handle(InPacket& recv) const
	{
		std::string msg = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "ScriptProgress len=" << msg.size());
		while (recv.available())
			recv.skip_byte();
		if (msg.empty())
			return;
		auto alarm = UI::get().get_element<UIQuestAlarm>();
		if (!alarm) alarm = UI::get().emplace<UIQuestAlarm>();
		else if (!alarm->is_active()) alarm->makeactive();
		if (alarm)
		{
			alarm->set_tip(msg);
			return;
		}
		if (auto ql = UI::get().get_element<UIQuestLog>())
		{
			if (ql->is_active())
			{
				ql->refresh();
				return;
			}
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(msg.c_str(), UIChatBar::MessageType::YELLOW);
	}

	void FameResponseHandler::handle(InPacket& recv) const
	{
		int8_t status = recv.available() ? recv.read_byte() : 0;
		std::string line;
		if (status == 0)
		{
			std::string name = recv.available() ? recv.read_string() : "";
			int8_t mode = recv.available() ? recv.read_byte() : 0;
			int16_t newfame = recv.available() >= 2 ? recv.read_short() : 0;
			line = std::string(mode ? "Raised" : "Dropped") + " fame of " + name
				+ " (now " + std::to_string(newfame) + ")";
		}
		else if (status == 5)
		{
			std::string from = recv.available() ? recv.read_string() : "";
			int8_t mode = recv.available() ? recv.read_byte() : 0;
			line = from + (mode ? " raised" : " dropped") + " your fame";
		}
		else
		{
			static const char* errs[] = {
				"ok", "Invalid name", "Level too low", "Daily fame limit",
				"Monthly fame limit", "received", "Unexpected fame error"
			};
			line = (status >= 0 && status <= 6) ? errs[status] : "Fame error";
		}
		LOG(LOG_DEBUG, "FameResponse status=" << (int)status);
		while (recv.available())
			recv.skip_byte();
		// CharInfo ensure：刷新人气，不 tip
		if (status == 0 || status == 5)
		{
			int32_t self = Stage::get().get_player().get_oid();
			auto ci = UI::get().get_element<UICharInfo>();
			if (!ci) ci = UI::get().emplace<UICharInfo>(self);
			else if (!ci->is_active()) ci->makeactive();
			if (ci)
			{
				CharInfoRequestPacket(self).dispatch();
				return;
			}
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(line.c_str(), UIChatBar::MessageType::YELLOW);
	}

	void NotifyLevelUpHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.available() ? recv.read_byte() : 0;
		int32_t level = recv.available() >= 4 ? recv.read_int() : 0;
		std::string name = recv.available() ? recv.read_string() : "";
		const char* tag = (type == 1) ? "Family" : (type == 2) ? "Guild" : "Notify";
		std::string line = std::string("<") + tag + "> " + name + " reached Lv." + std::to_string(level);
		LOG(LOG_DEBUG, "NotifyLevelUp type=" << (int)type << " lv=" << level);
		while (recv.available())
			recv.skip_byte();
		if (type == 2)
		{
			auto ui = UI::get().get_element<UIGuild>();
			if (!ui) ui = UI::get().emplace<UIGuild>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui)
			{
				ui->set_notice(line);
				return;
			}
		}
		else if (type == 1)
		{
			auto ui = UI::get().get_element<UIFamily>();
			if (!ui) ui = UI::get().emplace<UIFamily>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui)
			{
				ui->set_privileges({ line });
				return;
			}
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(line.c_str(), UIChatBar::MessageType::YELLOW);
	}

	void NotifyMarriageHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.available() ? recv.read_byte() : 0;
		std::string name = recv.available() ? recv.read_string() : "";
		const char* tag = type ? "Family" : "Guild";
		std::string line = std::string("<") + tag + "> " + name + " is now married";
		LOG(LOG_DEBUG, "NotifyMarriage type=" << (int)type);
		while (recv.available())
			recv.skip_byte();
		auto wed = UI::get().get_element<UIWedding>();
		if (!wed) wed = UI::get().emplace<UIWedding>();
		else if (!wed->is_active()) wed->makeactive();
		if (wed)
		{
			wed->set_invitation(false, line);
			return;
		}
		if (type == 0)
		{
			auto ui = UI::get().get_element<UIGuild>();
			if (!ui) ui = UI::get().emplace<UIGuild>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui)
			{
				ui->set_notice(line);
				return;
			}
		}
		else
		{
			auto ui = UI::get().get_element<UIFamily>();
			if (!ui) ui = UI::get().emplace<UIFamily>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui)
			{
				ui->set_privileges({ line });
				return;
			}
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(line.c_str(), UIChatBar::MessageType::YELLOW);
	}

	void NotifyJobChangeHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.available() ? recv.read_byte() : 0;
		int32_t job = recv.available() >= 4 ? recv.read_int() : 0;
		std::string name = recv.available() ? recv.read_string() : "";
		const char* tag = type ? "Family" : "Guild";
		std::string line = std::string("<") + tag + "> " + name
			+ " advanced (job " + std::to_string(job) + ")";
		LOG(LOG_DEBUG, "NotifyJobChange type=" << (int)type << " job=" << job);
		while (recv.available())
			recv.skip_byte();
		if (type == 0)
		{
			auto ui = UI::get().get_element<UIGuild>();
			if (!ui) ui = UI::get().emplace<UIGuild>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui)
			{
				ui->set_notice(line);
				return;
			}
		}
		else
		{
			auto ui = UI::get().get_element<UIFamily>();
			if (!ui) ui = UI::get().emplace<UIFamily>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui)
			{
				ui->set_privileges({ line });
				return;
			}
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(line.c_str(), UIChatBar::MessageType::YELLOW);
	}
}