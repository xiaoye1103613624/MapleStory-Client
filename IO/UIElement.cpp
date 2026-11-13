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

#include "UI.h"

#include "../Audio/Audio.h"
#include "../Constants.h"
#include "../Graphics/SpecialText.h"
#include "../Graphics/Text.h"
#include "../MapleStory.h"
#include "../Util/HudLayout.h"
#include "../Util/Locale.h"

namespace ms
{
	UIElement::UIElement(Point<int16_t> p, Point<int16_t> d, bool a) : position(p), dimension(d), active(a)
	{
		// D3：首开也淡入 —— 新窗构造即 active=true，不走 makeactive()，
		// 所以初值 0 由第一次 draw_sprites 起推进（约 9 帧收敛）。
		fade_in = 0.0f;
	}
	UIElement::UIElement(Point<int16_t> p, Point<int16_t> d) : UIElement(p, d, true) {}
	UIElement::UIElement() : UIElement(Point<int16_t>(), Point<int16_t>()) {}

	void UIElement::set_layout_policy(LayoutPolicy policy)
	{
		layout_policy = policy;
	}

	void UIElement::set_layout_edge(LayoutEdge edge, Point<int16_t> margin)
	{
		layout_policy = LayoutPolicy::Edge;
		layout_edge = edge;
		layout_margin = margin;
	}

	void UIElement::center_on_screen()
	{
		const int16_t vieww = Constants::Constants::get().get_viewwidth();
		const int16_t viewh = Constants::Constants::get().get_viewheight();

		position = HudLayout::center_on_screen(vieww, viewh, dimension);
	}

	void UIElement::update_screen(int16_t new_width, int16_t new_height)
	{
		// 像素还原 / 多分辨率：Center 与 Edge 重锚后一律 clamp，保证 UI 不画出客户区
		//（见 docs/features/pixel-ui-angel-restore-2026-11/notes/clamp-policy.md）。
		switch (layout_policy)
		{
			case LayoutPolicy::Center:
				position = HudLayout::center_on_screen(new_width, new_height, dimension);
				position = HudLayout::clamp_to_view(position, dimension, new_width, new_height);
				break;

			case LayoutPolicy::SavedPosClamped:
				position = HudLayout::clamp_to_view(position, dimension, new_width, new_height);
				break;

			case LayoutPolicy::HudGroup:
				// y 贴底公式留给 StatusBar/ChatBar 子类；此处只同步水平组偏移（默认 QS26 content_w）。
				position = Point<int16_t>(HudLayout::hud_center_x(new_width), position.y());
				break;

			case LayoutPolicy::Edge:
			{
				const int16_t mx = layout_margin.x();
				const int16_t my = layout_margin.y();
				int16_t x = position.x();
				int16_t y = position.y();

				switch (layout_edge)
				{
					case LayoutEdge::Left:
						x = HudLayout::edge_left(new_width, mx);
						break;
					case LayoutEdge::Right:
						x = HudLayout::edge_right(new_width, dimension.x(), mx);
						break;
					case LayoutEdge::Top:
						y = HudLayout::edge_top(new_height, my);
						break;
					case LayoutEdge::Bottom:
						y = HudLayout::edge_bottom(new_height, dimension.y(), my);
						break;
					case LayoutEdge::TopLeft:
						x = HudLayout::edge_left(new_width, mx);
						y = HudLayout::edge_top(new_height, my);
						break;
					case LayoutEdge::TopRight:
						x = HudLayout::edge_right(new_width, dimension.x(), mx);
						y = HudLayout::edge_top(new_height, my);
						break;
					case LayoutEdge::BottomLeft:
						x = HudLayout::edge_left(new_width, mx);
						y = HudLayout::edge_bottom(new_height, dimension.y(), my);
						break;
					case LayoutEdge::BottomRight:
						x = HudLayout::edge_right(new_width, dimension.x(), mx);
						y = HudLayout::edge_bottom(new_height, dimension.y(), my);
						break;
				}

				position = HudLayout::clamp_to_view(
					Point<int16_t>(x, y), dimension, new_width, new_height);
				break;
			}

			case LayoutPolicy::None:
			default:
				break;
		}
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
		locale_binds.bind(target, std::move(key));
	}

	void UIElement::bind_text(Text& target, std::string key, std::string value)
	{
		locale_binds.bind(target, std::move(key), std::move(value));
	}

	void UIElement::bind_text(OutlinedText& target, std::string key)
	{
		locale_binds.bind(target, std::move(key));
	}

	void UIElement::bind_text(OutlinedText& target, std::string key, std::string value)
	{
		locale_binds.bind(target, std::move(key), std::move(value));
	}

	void UIElement::refresh_localized()
	{
		if (!locale_binds.refresh())
			return;

		// 语言切换是低频运维事件，这条是上线级可观测点：真机上「改 xy-config 的
		// ui.language -> 已开着的窗立刻换文案」只有靠它才数得清到底哪些窗重取过，
		// 不必靠给某个键临时编一条两种语言不同的假文案来验机制。
		LOG(LOG_INFO, "Locale refresh gen=" << Locale::get().generation() << " rebound "
			<< locale_binds.count() << " labels (UIElement type " << get_type() << ")");
	}

	void UIElement::draw(float alpha) const
	{
		draw_sprites(alpha);
		draw_buttons(alpha);
	}

	void UIElement::draw_sprites(float alpha) const
	{
		// D3：UI_Toggle 视觉层 —— 开窗淡入乘进 sprite alpha。推进点同时放这里与
		// draw_buttons：任何消费路径（sprites 或 buttons）都保证同帧推进，
		// 不会出现"只消费不推进卡在全透明"。双消费窗每帧 +2×0.111 ≈ 4-5 帧收敛，
		// 单消费窗 ≈ 9 帧（~150ms），与原生 UI_Toggle 开关观感同量级。
		if (fade_in < 1.0f)
		{
			fade_in += 0.111f;
			if (fade_in > 1.0f)
				fade_in = 1.0f;
		}

		for (const Sprite& sprite : sprites)
			sprite.draw(position, alpha * fade_in);
	}

	void UIElement::draw_buttons(float) const
	{
		// D3：推进（见 draw_sprites 注释）；buttons 自身无 alpha 参数，不淡入。
		if (fade_in < 1.0f)
		{
			fade_in += 0.111f;
			if (fade_in > 1.0f)
				fade_in = 1.0f;
		}

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
		// 激活即置顶。旧实现只翻 active，不重排绘制队列，于是「关过再开」的非 toggled 窗
		//（目录 / UIGameMenu，TOGGLED=false）会留在它上次的位置上，被其后打开的窗盖住。
		// pre_add 只对 is_toggled 的元素抬层（IO/UIStateGame.cpp:757），补不到这一类。
		const bool was_active = active;

		active = true;

		// 像素还原：开窗瞬间按当前策略夹回视口，避免上次拖到屏外或换分辨率后露边。
		if (layout_policy != LayoutPolicy::None)
		{
			update_screen(
				Constants::Constants::get().get_viewwidth(),
				Constants::Constants::get().get_viewheight());
		}

		// D3：重新开窗重新淡入（原生 UI_Toggle 每次 OPEN 都走 MENUUP 视觉层）。
		if (!was_active)
		{
			fade_in = 0.0f;
			UI::get().bring_to_front(get_type());
		}
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