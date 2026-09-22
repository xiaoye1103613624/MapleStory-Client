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

#include "UILoginNotice.h"
#include "UILoginWait.h"
#include "UIRegion.h"

#include "../UI.h"

#include "../LoginBackground.h"

#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"

#include "../../Audio/Audio.h"
#include "../../Util/Randomizer.h"

#include "../../Net/Packets/LoginPackets.h"

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

		// 日志文案保持 ASCII：本仓库未启用 /utf-8，中文宽字节序列在错误的代码页下
		// 可能被误解析（尾字节 0x5C 会转义引号，导致 C2001），中文说明一律写在注释里。
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

	// 世界条幅（木质横杆）在**登录地图坐标系**中的 Y 偏移：IDA 中槽位 Y=0 相对条幅，
	// 因此条幅位置必须跟随地图相机（LoginBackground::get_viewpos()）走，
	// 而不是写死屏幕绝对像素——否则换分辨率就会错位（与 UILogin 用告示牌锚点同理）。
	// 取值来源：1280x720 实测条幅顶边屏幕 y=111，而同分辨率下 viewpos.y = 720/2 + 608 = 968，
	// 故地图系偏移 = 111 - 968 = -857。
	static constexpr int16_t WS_BANNER_OBJ_Y = -857;

	// LoginBackground 不可用时的兜底（退化路径，按 1280x720 标定值）。
	static constexpr int16_t WS_BANNER_FALLBACK_Y = 111;

	// 频道面板顶边相对条幅顶边的固定间距（旧版式：面板左上 y=74 - 条幅 y=40 = 34）。
	// 面板中心 = 条幅顶 + 间距 + 高/2；条幅下移后面板同步下移，保持既有相对版式。
	static constexpr int16_t CH_PANEL_TOP_GAP = 34;

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
		), worldcount(0), world_selected(false)
	{
		std::string version_text = Configuration::get().get_version();
		version = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS, "Ver. " + version_text);
		version_pos = nl::nx::UI["Login.img"]["Common"]["version"]["pos"];

		int16_t vieww = Constants::Constants::get().get_viewwidth();
		int16_t viewh = Constants::Constants::get().get_viewheight();

		// 背景由 LoginBackground 统一绘制（地图 + 外框），这里走 step=1 的大区选择视角。
		// 必须先把 step 置好，后面才能用 viewpos 求出条幅的屏幕落点。
		bool bg_managed = LoginBackground::get().available();

		if (bg_managed)
			LoginBackground::get().set_step(1);

		// 世界条幅：水平以 vieww 居中；纵向**锚在登录地图上**（地图系偏移 WS_BANNER_OBJ_Y），
		// 随相机 viewpos 变化，故任意分辨率下都贴在木质横杆上（与 UILogin 用告示牌锚点同理）。
		int16_t banner_y = WS_BANNER_FALLBACK_Y;

		if (bg_managed)
			banner_y = static_cast<int16_t>(LoginBackground::get().get_viewpos().y() + WS_BANNER_OBJ_Y);

		// 频道面板水平居中，其顶边比条幅低 CH_PANEL_TOP_GAP，故中心 = 顶 + 高/2。
		int16_t slotline_w = static_cast<int16_t>(WS_SLOT_STEP_X * WS_SLOT_COUNT);
		worldsrc_pos = Point<int16_t>(
			static_cast<int16_t>((vieww - slotline_w) / 2),
			banner_y
		);
		channelsrc_pos = Point<int16_t>(
			static_cast<int16_t>((vieww - CH_PANEL_W) / 2),
			static_cast<int16_t>(banner_y + CH_PANEL_TOP_GAP + CH_PANEL_H / 2)
		);

		worldid = Setting<DefaultWorld>::get().load();
		channelid = Setting<DefaultChannel>::get().load();

		nl::node Login = nl::nx::UI["Login.img"];
		nl::node Common = Login["Common"];
		nl::node WorldSelect = Login["WorldSelect"];

		// BeiDou 使用 GMS v83 版式（无 BtWorld/release、无 BtChannel 节点）。
		worldsrc = WorldSelect["BtWorld"];
		worldnamesrc = WorldSelect["world"];
		channelsrc = WorldSelect["channel"];

		// LoginBackground 不可用时（非 BeiDou 数据）退回自绘卷轴背景。
		if (!bg_managed)
		{
			Point<int16_t> background_pos = Point<int16_t>(vieww / 2, viewh / 2);
			sprites.emplace_back(WorldSelect["scroll"]["0"]["0"], background_pos);
		}

		buttons[Buttons::BtExit] = std::make_unique<MapleButton>(Common["BtExit"]);
		buttons[Buttons::BtGoWorld] = std::make_unique<MapleButton>(
			WorldSelect["BtGoworld"],
			channelsrc_pos + Point<int16_t>(150, 250)
		);
		buttons[Buttons::BtGoWorld]->set_active(false);

		channels_background = WorldSelect["chBackgrn"];

		// 频道按钮按网格排布在频道面板内（chBackgrn 449x233，原点在中心）。
		for (uint16_t i = 0; i < Buttons::BtGoWorld - Buttons::BtChannel0; i++)
		{
			std::string channel = std::to_string(i);

			buttons[Buttons::BtChannel0 + i] = std::make_unique<TwoSpriteButton>(
				channelsrc[channel]["normal"],
				channelsrc[channel]["normal"],
				channelsrc_pos - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2) + channel_cell_offset(i)
			);
			buttons[Buttons::BtChannel0 + i]->set_active(false);

			channel_gauge[i] = Gauge(
				Gauge::Type::WORLDSELECT,
				channelsrc["chgauge"],
				CHANNEL_LOAD,
				0.0f
			);
		}

		// 世界槽先以空槽(BtWorld/e)占位，收到世界列表后在 draw_world 中替换为真实按钮。
		for (uint8_t i = 0; i < WS_MAX_WORLDS; i++)
		{
			buttons[Buttons::BtWorld0 + i] = std::make_unique<TwoSpriteButton>(
				worldsrc["e"]["normal"]["0"],
				worldsrc["e"]["mouseOver"]["0"],
				worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i), 0)
			);
			buttons[Buttons::BtWorld0 + i]->set_active(false);
		}

		chatballoon.change_text("Please select the World you would like to play in.");

		for (size_t i = 1; i <= FLAG_SIZE; i++)
			flag_sprites.emplace_back(Login["WorldNotice"][i]);

		worldNotice = WorldSelect["worldNotice"]["0"];
		rebootNotice = WorldSelect["worldNotice"]["reboot"];
		worldNoticeMessage = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::MINESHAFT);

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

	void UIWorldSelect::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		// 世界名图（world/[worldId]）画在对应条幅槽位上。
		for (size_t i = 0; i < world_textures.size(); i++)
			world_textures[i].draw(position + worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i), 0));

		// 大区提示图标（IDA: 画在槽位 x+5 处）。
		for (size_t i = 0; i < worlds.size(); i++)
		{
			World world = worlds[i];

			if (world.flag > 0 && world.flag <= FLAG_SIZE)
				flag_sprites[world.flag - 1].draw(
					position + worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i) + 5, 0),
					alpha
				);
		}

		if (world_selected)
			channels_background.draw(position + channelsrc_pos);

		UIElement::draw_buttons(alpha);

		if (world_selected)
		{
			World selectedWorld = worlds[worldid];
			uint8_t channel_total = selectedWorld.channel_count;

			for (size_t i = 0; i < Buttons::BtGoWorld - Buttons::BtChannel0 && i < channel_total; i++)
			{
				// 负载条与频道按钮共用同一格点（channel_cell_offset），保证视觉对齐
				//（此前硬编码 22+100*col / 88+34*row，与按钮的 56 起始错位 32px 且列数不一致）。
				channel_gauge->draw(position + channelsrc_pos - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2) + channel_cell_offset(static_cast<uint16_t>(i)));
			}

			// 世界公告画在面板“表头”空白带（频道网格自面板顶 +56 起），
			// 旧值 (58,-50) 会落到面板中心下方、压住第一行频道按钮（实测 y 与 CH.1 同行）。
			if (!worldNoticeMessage.get_text().empty())
				worldNoticeMessage.draw(position + channelsrc_pos + Point<int16_t>(58, -90));
		}

		version.draw(position + version_pos - Point<int16_t>(0, 5));
		// 提示气球位于木质横杆上方（不随条幅下移，否则会压住告示牌）。
		chatballoon.draw(position + Point<int16_t>(747, 105));
	}

	Cursor::State UIWorldSelect::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		// channelsrc_pos 是面板中心，判定区域要用面板左上角。
		Point<int16_t> panel_topleft = channelsrc_pos - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2);

		Rectangle<int16_t> channels_bounds = Rectangle<int16_t>(
			position + panel_topleft,
			position + panel_topleft + channels_background.get_dimensions()
			);

		// 「到选择的世界去」(BtGoWorld) 位于频道面板下方、面板矩形之外；
		// 若只按面板矩形判定，点它会被当成“点面板外”而先去选世界，
		// 导致该按钮被 clear_selected_world() 停用、永远点不动。故把它的判定区并入。
		bool inside_goworld = false;

		auto goworld = buttons.find(Buttons::BtGoWorld);

		if (goworld != buttons.end() && goworld->second->is_active())
			inside_goworld = goworld->second->bounds(position).contains(cursorpos);

		if (world_selected && !channels_bounds.contains(cursorpos) && !inside_goworld)
		{
			if (clicked)
			{
				world_selected = false;
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

				uint8_t selected_channel = channelid;
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

			if (channelid >= world.channel_count)
				channelid = 0;
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

			channel_gauge[i].update(selectedWorld.channel_capacities[i]);
		}

		buttons[Buttons::BtGoWorld]->set_active(true);

		worldNoticeMessage.change_text(selectedWorld.event_message);
	}

	void UIWorldSelect::remove_selected()
	{
		deactivate();

		Sound(Sound::Name::SCROLLUP).play();

		world_selected = false;

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
			clear_selected_world();
			change_world(worlds[worldid]);

			return Button::State::PRESSED;
		}
		else if (id >= Buttons::BtChannel0 && id < Buttons::BtGoWorld)
		{
			const uint8_t selectedch = static_cast<uint8_t>(id - Buttons::BtChannel0);

			if (selectedch != channelid)
			{
				if (Button* previous = get_channel_button(channelid))
					previous->set_state(Button::State::NORMAL);

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
		Configuration::get().set_worldid(worldid);
		Configuration::get().set_channelid(channelid);

		UI::get().emplace<UILoginWait>();
		auto loginwait = UI::get().get_element<UILoginWait>();

		if (loginwait && loginwait->is_active())
			CharlistRequestPacket(get_worldbyid(worldid), channelid).dispatch();
	}

	void UIWorldSelect::clear_selected_world()
	{
		channelid = 0;

		// 频道按钮已在构造函数中按 CHANNEL_BUTTON_COUNT 全部创建；
		// 这里仍统一走带范围校验的访问器，避免日后调整容量时漏改本处。
		for (uint16_t i = 0; i < CHANNEL_BUTTON_COUNT; i++)
		{
			if (Button* channel_btn = get_channel_button(i))
				channel_btn->set_state(Button::State::NORMAL);
		}

		if (Button* first_channel = get_channel_button(0))
			first_channel->set_state(Button::State::PRESSED);

		for (size_t i = 0; i < Buttons::BtGoWorld - Buttons::BtChannel0; i++)
		{
			buttons[Buttons::BtChannel0 + i]->set_active(false);

			channel_gauge[i].update(0);
		}

		buttons[Buttons::BtGoWorld]->set_active(false);

		worldNoticeMessage.change_text("");
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
