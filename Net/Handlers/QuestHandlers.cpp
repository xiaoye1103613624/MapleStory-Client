//////////////////////////////////////////////////////////////////////////////////
//	QuestHandlers
//////////////////////////////////////////////////////////////////////////////////
#include "QuestHandlers.h"

#include "../../Data/QuestData.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIQuestLog.h"
#include "../../IO/UITypes/UIStatusMessenger.h"

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
			if (auto messenger = UI::get().get_element<UIStatusMessenger>())
				messenger->show_status(color, message);
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
					notify(Color::Name::YELLOW, "Quest time limit: " + qdata.get_name());
				}

				break;
			}
			case 7: // time limit remove: short pos, short quest
			{
				recv.read_short();
				recv.read_short();
				break;
			}
			case 8: // finish notify: short quest, int npc, int|short trailing
			{
				int16_t qid = recv.read_short();
				recv.read_int(); // npc

				if (recv.length() >= 4)
					recv.read_int();
				else if (recv.length() >= 2)
					recv.read_short();

				QuestLog& quests = Stage::get().get_player().get_quests();
				quests.set_completed(qid, 0);

				const QuestData& qdata = QuestData::get(qid);
				notify(Color::Name::YELLOW, "Quest complete: " + qdata.get_name());
				refresh_questlog_ui();
				break;
			}
			case 0x0A: // quest error
			{
				int16_t qid = recv.read_short();
				const QuestData& qdata = QuestData::get(qid);
				notify(Color::Name::RED, "Quest error: " + qdata.get_name());
				break;
			}
			case 0x0F: // quest expired
			{
				int16_t qid = recv.read_short();
				Stage::get().get_player().get_quests().remove_started(qid);

				const QuestData& qdata = QuestData::get(qid);
				notify(Color::Name::RED, "Quest expired: " + qdata.get_name());
				refresh_questlog_ui();
				break;
			}
			default:
				// Unknown mode — leave remaining bytes unread; do not crash.
				break;
		}
	}
}
