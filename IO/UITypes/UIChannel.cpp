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
#include "UIChannel.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../KeyAction.h"

#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"
#include "../../Configuration.h"
#include "../../Net/Handlers/Helpers/LoginParser.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIChannel::UIChannel() : UIDragElement<PosCHANNEL>()
	{
		current_channel = Configuration::get().get_channelid();

		// 频道数来自登录期 SERVERLIST 的实测值（本服 Scania=3，见 channel-layout.md §6.2），
		// 不是猜的 20 —— 原生 频道变更.png 只有 6 格 1 行，正是因为那个世界只有 3 个频道。
		// 兜底：断线重连/换频道回本图时如果这一轮没解过 SERVERLIST，退成「至少盖住当前频道」。
		channel_count = Configuration::get().get_channel_total(Configuration::get().get_worldid());

		if (channel_count < static_cast<uint8_t>(current_channel + 1))
			channel_count = static_cast<uint8_t>(current_channel + 1);

		selected_channel = current_channel;

		nl::node Channel = UiAssetResolve::window("Channel");

		// UIWindow.img/Channel 根本没有 backgrnd —— 这张脸是 t(标题+世界条) + 槽行 + s(按钮条) 三段拼的。
		// 旧门槛只认 backgrnd，缺件即 active=false，于是「点频道更换」什么也不出（channel-layout.md §0）。
		nl::node t = Channel["t"];
		nl::node s = Channel["s"];

		if (!UiAssetResolve::usable(t) || !UiAssetResolve::usable(s))
		{
			UiAssetResolve::diag_once("UIChannel", "Channel/t or Channel/s missing -> window suppressed");
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		sprites.emplace_back(t);

		plate_empty = Channel["channel0"];
		plate_selected = Channel["channel1"];
		plate_current = Channel["channel2"];

		uint8_t world = Configuration::get().get_worldid();

		if (Channel["world"][world])
			sprites.emplace_back(Channel["world"][world], Point<int16_t>(16, 31));

		for (uint8_t i = 0; i < slot_total(); i++)
		{
			if (i < channel_count && Channel["ch"][i])
				ch.emplace_back(Channel["ch"][i],
					Point<int16_t>(slot_x(i) + SLOT_LABEL_DX, slot_y(i) + SLOT_LABEL_DY));

			buttons[Buttons::CH + i] = std::make_unique<AreaButton>(
				Point<int16_t>(slot_x(i), slot_y(i)), Point<int16_t>(68, 20));
		}

		// 两颗橙钮在 s 条带里，顶边距条带顶 12（帧实测 SAD=0.07）。
		try_maple_btn(buttons, Buttons::CHANGE, Channel["BtChange"],
			Point<int16_t>(286, bottom_strip_y() + 12), "UIChannel BtChange");
		try_maple_btn(buttons, Buttons::CANCEL, Channel["BtCancel"],
			Point<int16_t>(380, bottom_strip_y() + 12), "UIChannel BtCancel");

		sprites.emplace_back(s, Point<int16_t>(0, bottom_strip_y()));

		dimension = Point<int16_t>(436, bottom_strip_y() + 36);
		dragarea = Point<int16_t>(dimension.x(), 20);
	}

	int16_t UIChannel::slot_x(uint8_t i)
	{
		return static_cast<int16_t>(9 + SLOT_PITCH_X * (i % SLOTS_PER_ROW));
	}

	int16_t UIChannel::slot_y(uint8_t i)
	{
		return static_cast<int16_t>(GRID_TOP + SLOT_PITCH_Y * (i / SLOTS_PER_ROW));
	}

	size_t UIChannel::slot_total() const
	{
		// 最后一排补满空格：原生帧 3 个带字槽后面跟着 3 块纯灰板（channel0 SAD=0.00）。
		return (static_cast<size_t>(channel_count) + SLOTS_PER_ROW - 1) / SLOTS_PER_ROW * SLOTS_PER_ROW;
	}

	int16_t UIChannel::bottom_strip_y() const
	{
		return static_cast<int16_t>(GRID_TOP + SLOT_PITCH_Y * (slot_total() / SLOTS_PER_ROW));
	}

	void UIChannel::draw(float inter) const
	{
		UIElement::draw(inter);

		for (uint8_t i = 0; i < slot_total(); i++)
		{
			const Texture* plate = &plate_empty;

			if (i == current_channel)
				plate = &plate_current;
			else if (i == selected_channel)
				plate = &plate_selected;

			if (plate->is_valid())
				plate->draw(DrawArgument(position.x() + slot_x(i), position.y() + slot_y(i)));
		}

		// Sprite::draw 非 const，所以这里只能按值拷贝（draw() 本身是 const）。
		for (Sprite sprite : ch)
			sprite.draw(position, inter);
	}

	void UIChannel::update()
	{
		UIElement::update();

		// 本窗体是常驻单例（get_element / emplace 复用同一个实例），ctor 里抓的
		// current_channel 会一直停在登录时选的那条。UIStateGame::update 只对
		// is_active() 的窗体派发，所以这里正好是「每次打开时刷新」的落点。
		// 换频成功后 SET_FIELD 已把 Configuration::channelid 改成新频道，
		// 不在这里重取的话当前格永远亮在旧频道上。
		current_channel = Configuration::get().get_channelid();

		if (selected_channel >= channel_count)
			selected_channel = current_channel;

		for (Sprite& sprite : ch)
			sprite.update();
	}

	void UIChannel::step_channel(uint8_t& target, int delta)
	{
		int total = static_cast<int>(channel_count);

		if (total <= 1)
			return;

		int v = static_cast<int>(target) + delta;

		// 越界绕到另一端
		if (v < 0)
			v += total;
		else if (v >= total)
			v -= total;

		// 当前所在频道不可选（选中它等于不换）
		if (v == static_cast<int>(current_channel))
		{
			v += (delta > 0) ? 1 : -1;

			if (v < 0)
				v = total - 1;
			else if (v >= total)
				v = 0;
		}

		target = static_cast<uint8_t>(v);
	}

	void UIChannel::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				cancel();
			}
			else if (keycode == KeyAction::Id::RETURN)
			{
				change_channel();
			}
			else if (keycode == KeyAction::Id::UP)
			{
				step_channel(selected_channel, -static_cast<int>(SLOTS_PER_ROW));
			}
			else if (keycode == KeyAction::Id::DOWN)
			{
				step_channel(selected_channel, static_cast<int>(SLOTS_PER_ROW));
			}
			else if (keycode == KeyAction::Id::LEFT)
			{
				step_channel(selected_channel, -1);
			}
			else if (keycode == KeyAction::Id::RIGHT)
			{
				step_channel(selected_channel, 1);
			}
			else if (keycode == 'C' || keycode == 'c')
			{
				// SoftKey 频道号（1-based 显示 → 0-based packet）
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					try {
						int ch = std::stoi(s);
						if (ch >= 1 && ch <= static_cast<int>(channel_count)) {
							selected_channel = static_cast<uint8_t>(ch - 1);
							change_channel();
							LOG(LOG_DEBUG, "UIChannel: SoftKey ch=" << ch);
						}
					} catch (...) {}
				}, []() {}, "Channel 1-N");
			}
			else if (keycode == 'G' || keycode == 'g' || keycode == 13)
			{
				change_channel();
				LOG(LOG_DEBUG, "UIChannel: Go ch=" << (selected_channel + 1));
			}
		}
	}

	Cursor::State UIChannel::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(clicked, cursorpos);

		if (dragged)
			return dstate;

		Cursor::State ret = clicked ? Cursor::State::CLICKING : Cursor::State::IDLE;

		for (size_t i = 0; i < Buttons::CH + slot_total(); i++)
		{
			auto it = buttons.find(i);

			if (it == buttons.end() || !it->second)
				continue;

			Button& button = *it->second;

			if (button.is_active() && button.bounds(position).contains(cursorpos))
			{
				if (button.get_state() == Button::State::NORMAL)
				{
					if (i < Buttons::CH)
					{
						Sound(Sound::Name::BUTTONOVER).play();

						button.set_state(Button::State::MOUSEOVER);
						ret = Cursor::State::CANCLICK;
					}
					else
					{
						button.set_state(Button::State::MOUSEOVER);
						ret = Cursor::State::IDLE;
					}
				}
				else if (button.get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						if (i < Buttons::CH)
							Sound(Sound::Name::BUTTONCLICK).play();

						button.set_state(button_pressed(i));

						ret = Cursor::State::IDLE;
					}
					else
					{
						if (i < Buttons::CH)
							ret = Cursor::State::CANCLICK;
						else
							ret = Cursor::State::IDLE;
					}
				}
			}
			else if (button.get_state() == Button::State::MOUSEOVER)
			{
				button.set_state(Button::State::NORMAL);
			}
		}

		return ret;
	}

	UIElement::Type UIChannel::get_type() const
	{
		return TYPE;
	}

	Button::State UIChannel::button_pressed(uint16_t buttonid)
	{
		if (buttonid < Buttons::CH)
		{
			switch (buttonid)
			{
				case Buttons::CANCEL:
					cancel();
					break;
				case Buttons::CHANGE:
					change_channel();
					break;
				default:
					break;
			}
		}
		else
		{
			if (buttonid - Buttons::CH == current_channel)
				return Button::State::NORMAL;

			selected_channel = static_cast<uint8_t>(buttonid - Buttons::CH);
		}

		return Button::State::NORMAL;
	}

	void UIChannel::cancel()
	{
		deactivate();

		current_channel = Configuration::get().get_channelid();
		selected_channel = current_channel;
	}

	void UIChannel::change_channel()
	{
		if (selected_channel == current_channel)
		{
			cancel();
			return;
		}

		// CHANGE_CHANNEL 的回包只给新频道服的 IP/port，不带 charId；频道服的
		// 入场券是 PLAYER_LOGIN(charId)。挂上标记，parse_login 重连成功后补发。
		LoginParser::expect_channel_reentry();

		ChangeChannelPacket(static_cast<uint8_t>(selected_channel)).dispatch();
		LOG(LOG_DEBUG, "UIChannel: ChangeChannel ch=" << static_cast<int>(selected_channel));
		deactivate();
	}
}