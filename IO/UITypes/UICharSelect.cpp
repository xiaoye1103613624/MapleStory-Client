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
#include "UICharSelect.h"

#include "UILoginNotice.h"
#include "UIRaceSelect.h"
#include "UISoftKey.h"
#include "UIWorldSelect.h"

#include "../UI.h"

#include "../LoginBackground.h"

#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"

#include "../../Configuration.h"

#include "../../Audio/Audio.h"
#include "../../Character/Job.h"

#include "../../Net/Packets/SelectCharPackets.h"

#define NOMINMAX
#include <windows.h>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 选角色按钮条版式：IDA CUICharSelect::OnCreate @0x60148D 字面量。
	// 按钮条窗口为 101x131（ctor CreateWnd(200,-1359,101,131)），三按钮贴左沿 x=4。
	//
	// 注意：OnCreate 里的 (4,-2)/(4,36)/(4,85) 是**按钮条窗口内**坐标，
	// 必须叠加按钮条窗口的帧内锚点 (200,-1359) 才能落到与 position 同一坐标系；
	// 否则会被当成帧内坐标 → 实际落到屏幕外（实测按钮不可见）。
	static constexpr int16_t CS_BTN_L = 200;
	static constexpr int16_t CS_BTN_T = -1359;
	static constexpr int16_t CS_BTN_X = CS_BTN_L + 4;
	static constexpr int16_t CS_BTN_Y_SELECT = CS_BTN_T + (-2);
	static constexpr int16_t CS_BTN_Y_NEW = CS_BTN_T + 36;
	static constexpr int16_t CS_BTN_Y_DELETE = CS_BTN_T + 85;

	// 角色槽纵向锚点：IDA `CUICharDetail::CUICharDetail`(0x60292F) 把
	//   v8.y = 占用 ? 1384 : 1348  →  CFadeWnd::SetOption(..., v8, v8, v8, 80, 0, 80)
	// 该点处于**登录画布坐标系**（`CreateFadeWnd(..., bScreenCoord = 0)`，非屏幕绝对）。
	// 画布步进带顶面 = |GetStepHeight(2)| = |-(8 + 600*2)| = 1208（`CLogin::GetStepHeight`），
	// 故带内偏移 = 1384-1208 = 176（占用）/ 1348-1208 = 140（空），两者差 36px。
	// 槽位窗口尺寸同样来自该 ctor：`CreateFadeWnd(183, 占用151 / 空115, 0, z=20, 0, 0)`。
	static constexpr int16_t CS_SLOT_INSET_EMPTY = 1348 - 1208;
	static constexpr int16_t CS_SLOT_INSET_OCCUPIED = 1384 - 1208;
	static constexpr int16_t CS_SLOT_W = 183;
	static constexpr int16_t CS_SLOT_H_EMPTY = 115;
	static constexpr int16_t CS_SLOT_H_OCCUPIED = 151;

	// 帧内坐标 -> 屏幕原点：position = LoginBackground::viewpos + 本常量。
	// 反推依据（非猜测）：选世界槽位帧内 = CUIWorldSelect 容器 (−140,−804) 的 (0,0)，
	// 实测屏幕 (235,110) ⇒ step1 偏移 (375,914)；viewpos(step1)=(612,968) ⇒ 差值本常量。
	static const Point<int16_t> CS_FRAME_ORIGIN(-237, -54);

	// 角色列表容器（IDA sub_603FF0: CreateWnd(-290,-1218,600,80,z:10,0)）
	static constexpr int16_t CS_LIST_T = -1218;

	UICharSelect::UICharSelect(std::vector<CharEntry> characters, int8_t characters_count, int32_t slots, int8_t require_pic) : UIElement(Point<int16_t>(0, 0), Point<int16_t>(800, 600)),
		characters(characters), characters_count(characters_count), slots(slots), require_pic(require_pic), tab_index(0), tab_active(false), tab_move(false), charslot_y(0), use_timestamp(false), burning_character(true), show_pic_btns(false)
	{
		std::string version_text = Configuration::get().get_version();
		version = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS, "Ver. " + version_text);

		nl::node Login = nl::nx::UI["Login.img"];
		nl::node Common = Login["Common"];
		version_pos = Common["version"]["pos"];

		selected_character = Setting<DefaultCharacter>::get().load();
		selected_page = selected_character / PAGESIZE;
		page_count = std::ceil((double)slots / (double)PAGESIZE);

		tab = nl::nx::UI["Basic.img"]["Cursor"]["18"]["0"];
		tab_move_pos = 0;

		tab_map[0] = Buttons::BtSelect;
		tab_map[1] = Buttons::BtNew;
		tab_map[2] = Buttons::BtDelete;

		nl::node CharSelect = Login["CharSelect"];
		nl::node selectedWorld = CharSelect["selectedWorld"];
		nl::node pageNew = CharSelect["pageNew"];

		uint16_t world;
		uint8_t world_id = Configuration::get().get_worldid();
		uint8_t channel_id = Configuration::get().get_channelid();

		if (auto worldselect = UI::get().get_element<UIWorldSelect>())
			world = worldselect->get_worldbyid(world_id);

		world_sprites.emplace_back(Common["selectWorld"]);
		world_sprites.emplace_back(selectedWorld["icon"][world]);
		world_sprites.emplace_back(selectedWorld["name"][world]);
		world_sprites.emplace_back(selectedWorld["ch"][channel_id]);

		// BeiDou 下由 LoginBackground 统一绘制原生登录背景（地图 + 外框），
		// 这里使用 step=2 的角色选择视角，跳过 GMS 板式背景。
		bool bg_managed = LoginBackground::get().available();

		if (bg_managed)
		{
			LoginBackground::get().set_step(2);

			// 帧内坐标 -> 屏幕原点。由**已验证的选世界条幅**反推，不是猜：
			//   选世界槽位帧内 = CUIWorldSelect 容器 (−140, −804) 的 (0,0)；
			//   实测其屏幕位置 = (235, 110)（wordsrc_pos，见 verified_worldselect.png）
			//   ⇒ step=1 偏移 = (375, 914)；而 viewpos(step1) = (612, 968)
			//   ⇒ position = viewpos + (−237, −54)（对任意 step/分辨率自动跟随相机）。
			position = LoginBackground::get().get_viewpos() + CS_FRAME_ORIGIN;
		}
		else
		{
			// 兼容数据命名：BeiDou 的登录背景图名为 Back/login.img 而非 UI_login.img
			nl::node map = nl::nx::Map001["Back"]["UI_login.img"];
			if (!map)
				map = nl::nx::Map001["Back"]["login.img"];
			nl::node back = map["back"];

			sprites.emplace_back(back["1"], Point<int16_t>(512, 384));

			for (nl::node node : map["ani"])
				sprites.emplace_back(node, Point<int16_t>(0, -2));

			sprites.emplace_back(back["2"], Point<int16_t>(512, 384));

			// 无登录地图时退化为把 800x600 版心居中。
			position = Point<int16_t>(
				static_cast<int16_t>((Constants::Constants::get().get_viewwidth() - 800) / 2),
				static_cast<int16_t>((Constants::Constants::get().get_viewheight() - 600) / 2)
			);
		}

		nl::node BurningNotice = Common["Burning"]["BurningNotice"];
		burning_notice = BurningNotice;
		burning_numPos = BurningNotice["numPos"];
		burning_count = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::CREAM, "1");

		charinfo = CharSelect["charInfo1"];

		nl::node charSlot = CharSelect["charSlot"];
		charslot = charSlot["0"];
		charslot_pos = charSlot["pos"];
		charslot_bounds = Rectangle<int16_t>(charSlot["worldInfoLT"], charSlot["worldInfoRB"]);

		pagebase = pageNew["base"]["0"];
		pagenumber = Charset(pageNew["number"], Charset::Alignment::LEFT);
		pagenumberpos = pageNew["numberpos"];
		pagepos = pageNew["pos"];

		signpost[0] = CharSelect["adventure"]["0"];
		signpost[1] = CharSelect["knight"]["0"];
		signpost[2] = CharSelect["aran"]["0"];

		nametag = CharSelect["nameTag"];

		nl::node BtNew = CharSelect["BtNew"];
		Texture BtNewTexture = Texture(BtNew["normal"]["0"]);
		Point<int16_t> BtNewDim = BtNewTexture.get_dimensions();
		BtNewPos = BtNewTexture.get_origin().abs();

		nl::node BtSelect = CharSelect["BtSelect"];
		Point<int16_t> BtSelectPos = Texture(BtSelect["normal"]["0"]).get_origin().abs();

		Point<int16_t> tab_adj = Point<int16_t>(107, 8);

		tab_pos[0] = tab_adj + BtSelectPos + Point<int16_t>(2, 0);
		tab_pos[1] = tab_adj + BtNewPos;
		tab_pos[2] = tab_adj + BtNewPos + Point<int16_t>(BtNewDim.x() - 1, 0);

		// 按钮位置按 IDA 字面量给（CUICharSelect::OnCreate @0x60148D）：
		//   BtSelect (4,-2) / BtNew (4,36) / BtDelete (4,85)，位于 101x131 按钮条内且贴左沿。
		// 北斗节点里没有 pos（实测 BtDelete/pos 等全 <absent>），不能靠节点取值。
		buttons[Buttons::BtSelect] = std::make_unique<MapleButton>(BtSelect, CS_BTN_X, CS_BTN_Y_SELECT);
		buttons[Buttons::BtNew] = std::make_unique<MapleButton>(BtNew, CS_BTN_X, CS_BTN_Y_NEW);
		buttons[Buttons::BtDelete] = std::make_unique<MapleButton>(CharSelect["BtDelete"], CS_BTN_X, CS_BTN_Y_DELETE);
		buttons[Buttons::BtPageL] = std::make_unique<MapleButton>(CharSelect["pageL"]);
		buttons[Buttons::BtPageR] = std::make_unique<MapleButton>(CharSelect["pageR"]);
		buttons[Buttons::BtChangePIC] = std::make_unique<MapleButton>(Common["BtChangePIC"]);
		buttons[Buttons::BtResetPIC] = std::make_unique<MapleButton>(Login["WorldSelect"]["BtResetPIC"]);
		buttons[Buttons::BtCharacter] = std::make_unique<MapleButton>(CharSelect["EditCharList"]["BtCharacter"]);
		buttons[Buttons::BtPreview] = std::make_unique<MapleButton>(Common["BtPreview"]);

		nl::node pos = CharSelect["pos"];
		nl::node avatarPos = pos["avatarPos"];
		avatarLT = avatarPos["avatarLT"];
		avatarRB = avatarPos["avatarRB"];
		avatarSpace = avatarPos["avatarSpace"];

		Point<int16_t> screen_adj = Point<int16_t>(0, 3);

		nl::node detailPos = pos["detailPos"];
		jobPos = detailPos["jobPos"];
		levelPos = detailPos["levelPos"];
		namePos = detailPos["namePos"];
		statDEXPos = detailPos["statDEXPos"];
		statINTPos = detailPos["statINTPos"];
		statLUKPos = detailPos["statLUKPos"];
		statSTRPos = detailPos["statSTRPos"];

		jobPos -= screen_adj;
		namePos -= Point<int16_t>(0, 5);
		statDEXPos -= screen_adj;
		statINTPos -= screen_adj;
		statLUKPos -= screen_adj;
		statSTRPos -= screen_adj;

		// 命中区需同时覆盖「占用槽」与「空槽」两种纵向锚点（两者相差 36px），
		// 故取较低的占用槽为顶点、高度补足差值，避免空槽/占用槽点击命中不一致。
		for (size_t i = 0; i < PAGESIZE; i++)
			buttons[Buttons::CHARACTER_SLOT0 + i] = std::make_unique<AreaButton>(
				get_character_slot_pos(i, true),
				avatarRB - avatarLT + Point<int16_t>(0, CS_SLOT_INSET_OCCUPIED - CS_SLOT_INSET_EMPTY));

		if (require_pic == 0 || !show_pic_btns)
		{
			buttons[Buttons::BtChangePIC]->set_active(false);
			buttons[Buttons::BtResetPIC]->set_active(false);
		}

		levelset = Charset(CharSelect["lv"], Charset::Alignment::CENTER);
		namelabel = OutlinedText(Text::Font::A14B, Text::Alignment::CENTER, Color::Name::WHITE, Color::Name::IRISHCOFFEE);

		for (size_t i = 0; i < InfoLabel::NUM_LABELS; i++)
			infolabels[i] = OutlinedText(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::WHITE, Color::Name::TOBACCOBROWN);

		for (CharEntry& entry : characters)
		{
			charlooks.emplace_back(entry.look);
			nametags.emplace_back(nametag, Text::Font::A12M, entry.stats.name);
		}

		emptyslot_effect = CharSelect["character"]["0"];
		emptyslot = CharSelect["character"]["1"]["0"];

		selectedslot_effect[0] = CharSelect["effect"][0];
		selectedslot_effect[1] = CharSelect["effect"][1];

		charslotlabel = OutlinedText(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::JAMBALAYA);
		charslotlabel.change_text(get_slot_text());

		update_buttons();

		if (characters_count > 0)
		{
			if (selected_character < characters_count)
				update_selected_character();
			else
				select_last_slot();
		}

		if (Configuration::get().get_auto_login())
		{
			SelectCharPicPacket(
				Configuration::get().get_auto_pic(),
				Configuration::get().get_auto_cid()
			).dispatch();
		}

	}

	void UICharSelect::draw(float inter) const
	{
		UIElement::draw_sprites(inter);

		version.draw(position + version_pos - Point<int16_t>(0, 5));

		charslot.draw(position + charslot_pos - Point<int16_t>(0, charslot_y));
		charslotlabel.draw(position + charslot_pos + Point<int16_t>(113, 5 - charslot_y));

		for (Sprite sprite : world_sprites)
			sprite.draw(position, inter);

		std::string total = pad_number_with_leading_zero(page_count);
		std::string current = pad_number_with_leading_zero(selected_page + 1);

		std::list<uint8_t> fliplist = { 3, 4, 5, 9, 10, 11 };

		for (uint8_t i = 0; i < PAGESIZE; i++)
		{
			uint8_t index = i + selected_page * PAGESIZE;
			// 按**绝对**角色索引判断镜像：原本该表是为“每页 12 槽、4 行”版式定的
			// （第 2/4 行镜像）。改为按绝对索引判断后，这些角色无论落在哪一页都保持既有朝向，
			// 不因每页槽位数变化而改变既有表现。
			bool flip_character = std::find(fliplist.begin(), fliplist.end(), index) != fliplist.end();
			bool selectedslot = index == selected_character;

			if (index < characters_count)
			{
				// 统一以 position 为唯一参照原点：槽位局部坐标 + position。
				// （get_character_slot_pos 返回局部坐标；按钮的 bounds() 由 UIElement
				//   负责叠加 position，故那里不能重复加。）
				Point<int16_t> charpos = position + get_character_slot_pos(i, true);
				DrawArgument chararg = DrawArgument(charpos, flip_character);

				nametags[index].draw(charpos + Point<int16_t>(2, 1));

				const StatsEntry& character_stats = characters[index].stats;

				if (selectedslot)
				{
					selectedslot_effect[1].draw(charpos + Point<int16_t>(-5, 16), inter);
					charinfo.draw(position);

					std::string levelstr = std::to_string(character_stats.stats[MapleStat::Id::LEVEL]);
					int8_t lvy = 19;
					int16_t lvx = levelset.draw("201", position + levelPos + Point<int16_t>(-1, lvy));

					levelset.draw('l', position + levelPos + Point<int16_t>(-lvx - 3, lvy - 1));
					namelabel.draw(position + namePos);

					for (size_t i = 0; i < InfoLabel::NUM_LABELS; i++)
					{
						Point<int16_t> labelpos = position + get_infolabel_pos(i);
						infolabels[i].draw(labelpos);
					}
				}

				uint8_t j = 0;
				uint16_t job = character_stats.stats[MapleStat::Id::JOB];

				if (job >= 0 && job < 1000)
					j = 0;
				else if (job >= 1000 && job < 2000)
					j = 1;
				else if (job >= 2000 && job < 2200)
					j = 2;
				else
					j = 0;

				signpost[j].draw(chararg);
				charlooks[index].draw(chararg, inter);

				if (selectedslot)
					selectedslot_effect[0].draw(charpos + Point<int16_t>(-5, -298), inter);
			}
			else if (i < slots)
			{
				Point<int16_t> emptyslotpos = position + get_character_slot_pos(i);

				emptyslot_effect.draw(emptyslotpos, inter);
				emptyslot.draw(DrawArgument(emptyslotpos, flip_character));
			}
		}

		UIElement::draw_buttons(inter);

		if (tab_active)
			tab.draw(position + tab_pos[tab_index] + Point<int16_t>(0, tab_move_pos));

		if (burning_character)
		{
			burning_notice.draw(position + BtNewPos, inter);
			burning_count.draw(position + BtNewPos + burning_numPos + Point<int16_t>(8, -4));
		}

		pagebase.draw(position + pagepos);
		pagenumber.draw(current.substr(0, 1), position + pagepos + Point<int16_t>(pagenumberpos[0]));
		pagenumber.draw(current.substr(1, 1), position + pagepos + Point<int16_t>(pagenumberpos[1]));
		pagenumber.draw(total.substr(0, 1), position + pagepos + Point<int16_t>(pagenumberpos[2]));
		pagenumber.draw(total.substr(1, 1), position + pagepos + Point<int16_t>(pagenumberpos[3]));
	}

	void UICharSelect::update()
	{
		UIElement::update();

		int16_t timestep_max = CHARSLOT_Y_MAX * Constants::TIMESTEP;

		if (use_timestamp)
		{
			if (show_timestamp)
			{
				if (timestamp > 0)
				{
					timestamp -= Constants::TIMESTEP;

					if (timestamp <= timestep_max)
						charslot_y += 1;
				}
			}
			else
			{
				if (timestamp <= timestep_max)
				{
					timestamp += Constants::TIMESTEP;

					if (charslot_y >= 0)
						charslot_y -= 1;
				}
			}
		}

		if (tab_move && tab_move_pos < 4)
			tab_move_pos += 1;

		if (tab_move && tab_move_pos == 4)
			tab_move = false;

		if (!tab_move && tab_move_pos > 0)
			tab_move_pos -= 1;

		for (CharLook& charlook : charlooks)
			charlook.update(Constants::TIMESTEP);

		for (Animation& effect : selectedslot_effect)
			effect.update();

		emptyslot_effect.update();

		if (burning_character)
			burning_notice.update();
	}

	void UICharSelect::doubleclick(Point<int16_t> cursorpos)
	{
		uint16_t button_index = selected_character + Buttons::CHARACTER_SLOT0;
		auto& btit = buttons[button_index];

		if (btit->is_active() && btit->bounds(position).contains(cursorpos) && btit->get_state() == Button::State::NORMAL && button_index >= Buttons::CHARACTER_SLOT0)
			button_pressed(Buttons::BtSelect);
	}

	Cursor::State UICharSelect::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (charslot_bounds.contains(cursorpos))
		{
			if (clicked)
			{
				if (use_timestamp)
					show_timestamp = !show_timestamp;

				return Cursor::State::CLICKING;
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
				else if (btit.second->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btit.second->set_state(button_pressed(btit.first));

						if (tab_active && btit.first == tab_map[tab_index])
							btit.second->set_state(Button::State::MOUSEOVER);

						ret = Cursor::State::IDLE;
					}
					else
					{
						if (!tab_active || btit.first != tab_map[tab_index])
							ret = Cursor::State::CANCLICK;
					}
				}
			}
			else if (btit.second->get_state() == Button::State::MOUSEOVER)
			{
				if (!tab_active || btit.first != tab_map[tab_index])
					btit.second->set_state(Button::State::NORMAL);
			}
		}

		return ret;
	}

	void UICharSelect::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				button_pressed(Buttons::BtPreview);
			}
			else if (keycode == KeyAction::Id::RETURN)
			{
				if (tab_active)
				{
					uint16_t btn_index = tab_map[tab_index];

					auto& btn = buttons[btn_index];
					Button::State state = btn->get_state();

					if (state != Button::State::DISABLED)
						button_pressed(btn_index);
				}
				else
				{
					button_pressed(Buttons::BtSelect);
				}
			}
			else
			{
				if (keycode == KeyAction::Id::TAB)
				{
					uint8_t prev_tab = tab_index;

					if (!tab_active)
					{
						tab_active = true;

						if (!buttons[Buttons::BtSelect]->is_active())
							tab_index++;
					}
					else
					{
						tab_index++;

						if (tab_index > 2)
						{
							tab_active = false;
							tab_index = 0;
						}
					}

					tab_move = true;
					tab_move_pos = 0;

					auto& prev_btn = buttons[tab_map[prev_tab]];
					Button::State prev_state = prev_btn->get_state();

					if (prev_state != Button::State::DISABLED)
						prev_btn->set_state(Button::State::NORMAL);

					if (tab_active)
					{
						auto& btn = buttons[tab_map[tab_index]];
						Button::State state = btn->get_state();

						if (state != Button::State::DISABLED)
							btn->set_state(Button::State::MOUSEOVER);
					}
				}
				else
				{
					uint8_t selected_index = selected_character;
					uint8_t index_total = std::min(characters_count, static_cast<int8_t>((selected_page + 1) * PAGESIZE));

					// 键盘导航的“列数”= 每行槽位数：参考版为一行 3 槽（x = 130*(i%3) - 220），
					// 与 PAGESIZE / SLOTS_PER_ROW 保持一致。
					uint8_t COLUMNS = 3;
					uint8_t columns = std::min(index_total, COLUMNS);

					uint8_t rows = std::floor((index_total - 1) / COLUMNS) + 1;

					int32_t current_col = 0;

					if (columns > 0)
					{
						div_t div = std::div(selected_index, columns);
						current_col = div.rem;
					}

					if (keycode == KeyAction::Id::UP)
					{
						uint8_t next_index = (selected_index - COLUMNS < 0 ? (selected_index - COLUMNS) + rows * COLUMNS : selected_index - COLUMNS);

						if (next_index == selected_character)
							return;

						if (next_index >= index_total)
							button_pressed(next_index - COLUMNS + Buttons::CHARACTER_SLOT0);
						else
							button_pressed(next_index + Buttons::CHARACTER_SLOT0);
					}
					else if (keycode == KeyAction::Id::DOWN)
					{
						uint8_t next_index = (selected_index + COLUMNS >= index_total ? current_col : selected_index + COLUMNS);

						if (next_index == selected_character)
							return;

						if (next_index > index_total)
							button_pressed(next_index + COLUMNS + Buttons::CHARACTER_SLOT0);
						else
							button_pressed(next_index + Buttons::CHARACTER_SLOT0);
					}
					else if (keycode == KeyAction::Id::LEFT)
					{
						if (selected_index != 0)
						{
							selected_index--;

							if (selected_index >= (selected_page + 1) * PAGESIZE - PAGESIZE)
								button_pressed(selected_index + Buttons::CHARACTER_SLOT0);
							else
								button_pressed(Buttons::BtPageL);
						}
					}
					else if (keycode == KeyAction::Id::RIGHT)
					{
						if (selected_index != characters_count - 1)
						{
							selected_index++;

							if (selected_index < index_total)
								button_pressed(selected_index + Buttons::CHARACTER_SLOT0);
							else
								button_pressed(Buttons::BtPageR);
						}
					}
				}
			}
		}
	}

	UIElement::Type UICharSelect::get_type() const
	{
		return TYPE;
	}

	void UICharSelect::add_character(CharEntry&& character)
	{
		charlooks.emplace_back(character.look);
		nametags.emplace_back(nametag, Text::Font::A13M, character.stats.name);
		characters.emplace_back(std::forward<CharEntry>(character));

		characters_count++;
	}

	void UICharSelect::post_add_character()
	{
		bool page_matches = (characters_count - 1) / PAGESIZE == selected_page;

		if (!page_matches)
			button_pressed(Buttons::BtPageR);

		update_buttons();

		if (characters_count > 1)
			select_last_slot();
		else
			update_selected_character();

		makeactive();

		charslotlabel.change_text(get_slot_text());
	}

	void UICharSelect::remove_character(int32_t id)
	{
		for (size_t i = 0; i < characters.size(); i++)
		{
			if (characters[i].id == id)
			{
				charlooks.erase(charlooks.begin() + i);
				nametags.erase(nametags.begin() + i);
				characters.erase(characters.begin() + i);

				characters_count--;

				charslotlabel.change_text(get_slot_text());

				if (selected_page > 0)
				{
					bool page_matches = (characters_count - 1) / PAGESIZE == selected_page;

					if (!page_matches)
						button_pressed(Buttons::BtPageL);
				}

				update_buttons();

				if (selected_character < characters_count)
					update_selected_character();
				else
					select_last_slot();

				return;
			}
		}
	}

	const CharEntry& UICharSelect::get_character(int32_t id)
	{
		for (CharEntry& character : characters)
			if (character.id == id)
				return character;

		LOG(LOG_DEBUG, "Invalid character id: [" << id << "]");

		static const CharEntry null_character = { {}, {}, 0 };

		return null_character;
	}

	Button::State UICharSelect::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtSelect:
			{
				if (characters.size() > 0)
				{
					Setting<DefaultCharacter>::get().save(selected_character);
					int32_t id = characters[selected_character].id;

					switch (require_pic)
					{
						case 0:
						{
							std::function<void()> onok = [&]()
							{
								request_pic();
							};

							UI::get().emplace<UILoginNotice>(UILoginNotice::Message::PIC_REQ, onok);
							break;
						}
						case 1:
						{
							std::function<void(const std::string&)> onok = [id](const std::string& pic)
							{
								SelectCharPicPacket(pic, id).dispatch();
							};

							UI::get().emplace<UISoftKey>(onok);
							break;
						}
						case 2:
						{
							SelectCharPacket(id).dispatch();
							break;
						}
					}
				}

				break;
			}
			case Buttons::BtNew:
			{
				Sound(Sound::Name::SCROLLUP).play();

				deactivate();

				tab_index = 0;
				tab_active = false;
				tab_move = false;
				tab_move_pos = 0;

				UI::get().emplace<UIRaceSelect>();
				break;
			}
			case Buttons::BtDelete:
			{
				int32_t id = characters[selected_character].id;

				switch (require_pic)
				{
					case 0:
					{
						std::function<void()> onok = [&]()
						{
							charslotlabel.change_text(get_slot_text());
						};

						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::CHAR_DEL_FAIL_NO_PIC, onok);
						break;
					}
					case 1:
					{
						std::function<void()> oncancel = [&]()
						{
							charslotlabel.change_text(get_slot_text());
						};

						std::function<void()> onok = [&, id, oncancel]()
						{
							std::function<void(const std::string&)> onok = [&, id](const std::string& pic)
							{
								DeleteCharPicPacket(pic, id).dispatch();
							};

							UI::get().emplace<UISoftKey>(onok, oncancel);
						};

						const StatsEntry& character_stats = characters[selected_character].stats;
						uint16_t cjob = character_stats.stats[MapleStat::Id::JOB];

						if (cjob < 1000)
							UI::get().emplace<UILoginNotice>(UILoginNotice::Message::DELETE_CONFIRMATION, onok, oncancel);
						else
							UI::get().emplace<UILoginNotice>(UILoginNotice::Message::CASH_ITEMS_CONFIRM_DELETION, onok, oncancel);

						break;
					}
					case 2:
					{
						DeleteCharPacket(id).dispatch();
						break;
					}
				}

				break;
			}
			case Buttons::BtPageL:
			{
				uint8_t previous_page = selected_page;

				if (selected_page > 0)
					selected_page--;
				else
					selected_page = page_count - 1;

				if (previous_page != selected_page)
					update_buttons();

				select_last_slot();
				break;
			}
			case Buttons::BtPageR:
			{
				uint8_t previous_page = selected_page;

				if (selected_page < page_count - 1)
					selected_page++;
				else
					selected_page = 0;

				if (previous_page != selected_page)
				{
					update_buttons();

					button_pressed(Buttons::CHARACTER_SLOT0);
				}

				break;
			}
			case Buttons::BtChangePIC:
			{
				break;
			}
			case Buttons::BtResetPIC:
			{
				std::string url = Configuration::get().get_resetpic();

				ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
				break;
			}
			case Buttons::BtCharacter:
			{
				break;
			}
			case Buttons::BtPreview:
			{
				deactivate();

				Sound(Sound::Name::SCROLLUP).play();

				if (auto worldselect = UI::get().get_element<UIWorldSelect>())
					worldselect->makeactive();

				break;
			}
			default:
			{
				if (buttonid >= Buttons::CHARACTER_SLOT0)
				{
					uint8_t previous_character = selected_character;
					selected_character = buttonid - Buttons::CHARACTER_SLOT0 + selected_page * PAGESIZE;

					if (previous_character != selected_character)
					{
						if (previous_character < characters_count)
						{
							charlooks[previous_character].set_stance(Stance::Id::STAND1);
							nametags[previous_character].set_selected(false);
						}

						if (selected_character < characters_count)
							update_selected_character();
					}
				}

				break;
			}
		}

		return Button::State::NORMAL;
	}

	void UICharSelect::update_buttons()
	{
		for (uint8_t i = 0; i < PAGESIZE; i++)
		{
			uint8_t index = i + selected_page * PAGESIZE;

			if (index < characters_count)
				buttons[Buttons::CHARACTER_SLOT0 + i]->set_state(Button::State::NORMAL);
			else
				buttons[Buttons::CHARACTER_SLOT0 + i]->set_state(Button::State::DISABLED);
		}

		if (characters_count >= slots)
			buttons[Buttons::BtNew]->set_state(Button::State::DISABLED);
		else
			buttons[Buttons::BtNew]->set_state(Button::State::NORMAL);

		bool character_found = false;

		for (int8_t i = PAGESIZE - 1; i >= 0; i--)
		{
			uint8_t index = i + selected_page * PAGESIZE;

			if (index < characters_count)
			{
				character_found = true;

				break;
			}
		}

		buttons[Buttons::BtSelect]->set_active(character_found);
		buttons[Buttons::BtDelete]->set_state(character_found ? Button::State::NORMAL : Button::State::DISABLED);
	}

	void UICharSelect::update_selected_character()
	{
		Sound(Sound::Name::CHARSELECT).play();

		if (charlooks.size() > selected_character)
		{
			charlooks[selected_character].set_stance(Stance::Id::WALK1);
			nametags[selected_character].set_selected(true);

			const StatsEntry& character_stats = characters[selected_character].stats;

			namelabel.change_text(character_stats.name);

			for (size_t i = 0; i < InfoLabel::NUM_LABELS; i++)
				infolabels[i].change_text(get_infolabel(i, character_stats));
		}
	}

	void UICharSelect::select_last_slot()
	{
		for (int8_t i = PAGESIZE - 1; i >= 0; i--)
		{
			uint8_t index = i + selected_page * PAGESIZE;

			if (index < characters_count)
			{
				button_pressed(i + Buttons::CHARACTER_SLOT0);

				return;
			}
		}
	}

	std::string UICharSelect::get_slot_text()
	{
		if (use_timestamp)
		{
			show_timestamp = true;
			timestamp = 7 * 1000;
			charslot_y = 0;
		}

		return pad_number_with_leading_zero(characters_count) + "/" + pad_number_with_leading_zero(slots);
	}

	std::string UICharSelect::pad_number_with_leading_zero(uint8_t value) const
	{
		std::string return_val = std::to_string(value);
		return_val.insert(return_val.begin(), 2 - return_val.length(), '0');

		return return_val;
	}

	Point<int16_t> UICharSelect::get_character_slot_pos(size_t index, bool occupied) const
	{
		// 北斗 gms083 版式：每行 **3** 个槽位、横向间距 **130**
		// （依据 CUICharDetail ctor 0x602960：x = 130 * (slot % 3) - 220，
		//  相对间隔即 130；本实现原为每行 6 个，与北斗不符）。
		static constexpr int16_t SLOTS_PER_ROW = 3;
		static constexpr int16_t SLOT_STEP_X = 130;
		// 画布 → 帧的**横向刚性平移量**：帧内 x = 画布 x + CS_CANVAS_TO_FRAME_X。
		//
		// 依据链：
		//   1) CUICharDetail ctor(0x60292F) 的 `x = 130 * (i % 3) - 220` 与 y 同处
		//      **登录画布坐标系**（CreateFadeWnd 的第 6 参 bScreenCoord = 0）。
		//   2) 纵向可借 GetStepHeight(2) = -8-600*2 = -1208 推出带内偏移；
		//      横向**无法**从现有 dump 反推画布原点的绝对位置：
		//        - CLogin::Init(0x5F42CD) 不直接创建任何窗口（无 CreateWnd/CreateDlg 调用）；
		//        - CWndMan::UpdateWindowPosition(0x9E03A6) 仅重排窗口 z 序，不含坐标换算；
		//        - CWndMan::GetOrgWindow(0x48BBA5, UIOrigin) 在登录链路中未见调用点。
		//   3) 故改用实测反推：i = 0/1/2 三个槽位的「按帧内 -220 系列预测」与实测落点
		//      差值**恒为 +140**（295/425/555 对 155/285/415），说明两坐标系之间是
		//      **刚性平移**而非缩放或非线性，因此可安全固化为本常量。
		static constexpr int16_t CS_CANVAS_TO_FRAME_X = 140;
		static constexpr int16_t SLOT_X0 = -220 + CS_CANVAS_TO_FRAME_X;
		// 无“行距”概念：参考版 CUICharDetail ctor 的 y 只与占用状态有关、**与索引无关**，
		// 且每页固定 3 个槽（见 PAGESIZE 注释），故槽位只按行内索引 i 排布，不再叠加行偏移。
		// （此前按 index/3 * 行距 叠加属臆造：参考版并无该行为，且多行时第 3 行起会落到屏幕外。）

		// 节点 avatarSpace/avatarLT 在北斗数据里缺失（实测 <absent>），退化为 (0,0)
		// 会让所有槽位重叠；故按 IDA 字面量取帧内绝对坐标：
		// CUICharDetail ctor 0x602960 -> x = 130 * (slot % 3) - 220
		// 即 3 槽依次位于 -220 / -90 / +40（step 恒为 130）。
		const int16_t inset = occupied ? CS_SLOT_INSET_OCCUPIED : CS_SLOT_INSET_EMPTY;
		return Point<int16_t>(
			static_cast<int16_t>(SLOT_X0 + SLOT_STEP_X * (index % SLOTS_PER_ROW)),
			static_cast<int16_t>(CS_LIST_T + inset)
		);
	}

	Point<int16_t> UICharSelect::get_infolabel_pos(size_t index) const
	{
		switch (index)
		{
			case InfoLabel::JOB:
				return jobPos;
			case InfoLabel::STR:
				return statSTRPos;
			case InfoLabel::DEX:
				return statDEXPos;
			case InfoLabel::INT:
				return statINTPos;
			case InfoLabel::LUK:
				return statLUKPos;
			case InfoLabel::NUM_LABELS:
			default:
				break;
		}

		return Point<int16_t>();
	}

	std::string UICharSelect::get_infolabel(size_t index, StatsEntry character_stats) const
	{
		switch (index)
		{
			case InfoLabel::JOB:
				return Job(character_stats.stats[MapleStat::Id::JOB]).get_name();
			case InfoLabel::STR:
				return std::to_string(character_stats.stats[MapleStat::Id::STR]);
			case InfoLabel::DEX:
				return std::to_string(character_stats.stats[MapleStat::Id::DEX]);
			case InfoLabel::INT:
				return std::to_string(character_stats.stats[MapleStat::Id::INT]);
			case InfoLabel::LUK:
				return std::to_string(character_stats.stats[MapleStat::Id::LUK]);
			case InfoLabel::NUM_LABELS:
				break;
			default:
				break;
		}

		return "";
	}

	void UICharSelect::request_pic()
	{
		std::function<void(const std::string&)> enterpic = [&](const std::string& entered_pic)
		{
			std::function<void(const std::string&)> verifypic = [&, entered_pic](const std::string& verify_pic)
			{
				if (entered_pic == verify_pic)
				{
					RegisterPicPacket(
						characters[selected_character].id,
						entered_pic
					).dispatch();
				}
				else
				{
					std::function<void()> onreturn = [&]()
					{
						request_pic();
					};

					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::PASSWORD_IS_INCORRECT, onreturn);
				}
			};

			UI::get().emplace<UISoftKey>(verifypic, []() {}, "Please re-enter your new PIC.", Point<int16_t>(24, 0));
		};

		UI::get().emplace<UISoftKey>(enterpic, []() {}, "Your new PIC must at least be 6 characters long.", Point<int16_t>(24, 0));
	}
}