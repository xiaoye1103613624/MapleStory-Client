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
////////////////////////////////////////////////////////////////////////////////
#include "UIWorldSelect.h"

#include "UILogin.h"
#include "UILoginNotice.h"
#include "UILoginWait.h"
#include "UIRegion.h"

#include "../UI.h"

#include "../LoginBackground.h"

#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"

#include "../../Graphics/Texture.h"
#include "../../Audio/Audio.h"
#include "../../Util/Randomizer.h"
#include "../../Constants.h"

#include "../../Net/Packets/LoginPackets.h"
#include "../../Net/Session.h"

#include <iostream>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 版式依据：gms083 IDA ?DrawWorldItems@CUIWorldSelect@@QAEXXZ (0x624e13)。
	// 世界列表为横向一行条幅：最多 30 槽，x 步进 27px；按钮节点为
	// UI/Login.img/WorldSelect/BtWorld/[worldId]，未开放的大区用 BtWorld/e 占位，
	// 世界名图为 WorldSelect/world/[worldId]，大区提示图标为 Login.img/WorldNotice/[worldId]。
	static constexpr int16_t WS_SLOT_STEP_X = 27;
	static constexpr int16_t WS_SLOT_COUNT = 30;

	// 世界按钮槽上限直接取自 UIWorldSelect::WORLD_BUTTON_COUNT
	// （Buttons 枚举的 BtChannel0 / BtGoWorld 均由它推导），不再单独写死数字，避免两处脱钩。
	static constexpr uint16_t WS_MAX_WORLDS = UIWorldSelect::WORLD_BUTTON_COUNT;

	// 越界告警限频标记：每类、每 id 在进程内只告警一次。
	// 世界数与 id 均由服务端下发，异常或恶意服务端可能持续触发，故不可无条件打印。
	static bool WS_BOUNDS_WARNED_WORLD[256] = { false };
	static bool WS_BOUNDS_WARNED_CHANNEL[256] = { false };

	/// <summary>
	/// 记录一次「按钮索引越界」告警（限频：同类同 id 只报一次）。
	/// 定级 WARN：属「服务端下发数据超出客户端容量」的可审计异常事件，
	/// 需在正式环境保留，便于定位服务端配置与客户端版本不匹配的问题。
	/// </summary>
	static void warn_button_index_out_of_range(bool* warned, uint16_t index, const char* kind, uint16_t limit)
	{
		if (index < 256)
		{
			if (warned[index])
				return;

			warned[index] = true;
		}

		// Log text stays ASCII for console readability; Chinese notes live in comments.
		std::cerr << "[UIWorldSelect][WARN] " << kind << " index=" << index
			<< " exceeds client button capacity " << limit << "; operation ignored. "
			<< "(server data does not match client capacity)" << std::endl;
	}

	/// <summary>
	/// 记录一次「服务端下发的频道数超出客户端容量」告警（进程内只报一次）。
	/// 与「索引越界」区分开，语义更明确：整段多余频道都会被忽略。
	/// </summary>
	static void warn_channel_count_out_of_range(uint16_t count)
	{
		static bool warned = false;

		if (warned)
			return;

		warned = true;

		std::cerr << "[UIWorldSelect][WARN] server channel count=" << count
			<< " exceeds client capacity " << UIWorldSelect::CHANNEL_BUTTON_COUNT
			<< "; extra channels ignored (only the first "
			<< UIWorldSelect::CHANNEL_BUTTON_COUNT << " take effect)." << std::endl;
	}

	// 频道面板 chBackgrn 尺寸 449x233（原点在中心），据此算左上角。
	static constexpr int16_t CH_PANEL_W = 449;
	static constexpr int16_t CH_PANEL_H = 233;

	// MapLogin WorldSelect/signboard/0 横杆；木牌挂在其底边绳扣下。
	static constexpr Point<int16_t> WS_POLE_MAP_POS(51, -888);
	static constexpr int16_t WS_BOARD_HANG_PAD = 4;

	// MapLogin WorldSelect/signboard/2 羊皮卷；频道面板顶边在其底边之下。
	static constexpr Point<int16_t> WS_SCROLL_MAP_POS(42, -753);
	static constexpr int16_t CH_BELOW_GAP = 8;

	// LoginBackground 不可用时的兜底（退化路径，按 1280x720 标定值）。
	static constexpr int16_t WS_BANNER_FALLBACK_Y = 111;
	static constexpr int16_t CH_PANEL_TOP_GAP = 34;

	// 选中后频道面板滑入。BeiDou 地图已有 signboard 卷轴，滑距过大易错位；缩短。
	static constexpr int16_t CH_SLIDE_DIST = 24;
	static constexpr float CH_SLIDE_MS = 180.0f;

	// 选中世界后 world/[id]（图标+名）相对 chBackgrn 中心：再往左上贴表头。
	static constexpr Point<int16_t> WS_WORLD_NAME_ON_PANEL(-222, -114);

	// BtGoWorld 140x41：相对面板中心，落在羊皮纸内右下（图2）。
	static constexpr int16_t WS_GOWORLD_W = 140;
	static constexpr int16_t WS_GOWORLD_H = 41;
	static constexpr Point<int16_t> WS_GOWORLD_OFF(
		CH_PANEL_W / 2 - WS_GOWORLD_W - 16,
		CH_PANEL_H / 2 - WS_GOWORLD_H - 12
	);

	// 左侧侧栏对照图2：整体下移、间距拉开（结束游戏独立在左上角）。
	static constexpr int16_t WS_SIDEBAR_X = 42;
	static constexpr int16_t WS_SIDEBAR_Y0 = 128;      // step「选择频道」
	static constexpr int16_t WS_SIDEBAR_AFTER_STEP = 20;
	static constexpr int16_t WS_SIDEBAR_GAP = 16;
	static constexpr Point<int16_t> WS_EXIT_POS(8, 8);

	// 公告底板相对 chBackgrn 中心：放在面板左侧。
	static constexpr int16_t WS_NOTICE_GAP = 12;

	// 频道按钮/负载条在频道面板内的网格偏移。
	static Point<int16_t> channel_cell_offset(uint16_t i)
	{
		constexpr int16_t COLS = 4;
		constexpr int16_t CELL_W = 100;
		constexpr int16_t CELL_H = 34;

		div_t d = std::div(static_cast<int>(i), static_cast<int>(COLS));

		return Point<int16_t>(22 + CELL_W * d.rem, 56 + CELL_H * d.quot);
	}

	UIWorldSelect::UIWorldSelect() : UIElement(
			Point<int16_t>(0, 0),
			Point<int16_t>(
				Constants::Constants::get().get_viewwidth(),
				Constants::Constants::get().get_viewheight()
			)
		), worldcount(0), world_selected(false), channel_slide(1.0f), notice_slide(1.0f),
		scroll_playing(false), scroll_has_anim(false)
	{
		std::string version_text = Configuration::get().get_version();
		version = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS, "Ver. " + version_text);
		version_pos = nl::nx::UI["Login.img"]["Common"]["version"]["pos"];

		int16_t vieww = Constants::Constants::get().get_viewwidth();
		int16_t viewh = Constants::Constants::get().get_viewheight();

		// 背景由 LoginBackground 统一绘制（地图 + 外框），这里走 step=1 的大区选择视角。
		bool bg_managed = LoginBackground::get().available();

		if (bg_managed)
			LoginBackground::get().set_step(1);

		Point<int16_t> viewpos = bg_managed
			? LoginBackground::get().get_viewpos()
			: Point<int16_t>(vieww / 2, viewh / 2);

		int16_t slotline_w = static_cast<int16_t>(WS_SLOT_STEP_X * WS_MAX_WORLDS);

		if (bg_managed)
		{
			auto sane_origin = [](Point<int16_t> o) -> Point<int16_t>
			{
				if (o.x() < -2048 || o.x() > 2048 || o.y() < -2048 || o.y() > 2048)
					return Point<int16_t>(0, 0);
				return o;
			};

			// 木牌对准横杆 signboard/0：水平以横杆中心居中，竖直挂在底边绳扣下。
			Texture pole_tex(nl::nx::Map["Obj"]["login.img"]["WorldSelect"]["signboard"]["0"]["0"]);
			Point<int16_t> pole_origin = sane_origin(pole_tex.get_origin());
			Point<int16_t> pole_dim = pole_tex.get_dimensions();
			Point<int16_t> pole_topleft = WS_POLE_MAP_POS + viewpos - pole_origin;
			int16_t pole_center_x = static_cast<int16_t>(pole_topleft.x() + pole_dim.x() / 2);
			int16_t pole_bottom_y = static_cast<int16_t>(pole_topleft.y() + pole_dim.y());
			worldsrc_pos = Point<int16_t>(
				static_cast<int16_t>(pole_center_x - slotline_w / 2),
				static_cast<int16_t>(pole_bottom_y + WS_BOARD_HANG_PAD)
			);

			// 频道面板顶边在羊皮卷 signboard/2 底边之下（不再以卷心为面板中心）。
			Texture scroll_tex(nl::nx::Map["Obj"]["login.img"]["WorldSelect"]["signboard"]["2"]["0"]);
			Point<int16_t> scroll_origin = sane_origin(scroll_tex.get_origin());
			Point<int16_t> scroll_dim = scroll_tex.get_dimensions();
			Point<int16_t> scroll_topleft = WS_SCROLL_MAP_POS + viewpos - scroll_origin;
			int16_t scroll_center_x = static_cast<int16_t>(scroll_topleft.x() + scroll_dim.x() / 2);
			int16_t scroll_bottom_y = static_cast<int16_t>(scroll_topleft.y() + scroll_dim.y());
			int16_t panel_top = static_cast<int16_t>(scroll_bottom_y + CH_BELOW_GAP);
			channelsrc_pos = Point<int16_t>(
				scroll_center_x,
				static_cast<int16_t>(panel_top + CH_PANEL_H / 2)
			);
		}
		else
		{
			worldsrc_pos = Point<int16_t>(
				static_cast<int16_t>((vieww - slotline_w) / 2),
				WS_BANNER_FALLBACK_Y
			);
			channelsrc_pos = Point<int16_t>(
				static_cast<int16_t>((vieww - CH_PANEL_W) / 2),
				static_cast<int16_t>(WS_BANNER_FALLBACK_Y + CH_PANEL_TOP_GAP + CH_PANEL_H / 2)
			);
		}

		worldid = Setting<DefaultWorld>::get().load();
		channelid = Setting<DefaultChannel>::get().load();

		nl::node Login = nl::nx::UI["Login.img"];
		nl::node Common = Login["Common"];
		nl::node WorldSelect = Login["WorldSelect"];

		worldsrc = WorldSelect["BtWorld"];
		worldnamesrc = WorldSelect["world"];
		channelsrc = WorldSelect["channel"];

		if (!bg_managed)
		{
			Point<int16_t> background_pos = Point<int16_t>(vieww / 2, viewh / 2);
			sprites.emplace_back(WorldSelect["scroll"]["0"]["0"], background_pos);
		}

		buttons[Buttons::BtExit] = std::make_unique<MapleButton>(Common["BtExit"], WS_EXIT_POS);
		buttons[Buttons::BtGoWorld] = std::make_unique<MapleButton>(
			WorldSelect["BtGoworld"],
			channelsrc_pos + WS_GOWORLD_OFF
		);
		buttons[Buttons::BtGoWorld]->set_active(false);

		// 左侧侧栏：step 装饰 + 地图/全角色 stub + 回到首页。
		step_channel = Sprite(Common["step"]["1"], Point<int16_t>(WS_SIDEBAR_X, WS_SIDEBAR_Y0));

		const int16_t step_h = Texture(Common["step"]["1"]).get_dimensions().y();
		int16_t side_y = static_cast<int16_t>(WS_SIDEBAR_Y0 + step_h + WS_SIDEBAR_AFTER_STEP);

		buttons[Buttons::BtViewChoice] = std::make_unique<MapleButton>(
			WorldSelect["BtViewChoice"],
			Point<int16_t>(WS_SIDEBAR_X, side_y)
		);
		side_y = static_cast<int16_t>(
			side_y + Texture(WorldSelect["BtViewChoice"]["normal"]["0"]).get_dimensions().y() + WS_SIDEBAR_GAP
		);

		buttons[Buttons::BtViewAll] = std::make_unique<MapleButton>(
			WorldSelect["BtViewAll"],
			Point<int16_t>(WS_SIDEBAR_X, side_y)
		);
		side_y = static_cast<int16_t>(
			side_y + Texture(WorldSelect["BtViewAll"]["normal"]["0"]).get_dimensions().y() + WS_SIDEBAR_GAP
		);

		buttons[Buttons::BtStart] = std::make_unique<MapleButton>(
			Common["BtStart"],
			Point<int16_t>(WS_SIDEBAR_X, side_y)
		);

		channels_background = WorldSelect["chBackgrn"];

		for (uint16_t i = 0; i < Buttons::BtGoWorld - Buttons::BtChannel0; i++)
		{
			std::string channel = std::to_string(i);

			buttons[Buttons::BtChannel0 + i] = std::make_unique<TwoSpriteButton>(
				channelsrc[channel]["normal"],
				channelsrc[channel]["normal"],
				channelsrc_pos - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2) + channel_cell_offset(i)
			);
			buttons[Buttons::BtChannel0 + i]->set_active(false);

			// maximum = 条宽；update 传入 0..1 负载比例（见 change_world）。
			Texture gauge_tex(channelsrc["chgauge"]);
			channel_gauge[i] = Gauge(
				Gauge::Type::WORLDSELECT,
				gauge_tex,
				std::max<int16_t>(1, gauge_tex.width()),
				0.0f
			);
		}

		for (uint8_t i = 0; i < WS_MAX_WORLDS; i++)
		{
			buttons[Buttons::BtWorld0 + i] = std::make_unique<TwoSpriteButton>(
				worldsrc["e"]["normal"]["0"],
				worldsrc["e"]["mouseOver"]["0"],
				worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i), 0)
			);
			buttons[Buttons::BtWorld0 + i]->set_active(true);
		}

		chatballoon.change_text("Please select the World you would like to play in.");

		// BeiDou：左侧已有「公告」气球，勿再初始化英文气泡（避免双提示框）。
		if (LoginBackground::get().available())
			chatballoon.change_text("");

		for (size_t i = 1; i <= FLAG_SIZE; i++)
			flag_sprites.emplace_back(Login["WorldNotice"][i]);

		if (Configuration::get().get_auto_login())
		{
			uint8_t world = Configuration::get().get_auto_world();
			uint8_t channel = Configuration::get().get_auto_channel();

			Configuration::get().set_worldid(world);
			Configuration::get().set_channelid(channel);

			UI::get().emplace<UILoginWait>();

			auto loginwait = UI::get().get_element<UILoginWait>();

			if (loginwait && loginwait->is_active())
				CharlistRequestPacket(world, channel).dispatch();
		}
	}

	Point<int16_t> UIWorldSelect::channel_panel_center() const
	{
		return channelsrc_pos;
	}

	Point<int16_t> UIWorldSelect::channel_panel_visual_center() const
	{
		// t=0: panel sits CH_SLIDE_DIST above rest; t=1: at rest.
		int16_t dy = static_cast<int16_t>((1.0f - channel_slide) * (-CH_SLIDE_DIST));
		return channelsrc_pos + Point<int16_t>(0, dy);
	}

	void UIWorldSelect::layout_channel_controls(Point<int16_t> panel_center)
	{
		Point<int16_t> panel_topleft = panel_center - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2);

		if (Button* go = buttons[Buttons::BtGoWorld].get())
			go->set_position(panel_center + WS_GOWORLD_OFF);

		for (uint16_t i = 0; i < CHANNEL_BUTTON_COUNT; i++)
		{
			if (Button* ch = get_channel_button(i))
				ch->set_position(panel_topleft + channel_cell_offset(i));
		}
	}

	void UIWorldSelect::update()
	{
		UIElement::update();

		if (world_selected && channel_slide < 1.0f)
		{
			channel_slide += static_cast<float>(Constants::TIMESTEP) / CH_SLIDE_MS;

			if (channel_slide > 1.0f)
				channel_slide = 1.0f;

			layout_channel_controls(channel_panel_visual_center());
		}

		// 公告展开与频道滑入同拍。
		if (world_selected && notice_slide < 1.0f)
		{
			notice_slide += static_cast<float>(Constants::TIMESTEP) / CH_SLIDE_MS;

			if (notice_slide > 1.0f)
				notice_slide = 1.0f;
		}

		if (scroll_playing)
		{
			if (scroll_open.update())
			{
				// 播完一轮后定格为静态频道底板，避免 rewind 闪回首帧。
				scroll_playing = false;
			}
		}
	}

	void UIWorldSelect::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		step_channel.draw(position, alpha);

		for (size_t i = 0; i < worlds.size(); i++)
		{
			World world = worlds[i];

			if (world.flag > 0 && world.flag <= FLAG_SIZE)
				flag_sprites[world.flag - 1].draw(
					position + worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i) + 5, 0),
					alpha
				);
		}

		Point<int16_t> panel_c = channel_panel_visual_center();

		if (world_selected)
		{
			// 优先播放 Login.img/WorldSelect/scroll/0 卷轴展开；播完后画静态 chBackgrn。
			if (scroll_playing && scroll_has_anim)
				scroll_open.draw(position + panel_c, alpha);
			else
				channels_background.draw(position + panel_c);
		}

		UIElement::draw_buttons(alpha);

		if (world_selected && worldid < worlds.size())
		{
			World selectedWorld = worlds[worldid];
			uint8_t channel_total = selectedWorld.channel_count;

			Texture world_name(worldnamesrc[std::to_string(selectedWorld.id)]);
			world_name.draw(position + panel_c + WS_WORLD_NAME_ON_PANEL);

			Point<int16_t> panel_topleft = panel_c - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2);

			for (size_t i = 0; i < Buttons::BtGoWorld - Buttons::BtChannel0 && i < channel_total; i++)
				channel_gauge[i].draw(position + panel_topleft + channel_cell_offset(static_cast<uint16_t>(i)));

			if (!world_notice.empty())
			{
				// 公告随 channel_slide 自上方滑入（与卷轴展开同步）。
				const int16_t notice_dy = static_cast<int16_t>((1.0f - notice_slide) * (-40));
				const int16_t notice_x = static_cast<int16_t>(
					panel_c.x() - CH_PANEL_W / 2 - world_notice.width() - WS_NOTICE_GAP
				);
				const int16_t notice_y = static_cast<int16_t>(
					panel_c.y() - CH_PANEL_H / 2 + 8 + notice_dy
				);
				world_notice.draw(position + Point<int16_t>(notice_x, notice_y));
			}
		}

		version.draw(position + version_pos - Point<int16_t>(0, 5));
		// BeiDou 用左侧「公告」气球；旧版英文 chatballoon 再画会变成两个提示框。
		if (!LoginBackground::get().available())
			chatballoon.draw(position + Point<int16_t>(747, 105));
	}

	Cursor::State UIWorldSelect::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Point<int16_t> panel_c = channel_panel_visual_center();
		Point<int16_t> panel_topleft = panel_c - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2);

		Rectangle<int16_t> channels_bounds = Rectangle<int16_t>(
			position + panel_topleft,
			position + panel_topleft + channels_background.get_dimensions()
			);

		// 「到选择的世界去」在卷内右下，仍单独并入命中，避免点按钮边缘被当成面板外取消选中。
		bool inside_goworld = false;

		auto goworld = buttons.find(Buttons::BtGoWorld);

		if (goworld != buttons.end() && goworld->second->is_active())
			inside_goworld = goworld->second->bounds(position).contains(cursorpos);

		if (world_selected && !channels_bounds.contains(cursorpos) && !inside_goworld)
		{
			if (clicked)
			{
				world_selected = false;
				channel_slide = 1.0f;
				clear_selected_world();
			}
		}

		Cursor::State ret = clicked ? Cursor::State::CLICKING : Cursor::State::IDLE;

		for (auto& btit : buttons)
		{
			if (btit.second->is_active() && btit.second->bounds(position).contains(cursorpos))
			{
				if (btit.second->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();

					btit.second->set_state(Button::State::MOUSEOVER);
					ret = Cursor::State::CANCLICK;
				}
				else if (btit.second->get_state() == Button::State::PRESSED)
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

	void UIWorldSelect::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (world_selected)
			{
				World selectedWorld = worlds[worldid];

				uint8_t selected_channel = (channelid == 0xFF) ? 0 : channelid;
				uint8_t channel_total = selectedWorld.channel_count;

				uint8_t columns = std::min(channel_total, COLUMNS);

				uint8_t rows = std::floor((channel_total - 1) / COLUMNS) + 1;

				div_t div = std::div(selected_channel, columns);
				int32_t current_col = div.rem;

				if (keycode == KeyAction::Id::UP)
				{
					auto next_channel = (selected_channel - COLUMNS < 0 ? (selected_channel - COLUMNS) + rows * COLUMNS : selected_channel - COLUMNS);

					if (next_channel == channelid)
						return;

					if (next_channel > channel_total)
						button_pressed(next_channel - COLUMNS + Buttons::BtChannel0);
					else
						button_pressed(next_channel + Buttons::BtChannel0);
				}
				else if (keycode == KeyAction::Id::DOWN)
				{
					auto next_channel = (selected_channel + COLUMNS >= channel_total ? current_col : selected_channel + COLUMNS);

					if (next_channel == channelid)
						return;

					if (next_channel > channel_total)
						button_pressed(next_channel + COLUMNS + Buttons::BtChannel0);
					else
						button_pressed(next_channel + Buttons::BtChannel0);
				}
				else if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::TAB)
				{
					if (selected_channel != 0)
						selected_channel--;
					else
						selected_channel = channel_total - 1;

					button_pressed(selected_channel + Buttons::BtChannel0);
				}
				else if (keycode == KeyAction::Id::RIGHT)
				{
					if (selected_channel != channel_total - 1)
						selected_channel++;
					else
						selected_channel = 0;

					button_pressed(selected_channel + Buttons::BtChannel0);
				}
				else if (escape)
				{
					world_selected = false;
					channel_slide = 1.0f;
					clear_selected_world();
				}
				else if (keycode == KeyAction::Id::RETURN)
				{
					button_pressed(Buttons::BtGoWorld);
				}
			}
			else
			{
				auto selected_world = worldid;

				if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::RIGHT || keycode == KeyAction::Id::UP || keycode == KeyAction::Id::DOWN || keycode == KeyAction::Id::TAB)
				{
					// 导航范围取「服务端下发世界数 ∩ 客户端按钮容量」：
					// worldcount 可能大于容量，若直接用它做取模/边界，选中值会落到
					// 不存在的槽位，索引随即越界到频道按钮段。
					const uint16_t navcount = navigable_world_count();

					if (navcount > 0)
					{
						if (worldid >= navcount)
							worldid = 0;

						if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::UP)
							selected_world = (worldid == 0) ? navcount - 1 : worldid - 1;
						else
							selected_world = (worldid + 1 >= navcount) ? 0 : worldid + 1;

						if (Button* previous = get_world_button(worldid))
							previous->set_state(Button::State::NORMAL);

						worldid = static_cast<uint8_t>(selected_world);

						if (Button* current = get_world_button(worldid))
							current->set_state(Button::State::PRESSED);
					}
				}
				else if (escape)
				{
					auto quitconfirm = UI::get().get_element<UIQuitConfirm>();

					if (quitconfirm && quitconfirm->is_active())
						return UI::get().send_key(keycode, pressed);
					else
						button_pressed(Buttons::BtExit);
				}
				else if (keycode == KeyAction::Id::RETURN)
				{
					auto quitconfirm = UI::get().get_element<UIQuitConfirm>();

					if (quitconfirm && quitconfirm->is_active())
					{
						return UI::get().send_key(keycode, pressed);
					}
					else
					{
						// 回车进入当前选中世界：仅在选中槽位确实存在时才派发，
						// 否则按下的 id 会落到频道/进入按钮段。
						if (worldid < navigable_world_count())
							button_pressed(worldid + Buttons::BtWorld0);
					}
				}
			}
		}
	}

	UIElement::Type UIWorldSelect::get_type() const
	{
		return TYPE;
	}

	void UIWorldSelect::draw_world()
	{
		if (worldcount <= 0)
			return;

		world_textures.clear();
		world_map.clear();

		for (size_t i = 0; i < worlds.size() && i < WS_MAX_WORLDS; i++)
		{
			World world = worlds[i];

			if (world.channel_count < 2)
				continue;

			std::string world_str = std::to_string(world.id);
			nl::node worldbtn = worldsrc[world_str];

			// 记录槽位 -> 真实世界 id，发包时使用。
			world_map[static_cast<uint16_t>(i)] = static_cast<uint16_t>(world.id);

			world_textures.emplace_back(worldnamesrc[world_str]);

			buttons[Buttons::BtWorld0 + i] = std::make_unique<TwoSpriteButton>(
				worldbtn["normal"]["0"],
				worldbtn["mouseOver"]["0"],
				worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i), 0)
			);
			buttons[Buttons::BtWorld0 + i]->set_active(true);

			// 不在 WorldList 到达时预选任何频道：预选会使用户"首点频道即进入世界"，
			// 跳过「选中→(再点或点到选择的世界去)确认」的两步流程与频道滑入动画。
			// channelid 维持 clear_selected_world()/头文件默认的 0xFF（未选）。
			}
	}

	void UIWorldSelect::add_world(World world)
	{
		// 超出按钮容量时不再静默丢弃：记录限频告警，便于发现「服务端世界列表
		// 与客户端按钮容量不匹配」。同时不再把该世界放进 worlds，
		// 以保证 worldcount == worlds.size() 这一不变量，键盘导航因此不会越过实际槽位。
		if (worldcount >= WS_MAX_WORLDS)
		{
			warn_button_index_out_of_range(WS_BOUNDS_WARNED_WORLD, WS_MAX_WORLDS, "world", WS_MAX_WORLDS);

			return;
		}

		worlds.emplace_back(std::move(world));
		worldcount++;
	}

	void UIWorldSelect::change_world(World selectedWorld)
	{
		if (Button* world_btn = get_world_button(worldid))
			world_btn->set_state(Button::State::PRESSED);

		// 频道相关数据全部来自服务端，且三者长度未必一致：
		//   - channel_count：包里的频道数（uint8_t）；
		//   - channel_capacities：变长 vector，越界读属未定义行为；
		//   - channel_gauge：**定长数组**（CHANNEL_BUTTON_COUNT），越界写会直接破坏内存。
		// 故按三者最小值收敛，多余频道忽略并限频告警。
		uint16_t limit = static_cast<uint16_t>(selectedWorld.channel_count);

		if (limit > CHANNEL_BUTTON_COUNT)
		{
			warn_channel_count_out_of_range(limit);

			limit = CHANNEL_BUTTON_COUNT;
		}

		if (static_cast<size_t>(limit) > selectedWorld.channel_capacities.size())
			limit = static_cast<uint16_t>(selectedWorld.channel_capacities.size());

		for (uint16_t i = 0; i < limit; ++i)
		{
			if (Button* channel_btn = get_channel_button(i))
			{
				channel_btn->set_active(true);

				if (i == channelid)
					channel_btn->set_state(Button::State::PRESSED);
			}

			// Gauge::draw 用 percentage*maximum；必须传 0..1，不能传绝对人数。
			float load = static_cast<float>(selectedWorld.channel_capacities[i]) / static_cast<float>(CHANNEL_LOAD);
			if (load < 0.0f)
				load = 0.0f;
			if (load > 1.0f)
				load = 1.0f;
			channel_gauge[i].update(load);
		}

		buttons[Buttons::BtGoWorld]->set_active(true);

		world_notice.change_text(selectedWorld.event_message);
		notice_slide = 0.0f;
	}

	void UIWorldSelect::remove_selected()
	{
		deactivate();

		Sound(Sound::Name::SCROLLUP).play();

		world_selected = false;
		channel_slide = 1.0f;
		notice_slide = 1.0f;
		scroll_playing = false;

		clear_selected_world();
	}

	void UIWorldSelect::set_region(uint8_t regionid)
	{
		// BeiDou 版式下没有 index/[region] 分层，大区列表直接由服务端下发的
		// 世界 id 决定，故这里仅保留接口以兼容既有调用。
		(void)regionid;
	}

	uint16_t UIWorldSelect::get_worldbyid(uint16_t worldid)
	{
		auto it = world_map.find(worldid);

		// 槽位 -> 真实世界 id；查不到时退回槽位本身（避免解引用 end()）。
		return (it != world_map.end()) ? it->second : worldid;
	}

	Button* UIWorldSelect::get_world_button(uint16_t index) const
	{
		if (index >= WS_MAX_WORLDS)
		{
			warn_button_index_out_of_range(WS_BOUNDS_WARNED_WORLD, index, "world", WS_MAX_WORLDS);

			return nullptr;
		}

		auto it = buttons.find(static_cast<uint16_t>(Buttons::BtWorld0 + index));

		// 未创建（例如该槽世界因 channel_count < 2 被跳过）时同样返回空，由调用方跳过。
		return (it != buttons.end()) ? it->second.get() : nullptr;
	}

	Button* UIWorldSelect::get_channel_button(uint16_t index) const
	{
		if (index >= CHANNEL_BUTTON_COUNT)
		{
			warn_button_index_out_of_range(WS_BOUNDS_WARNED_CHANNEL, index, "channel", CHANNEL_BUTTON_COUNT);

			return nullptr;
		}

		auto it = buttons.find(static_cast<uint16_t>(Buttons::BtChannel0 + index));

		return (it != buttons.end()) ? it->second.get() : nullptr;
	}

	uint16_t UIWorldSelect::navigable_world_count() const
	{
		// 键盘导航不得越过实际容量：worldcount 只反映服务端下发的世界数，
		// 可能大于客户端按钮容量（WS_MAX_WORLDS），越过就会命中通道按钮段。
		const uint16_t by_server = static_cast<uint16_t>(worldcount);

		return (by_server < WS_MAX_WORLDS) ? by_server : WS_MAX_WORLDS;
	}

	Button::State UIWorldSelect::button_pressed(uint16_t id)
	{
		// 参考端 CUIWorldSelect::IsRequestValid：step 1 且 CLogin+368 门闩为空。
		// 当前端以 UILoginWait 作为“请求进行中”的等价标记。
		if (UI::get().get_element<UILoginWait>())
			return Button::State::NORMAL;

		if (id == Buttons::BtGoWorld)
		{
			enter_world();

			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtExit)
		{
			UI::get().emplace<UIQuitConfirm>();

			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtStart)
		{
			// 回到首页：重置会话后再进登录，否则服仍停在选世界态，
			// 再点连接会一直卡在 Loading（收不到 LoginResult）。
			Sound(Sound::Name::SCROLLUP).play();
			Session::get().reconnect();
			LoginBackground::get().set_step(0);
			UI::get().remove(UIElement::Type::WORLDSELECT);
			UI::get().remove(UIElement::Type::LOGINWAIT);
			UI::get().remove(UIElement::Type::LOGINNOTICE);
			UI::get().emplace<UILogin>();

			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtViewChoice || id == Buttons::BtViewAll)
		{
			// 本版 stub：资源与布局到位，功能后续再接。
			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtRegion)
		{
			UI::get().emplace<UIRegion>();

			deactivate();

			return Button::State::NORMAL;
		}
		else if (id >= Buttons::BtWorld0 && id < Buttons::BtChannel0)
		{
			const uint16_t next_world = static_cast<uint16_t>(id - Buttons::BtWorld0);

			// 该槽位必须实际存在对应世界：draw_world 会跳过 channel_count < 2 的世界，
			// 服务端下发的世界数也可能少于槽位号，此时 worlds[next_world] 会越界。
			if (next_world >= worlds.size())
			{
				warn_button_index_out_of_range(
					WS_BOUNDS_WARNED_WORLD, next_world, "world", static_cast<uint16_t>(worlds.size()));

				return Button::State::NORMAL;
			}

			if (Button* previous = get_world_button(worldid))
				previous->set_state(Button::State::NORMAL);

			worldid = static_cast<uint8_t>(next_world);

			ServerStatusRequestPacket(get_worldbyid(worldid)).dispatch();

			world_selected = true;
			channel_slide = 0.0f;
			notice_slide = 0.0f;
			// BeiDou 地图已有 signboard 卷轴；Login.img/WorldSelect/scroll 是旧版全屏卷（513×401），
			// 锚在频道面板中心会错位。仅用 CH_SLIDE，不再播该动画。
			if (!LoginBackground::get().available())
			{
				if (nl::node open = nl::nx::UI["Login.img"]["WorldSelect"]["scroll"]["0"])
				{
					scroll_open = Animation(open);
					scroll_open.reset();
					scroll_has_anim = true;
					scroll_playing = true;
				}
				else
				{
					scroll_has_anim = false;
					scroll_playing = false;
				}
			}
			else
			{
				scroll_has_anim = false;
				scroll_playing = false;
			}
			layout_channel_controls(channel_panel_visual_center());
			clear_selected_world();
			change_world(worlds[worldid]);

			return Button::State::PRESSED;
		}
		else if (id >= Buttons::BtChannel0 && id < Buttons::BtGoWorld)
		{
			const uint8_t selectedch = static_cast<uint8_t>(id - Buttons::BtChannel0);

			// 尚未选中任何频道（clear 后 channelid=0xFF）或点了其它频道 → 仅选中。
			// 再次点击同一频道 → 进入世界。
			if (channelid == 0xFF || selectedch != channelid)
			{
				if (channelid != 0xFF)
				{
					if (Button* previous = get_channel_button(channelid))
						previous->set_state(Button::State::NORMAL);
				}

				channelid = selectedch;

				if (Button* current = get_channel_button(channelid))
					current->set_state(Button::State::PRESSED);

				Sound(Sound::Name::WORLDSELECT).play();
			}
			else
			{
				enter_world();
			}

			return Button::State::PRESSED;
		}
		else
		{
			return Button::State::NORMAL;
		}
	}

	void UIWorldSelect::enter_world()
	{
		if (channelid == 0xFF || UI::get().get_element<UILoginWait>())
			return;

		Configuration::get().set_worldid(worldid);
		Configuration::get().set_channelid(channelid);

		UI::get().emplace<UILoginWait>();
		auto loginwait = UI::get().get_element<UILoginWait>();

		if (loginwait && loginwait->is_active())
			CharlistRequestPacket(get_worldbyid(worldid), channelid).dispatch();
	}

	void UIWorldSelect::clear_selected_world()
	{
		// 不预选 CH.0：否则首点 CH.1（index 0）会命中 selectedch==channelid 直接 enter_world，
		// 频道滑入动画与「再点确认」流程被跳过。
		channelid = 0xFF;

		// 频道按钮已在构造函数中按 CHANNEL_BUTTON_COUNT 全部创建；
		// 这里仍统一走带范围校验的访问器，避免日后调整容量时漏改本处。
		for (uint16_t i = 0; i < CHANNEL_BUTTON_COUNT; i++)
		{
			if (Button* channel_btn = get_channel_button(i))
				channel_btn->set_state(Button::State::NORMAL);
		}

		for (size_t i = 0; i < Buttons::BtGoWorld - Buttons::BtChannel0; i++)
		{
			buttons[Buttons::BtChannel0 + i]->set_active(false);

			channel_gauge[i].update(0);
		}

		buttons[Buttons::BtGoWorld]->set_active(false);

		world_notice.change_text("");
	}

	uint16_t UIWorldSelect::get_next_world(uint16_t id, bool upward)
	{
		// 与键盘导航同源：范围取「服务端下发世界数 ∩ 客户端按钮容量」，
		// 保证返回值始终指向实际存在的槽位。
		const uint16_t navcount = navigable_world_count();

		if (navcount == 0)
			return 0;

		if (upward)
			return (id == 0) ? navcount - 1 : id - 1;

		return (id + 1 >= navcount) ? 0 : id + 1;
	}
}
