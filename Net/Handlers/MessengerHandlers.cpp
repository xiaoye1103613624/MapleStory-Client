////////////////////////////////////////////////////////////////////////////////
//	Messenger 入站 handler（313）
////////////////////////////////////////////////////////////////////////////////
#include "MessengerHandlers.h"

#include "Helpers/LoginParser.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIMessenger.h"
#include "../../IO/UITypes/UINotice.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"
#include "../../Net/Packets/MessengerPackets.h"
#include "../../Util/Misc.h"

#include <string>

namespace ms
{
	// 仅保证窗体存在；不调用 try_create_room（邀请进房已由 Open(id) 处理）。
	static Optional<UIMessenger> ensure_ui()
	{
		auto ui = UI::get().get_element<UIMessenger>();

		if (!ui)
			ui = UI::get().emplace<UIMessenger>();

		if (ui && !ui->is_active())
			ui->makeactive();

		return ui;
	}

	void MessengerHandler::handle(InPacket& recv) const
	{
		auto ensure_sm = [](Color::Name color, const std::string& tip) {
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), mt);
				return;
			}
		};

		uint8_t mode = recv.read_byte();

		LOG(LOG_DEBUG, "MessengerHandler: mode=" << static_cast<int>(mode));

		switch (mode)
		{
			case 0x00: // addMessengerPlayer
			case 0x07: // updateMessengerPlayer
			{
				int8_t pos = recv.read_byte();
				LookEntry look = LoginParser::parse_look(recv);
				std::string name = recv.read_string();
				int8_t channel = recv.read_byte();
				recv.read_byte(); // pad

				if (auto ui = ensure_ui())
					ui->set_member(pos, name, channel, look);
				while (recv.available())
					recv.skip_byte();
				// 立绘座位已刷，不 tip
				break;
			}
			case 0x01: // joinMessenger — 本机座位
			{
				int8_t pos = recv.read_byte();
				while (recv.available())
					recv.skip_byte();

				if (auto ui = ensure_ui())
					ui->set_local_pos(pos);
				(void)pos;
				break;
			}
			case 0x02: // removeMessengerPlayer
			{
				int8_t pos = recv.read_byte();
				while (recv.available())
					recv.skip_byte();

				if (auto ui = ensure_ui())
					ui->clear_member(pos);
				(void)pos;
				break;
			}
			case 0x03: // messengerInvite
			{
				std::string from = recv.read_string();
				recv.read_byte();
				int32_t messengerid = recv.read_int();
				recv.read_byte();
				while (recv.available())
					recv.skip_byte();

				std::string msg = from + " has invited you to Maple Messenger.";
				// YesNo 已显，不重复 SM

				UI::get().emplace<UIYesNo>(msg, [from, messengerid](bool yes)
				{
					if (yes)
					{
						MessengerOpenPacket(messengerid).dispatch();
						ensure_ui();
					}
					else
					{
						MessengerDeclinePacket(from).dispatch();
					}
				});

				break;
			}
			case 0x04: // invite result note (mode2: 1=ok, 0=fail/offline)
			case 0x05: // decline note
			{
				std::string name = recv.read_string();
				int8_t mode2 = recv.read_byte();
				while (recv.available())
					recv.skip_byte();

				std::string tip;

				if (mode == 0x04)
					tip = (mode2 == 1)
						? (name + " has been invited.")
						: (name + " is currently unavailable.");
				else
					tip = name + " declined your invitation.";

				if (auto ui = ensure_ui())
				{
					ui->append_chat(tip);
					break;
				}
				ensure_sm(Color::Name::WHITE, tip);

				break;
			}
			case 0x06: // messengerChat（真聊天 "Name : msg" 或打字指示 Name+"0"|"1"）
			{
				std::string text = recv.read_string();
				while (recv.available())
					recv.skip_byte();

				if (auto ui = ensure_ui())
				{
					ui->handle_messenger_chat(text);
					break;
				}
				// 无窗时：打字指示丢弃；真聊天仍走 StatusMessenger
				if (!text.empty() && text.find(" : ") != std::string::npos)
					ensure_sm(Color::Name::YELLOW, text);

				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "MessengerHandler: unhandled mode=" << static_cast<int>(mode)
					<< " rem=" << recv.available());
				std::string extra = recv.available() ? recv.read_string() : "";
				while (recv.available())
					recv.skip_byte();
				std::string tip = extra.empty()
					? ("Messenger mode " + std::to_string(mode))
					: extra;
				if (auto ui = ensure_ui())
				{
					ui->append_chat(tip);
					break;
				}
				ensure_sm(Color::Name::YELLOW, tip);
				break;
			}
		}
	}
}
