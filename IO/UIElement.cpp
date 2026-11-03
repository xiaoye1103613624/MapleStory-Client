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
#include "UIElement.h"

#include "../Audio/Audio.h"
#include "../Constants.h"
#include "../Graphics/SpecialText.h"
#include "../Graphics/Text.h"
#include "../MapleStory.h"
#include "../Util/Locale.h"

namespace ms
{
	UIElement::UIElement(Point<int16_t> p, Point<int16_t> d, bool a) : position(p), dimension(d), active(a) {}
	UIElement::UIElement(Point<int16_t> p, Point<int16_t> d) : UIElement(p, d, true) {}
	UIElement::UIElement() : UIElement(Point<int16_t>(), Point<int16_t>()) {}

	void UIElement::center_on_screen()
	{
		const int16_t vieww = Constants::Constants::get().get_viewwidth();
		const int16_t viewh = Constants::Constants::get().get_viewheight();

		position = Point<int16_t>(
			static_cast<int16_t>((vieww - dimension.x()) / 2),
			static_cast<int16_t>((viewh - dimension.y()) / 2)
		);
	}

	void UIElement::anchor_right_of(const UIElement& host, int16_t gap)
	{
		const int16_t vieww = Constants::Constants::get().get_viewwidth();
		const int16_t viewh = Constants::Constants::get().get_viewheight();

		int16_t x = static_cast<int16_t>(host.position.x() + host.dimension.x() + gap);
		int16_t y = host.position.y();

		if (static_cast<int32_t>(x) + dimension.x() > vieww)
			x = static_cast<int16_t>(host.position.x() - dimension.x() - gap);

		if (x < 0)
			x = 0;
		if (static_cast<int32_t>(y) + dimension.y() > viewh)
			y = static_cast<int16_t>(viewh - dimension.y());
		if (y < 0)
			y = 0;

		position = Point<int16_t>(x, y);
	}

	void UIElement::bind_text(Text& target, std::string key)
	{
		locale_binds.push_back({ &target, nullptr, std::move(key), {} });
		commit_bind();
	}

	void UIElement::bind_text(Text& target, std::string key, std::string value)
	{
		locale_binds.push_back({ &target, nullptr, std::move(key), std::move(value) });
		commit_bind();
	}

	void UIElement::bind_text(OutlinedText& target, std::string key)
	{
		locale_binds.push_back({ nullptr, &target, std::move(key), {} });
		commit_bind();
	}

	void UIElement::bind_text(OutlinedText& target, std::string key, std::string value)
	{
		locale_binds.push_back({ nullptr, &target, std::move(key), std::move(value) });
		commit_bind();
	}

	std::string UIElement::bind_text_of(const LocaleBind& bind)
	{
		return bind.value.empty()
			? Locale::text(bind.key)
			: Locale::format(bind.key, bind.value);
	}

	void UIElement::commit_bind()
	{
		const LocaleBind& bind = locale_binds.back();

		if (bind.target)
			bind.target->change_text(bind_text_of(bind));
		else
			bind.outlined->change_text(bind_text_of(bind));

		locale_gen = Locale::get().generation();
	}

	void UIElement::refresh_localized()
	{
		if (locale_binds.empty())
			return;

		const std::uint32_t gen = Locale::get().generation();

		if (gen == locale_gen)
			return;

		locale_gen = gen;

		for (const LocaleBind& bind : locale_binds)
		{
			const std::string text = bind_text_of(bind);

			if (bind.target)
				bind.target->change_text(text);
			else
				bind.outlined->change_text(text);
		}

		// 语言切换是低频运维事件，这条是上线级可观测点：真机上「改 xy-config 的
		// ui.language -> 已开着的窗立刻换文案」只有靠它才数得清到底哪些窗重取过，
		// 不必靠给某个键临时编一条两种语言不同的假文案来验机制。
		LOG(LOG_INFO, "Locale refresh gen=" << gen << " rebound "
			<< locale_binds.size() << " labels (UIElement type " << get_type() << ")");
	}

	void UIElement::draw(float alpha) const
	{
		draw_sprites(alpha);
		draw_buttons(alpha);
	}

	void UIElement::draw_sprites(float alpha) const
	{
		for (const Sprite& sprite : sprites)
			sprite.draw(position, alpha);
	}

	void UIElement::draw_buttons(float) const
	{
		for (auto& iter : buttons)
			if (const Button* button = iter.second.get())
				button->draw(position);
	}

	void UIElement::update()
	{
		for (auto& sprite : sprites)
			sprite.update();

		for (auto& iter : buttons)
			if (Button* button = iter.second.get())
				button->update();
	}

	void UIElement::makeactive()
	{
		active = true;
	}

	void UIElement::deactivate()
	{
		active = false;
	}

	bool UIElement::is_active() const
	{
		return active;
	}

	void UIElement::toggle_active()
	{
		if (active)
			deactivate();
		else
			makeactive();
	}

	bool UIElement::is_in_range(Point<int16_t> cursor_position) const
	{
		auto bounds = Rectangle<int16_t>(position, position + dimension);

		return bounds.contains(cursor_position);
	}

	void UIElement::remove_cursor()
	{
		// 防御：buttons map 中可能存在 operator[] 只读访问产生的空 unique_ptr 条目
		//（见 UIMiniMap::update_buttons 历史 bug），必须判空后再解引用。
		for (auto& btit : buttons)
		{
			auto button = btit.second.get();

			if (button && button->get_state() == Button::State::MOUSEOVER)
				button->set_state(Button::State::NORMAL);
		}
	}

	Cursor::State UIElement::send_cursor(bool clicked, Point<int16_t> cursor_position)
	{
		Cursor::State ret = clicked ? Cursor::State::CLICKING : Cursor::State::IDLE;

		for (auto& btit : buttons)
		{
			if (!btit.second)
				continue;

			if (btit.second->is_active() && btit.second->bounds(position).contains(cursor_position))
			{
				if (btit.second->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();

					btit.second->set_state(Button::State::MOUSEOVER);
					ret = Cursor::State::CANCLICK;
				}
				else if (btit.second->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btit.second->set_state(button_pressed(btit.first));

						ret = Cursor::State::IDLE;
					}
					else
					{
						ret = Cursor::State::CANCLICK;
					}
				}
			}
			else if (btit.second->get_state() == Button::State::MOUSEOVER)
			{
				btit.second->set_state(Button::State::NORMAL);
			}
		}

		return ret;
	}
}