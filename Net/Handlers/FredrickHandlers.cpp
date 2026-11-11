////////////////////////////////////////////////////////////////////////////////
//	FREDRICK / FREDRICK_MESSAGE — 对照 s9 PacketCreator.getFredrick / fredrickMessage
////////////////////////////////////////////////////////////////////////////////
#include "FredrickHandlers.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIStoreBank.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"
#include "../../Util/Misc.h"

#include <vector>
#include <string>

namespace ms
{
	namespace {
		void fr_sm(Color::Name color, const std::string& tip)
		{
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), mt);
				return;
			}
		}
	}
	static void skip_iteminfo_zp(InPacket& recv, int32_t& itemid_out, int16_t& qty_out)
	{
		uint8_t itemType = recv.read_byte();
		int32_t itemid = recv.read_int();
		itemid_out = itemid;
		qty_out = 1;

		bool isCash = recv.read_bool();
		if (isCash)
			recv.skip(8);
		recv.skip(8);

		if (itemType == 1) {
			recv.skip(2);
			recv.skip(30);
			recv.skip_string();
			recv.skip_short();
			if (isCash) recv.skip(10);
			else recv.skip(18);
			recv.skip(8);
			recv.skip(4);
		} else if (itemid >= 5000000 && itemid <= 5000102) {
			recv.skip(37);
		} else {
			qty_out = recv.read_short();
			recv.skip_string();
			recv.skip_short();
			if ((itemid / 10000 == 233) || (itemid / 10000 == 207))
				recv.skip(8);
		}
	}

	static Optional<UIStoreBank> ensure_ui()
	{
		auto ui = UI::get().get_element<UIStoreBank>();
		if (!ui) ui = UI::get().emplace<UIStoreBank>();
		else if (!ui->is_active()) ui->makeactive();
		return ui;
	}

	void FredrickHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.read_byte();
		LOG(LOG_DEBUG, "FredrickHandler: mode=0x" << std::hex << static_cast<int>(mode) << std::dec);

		switch (mode)
		{
			case 0x23: // open dump
			{
				recv.read_int(); // npcId
				recv.read_int(); // opaque 32272
				recv.skip(5);
				int64_t mesos = recv.read_long();
				recv.skip(1);
				uint8_t count = recv.read_byte();

				std::vector<FredrickItem> items;
				items.reserve(count);
				for (uint8_t i = 0; i < count && recv.available(); i++) {
					FredrickItem it;
					skip_iteminfo_zp(recv, it.itemid, it.quantity);
					items.push_back(it);
				}
				if (recv.available() >= 3)
					recv.skip(3);

				if (auto ui = ensure_ui())
					ui->load(mesos, std::move(items));
				while (recv.available())
					recv.skip_byte();
				// StoreBank 已刷，不 tip
				(void)count; (void)mesos;
				break;
			}
			case 0x24:
			{
				recv.skip(8);
				while (recv.available())
					recv.skip_byte();
				if (auto ui = ensure_ui())
				{
					ui->show_notice("Fredrick updated");
					break;
				}
				fr_sm(Color::Name::YELLOW, "Fredrick updated");
				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "Fredrick mode=" << static_cast<int>(mode)
					<< " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
				std::string tip = "Fredrick mode 0x" + std::to_string(mode);
				if (auto ui = ensure_ui())
				{
					ui->show_notice(tip);
					break;
				}
				fr_sm(Color::Name::YELLOW, tip);
				break;
			}
		}
	}

	void FredrickMessageHandler::handle(InPacket& recv) const
	{
		uint8_t code = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		LOG(LOG_DEBUG, "FredrickMessageHandler: code=0x" << std::hex << static_cast<int>(code) << std::dec);

		const char* tip = "Fredrick notice";
		switch (code) {
			case 0x1E: tip = "Retrieved!"; break;
			case 0x1F: tip = "Meso overflow"; break;
			case 0x20: tip = "Unique conflict"; break;
			case 0x21: tip = "Need meso to settle"; break;
			case 0x22: tip = "Inventory full"; break;
			default: break;
		}
		while (recv.available())
			recv.skip_byte();

		if (auto ui = ensure_ui()) {
			ui->show_notice(tip);
			if (code == 0x1E)
				ui->clear_and_close();
			return;
		}
		fr_sm(code == 0x1E ? Color::Name::YELLOW : Color::Name::RED, tip);
	}
}
