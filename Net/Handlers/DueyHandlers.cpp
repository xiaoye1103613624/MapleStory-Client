////////////////////////////////////////////////////////////////////////////////
//	PARCEL handler — 对照 s9 PacketCreator.sendDuey*
////////////////////////////////////////////////////////////////////////////////
#include "DueyHandlers.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIDelivery.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"
#include "../../Util/Misc.h"

#include <vector>
#include <string>

namespace ms
{
	namespace {
		void duey_sm(Color::Name color, const std::string& tip)
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
	static void skip_iteminfo_zp(InPacket& recv, int32_t& itemid_out)
	{
		uint8_t itemType = recv.read_byte();
		int32_t itemid = recv.read_int();
		itemid_out = itemid;
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
			recv.skip_short();
			recv.skip_string();
			recv.skip_short();
			if ((itemid / 10000 == 233) || (itemid / 10000 == 207))
				recv.skip(8);
		}
	}

	static Optional<UIDelivery> ensure_ui()
	{
		auto ui = UI::get().get_element<UIDelivery>();
		if (!ui) ui = UI::get().emplace<UIDelivery>();
		else if (!ui->is_active()) ui->makeactive();
		return ui;
	}

	void ParcelHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.read_byte();
		LOG(LOG_DEBUG, "ParcelHandler: mode=0x" << std::hex << static_cast<int>(mode) << std::dec);

		switch (mode)
		{
			case 0x08: // open + list
			{
				recv.read_byte(); // unk0
				uint8_t count = recv.read_byte();
				std::vector<DueyPackage> pkgs;
				pkgs.reserve(count);
				for (uint8_t i = 0; i < count; i++) {
					DueyPackage p;
					p.id = recv.read_int();
					p.sender = recv.read_padded_string(13);
					p.mesos = recv.read_int();
					recv.skip(8); // sentTimeFT
					int32_t hasMsg = recv.read_int();
					std::string msg = recv.read_padded_string(200);
					if (hasMsg)
						p.message = msg;
					recv.read_byte(); // unk1
					uint8_t hasItem = recv.read_byte();
					if (hasItem)
						skip_iteminfo_zp(recv, p.itemid);
					pkgs.push_back(std::move(p));
				}
				if (recv.available())
					recv.read_byte(); // trailer
				if (auto ui = ensure_ui())
					ui->set_packages(std::move(pkgs));
				while (recv.available())
					recv.skip_byte();
				// Delivery 已刷，不 tip
				(void)count;
				break;
			}
			case 0x1A: // quick-ticket open (send focus, no list)
			{
				while (recv.available())
					recv.skip_byte();
				if (auto ui = ensure_ui()) {
					ui->set_packages({});
					ui->set_tab(0);
					ui->show_notice("Quick delivery — use Send tab");
				}
				break;
			}
			case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D:
			case 0x0E: case 0x0F: case 0x10: case 0x11: case 0x12:
			case 0x13: case 0x14: case 0x15: case 0x16:
			{
				static const char* msgs[] = {
					"", "", "", "", "", "", "", "", "",
					"Actions enabled", "Not enough mesos", "Incorrect request",
					"Name not found", "Same account", "Err0E", "Err0F", "Err10", "Err11",
					"Sent!", "Claim error", "Recv enable", "No free slots", "Unique conflict"
				};
				std::string tip = (mode < 0x17) ? msgs[mode] : "Duey notice";
				while (recv.available())
					recv.skip_byte();
				if (auto ui = ensure_ui())
				{
					ui->show_notice(tip);
					break;
				}
				duey_sm(mode == 0x12 ? Color::Name::YELLOW : Color::Name::WHITE, tip);
				break;
			}
			case 0x17: // remove package
			{
				int32_t pid = recv.read_int();
				uint8_t flag = recv.read_byte();
				while (recv.available())
					recv.skip_byte();
				if (auto ui = ensure_ui())
				{
					ui->remove_package(pid);
					LOG(LOG_DEBUG, "ParcelHandler: remove id=" << pid << " flag=" << static_cast<int>(flag));
					break;
				}
				duey_sm(Color::Name::WHITE, "Package removed");
				LOG(LOG_DEBUG, "ParcelHandler: remove id=" << pid << " flag=" << static_cast<int>(flag));
				(void)flag;
				break;
			}
			case 0x19: // parcel received notify
			{
				std::string from = recv.read_string();
				bool quick = recv.read_bool();
				std::string tip = from + (quick ? " sent you a quick package." : " sent you a package.");
				while (recv.available())
					recv.skip_byte();
				if (auto ui = ensure_ui())
				{
					ui->show_notice(tip);
					break;
				}
				duey_sm(Color::Name::YELLOW, tip);
				break;
			}
			case 0x1B: // login unchecked
			{
				recv.read_bool();
				while (recv.available())
					recv.skip_byte();
				if (auto ui = ensure_ui())
				{
					ui->show_notice("You have unchecked Duey packages.");
					break;
				}
				duey_sm(Color::Name::YELLOW, "You have unchecked Duey packages.");
				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "ParcelHandler: unhandled mode=" << static_cast<int>(mode)
					<< " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
				std::string tip = "Duey mode " + std::to_string(mode);
				if (auto ui = ensure_ui())
				{
					ui->show_notice(tip);
					break;
				}
				duey_sm(Color::Name::YELLOW, tip);
				break;
			}
		}
	}
}
