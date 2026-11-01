//////////////////////////////////////////////////////////////////////////////////
//	QuestHandlers
//////////////////////////////////////////////////////////////////////////////////
#include "QuestHandlers.h"

#include "../../Data/QuestData.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIQuestLog.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../IO/UITypes/UIMiscShells.h"

namespace ms
{
	namespace
	{
		void refresh_questlog_ui()
		{
			if (auto questlog = UI::get().get_element<UIQuestLog>())
				questlog->refresh();
		}

		void notify(Color::Name color, const std::string& message)
		{
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(message.c_str(), mt);
				return;
			}
		}

		void quest_alarm(const std::string& tip)
		{
			// QuestLog 已开：只刷 UI，不 tip（与 Messaging mode1 ql_open 一致）
			if (auto ql = UI::get().get_element<UIQuestLog>())
			{
				if (ql->is_active())
				{
					ql->refresh();
					return;
				}
			}
			// QuestAlarm ensure（对齐 ScriptProgress）
			auto ui = UI::get().get_element<UIQuestAlarm>();
			if (!ui) ui = UI::get().emplace<UIQuestAlarm>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui)
			{
				ui->set_tip(tip);
				return;
			}
			notify(Color::Name::YELLOW, tip);
		}
	}

	void UpdateQuestInfoHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.read_byte();

		switch (mode)
		{
			case 6: // time limit add: short count, (short quest, int time)*
			{
				int16_t count = recv.read_short();

				for (int16_t i = 0; i < count; i++)
				{
					int16_t qid = recv.read_short();
					recv.read_int(); // time remaining

					const QuestData& qdata = QuestData::get(qid);
					quest_alarm("⏱ " + qdata.get_name());
				}
				while (recv.available())
					recv.skip_byte();

				break;
			}
			case 7: // time limit remove: short pos, short quest
			{
				recv.read_short();
				recv.read_short();
				while (recv.available())
					recv.skip_byte();
				break;
			}
			case 8: // finish notify: short quest, int npc, int|short trailing
			{
				int16_t qid = recv.read_short();
				recv.read_int(); // npc

				if (recv.available() >= 4)
					recv.read_int();
				else if (recv.available() >= 2)
					recv.read_short();

				while (recv.available())
					recv.skip_byte();

				QuestLog& quests = Stage::get().get_player().get_quests();
				quests.set_completed(qid, 0);

				const QuestData& qdata = QuestData::get(qid);
				quest_alarm("✓ " + qdata.get_name());
				// quest_alarm 已 SM 或刷 Alarm；不重复 notify
				refresh_questlog_ui();
				break;
			}
			case 0x0A: // quest error
			{
				int16_t qid = recv.read_short();
				while (recv.available())
					recv.skip_byte();
				const QuestData& qdata = QuestData::get(qid);
				quest_alarm("Quest error: " + qdata.get_name());
				break;
			}
			case 0x0F: // quest expired
			{
				int16_t qid = recv.read_short();
				while (recv.available())
					recv.skip_byte();
				Stage::get().get_player().get_quests().remove_started(qid);

				const QuestData& qdata = QuestData::get(qid);
				quest_alarm("Quest expired: " + qdata.get_name());
				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "QuestInfo mode=" << static_cast<int>(mode)
					<< " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
				quest_alarm("Quest mode " + std::to_string(mode));
				break;
			}
		}
	}
}
