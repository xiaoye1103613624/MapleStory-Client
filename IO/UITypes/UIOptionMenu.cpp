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
#include "UIOptionMenu.h"

#include "UIMiscShells.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../KeyAction.h"

#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"
#include "../Components/MapleComboBox.h"
#include "../Components/TwoSpriteButton.h"

#include "../../Audio/Audio.h"
#include "../../Configuration.h"
#include "../../Constants.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIOptionMenu::UIOptionMenu() : UIDragElement<PosOPTIONMENU>(), selected_tab(0)
	{
		nl::node OptionMenu = nl::nx::UI["StatusBar3.img"]["OptionMenu"];

		if (!UiAssetResolve::usable(OptionMenu))
		{
			nl::node GameOpt = UiAssetResolve::window("GameOpt");
			nl::node SysOpt = UiAssetResolve::window("SysOpt");
			nl::node fallback = UiAssetResolve::usable(GameOpt) ? GameOpt : SysOpt;

			if (!UiAssetResolve::usable(fallback) || !fallback["backgrnd"])
			{
				active = false;
				dimension = Point<int16_t>(0, 0);
				return;
			}

			sprites.emplace_back(fallback["backgrnd"]);
			dimension = Texture(fallback["backgrnd"]).get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
			return;
		}

		nl::node backgrnd = OptionMenu["backgrnd"];

		sprites.emplace_back(backgrnd);
		sprites.emplace_back(OptionMenu["backgrnd2"]);

		nl::node graphic = OptionMenu["graphic"];

		tab_background[Buttons::TAB0] = graphic["layer:backgrnd"];
		tab_background[Buttons::TAB1] = OptionMenu["sound"]["layer:backgrnd"];
		tab_background[Buttons::TAB2] = OptionMenu["game"]["layer:backgrnd"];
		tab_background[Buttons::TAB3] = OptionMenu["invite"]["layer:backgrnd"];
		tab_background[Buttons::TAB4] = OptionMenu["screenshot"]["layer:backgrnd"];

		buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(OptionMenu["button:Cancel"]);
		buttons[Buttons::OK] = std::make_unique<MapleButton>(OptionMenu["button:OK"]);
		buttons[Buttons::UIRESET] = std::make_unique<MapleButton>(OptionMenu["button:UIReset"]);

		nl::node tab = OptionMenu["tab"];
		nl::node tab_disabled = tab["disabled"];
		nl::node tab_enabled = tab["enabled"];

		for (size_t i = Buttons::TAB0; i < Buttons::CANCEL; i++)
			buttons[i] = std::make_unique<TwoSpriteButton>(tab_disabled[i], tab_enabled[i]);

		std::string sButtonUOL = graphic["combo:resolution"]["sButtonUOL"].get_string();
		std::string ctype = std::string(1, sButtonUOL.back());
		MapleComboBox::Type type = static_cast<MapleComboBox::Type>(std::stoi(ctype));

		std::vector<std::string> resolutions =
		{
			"800 x 600 ( 4 : 3 )",
			"1024 x 768 ( 4 : 3 )",
			"1280 x 720 ( 16 : 9 )",
			"1366 x 768 ( 16 : 9 )",
			"1920 x 1080 ( 16 : 9 ) - Beta"
		};

		int16_t max_width = Configuration::get().get_max_width();
		int16_t max_height = Configuration::get().get_max_height();

		if (max_width >= 1920 && max_height >= 1200)
			resolutions.emplace_back("1920 x 1200 ( 16 : 10 ) - Beta");

		uint16_t default_option = 0;
		int16_t screen_width = Constants::Constants::get().get_viewwidth();
		int16_t screen_height = Constants::Constants::get().get_viewheight();

		switch (screen_width)
		{
		case 800:
			default_option = 0;
			break;
		case 1024:
			default_option = 1;
			break;
		case 1280:
			default_option = 2;
			break;
		case 1366:
			default_option = 3;
			break;
		case 1920:
			switch (screen_height)
			{
			case 1080:
				default_option = 4;
				break;
			case 1200:
				default_option = 5;
				break;
			}

			break;
		}

		int64_t combobox_width = graphic["combo:resolution"]["boxWidth"].get_integer();
		Point<int16_t> lt = Point<int16_t>(graphic["combo:resolution"]["lt"]);

		buttons[Buttons::SELECT_RES] = std::make_unique<MapleComboBox>(type, resolutions, default_option, position, lt, combobox_width);

		Point<int16_t> bg_dimensions = Texture(backgrnd).get_dimensions();

		dimension = bg_dimensions;
		dragarea = Point<int16_t>(bg_dimensions.x(), 20);

		change_tab(Buttons::TAB2);
	}

	void UIOptionMenu::draw(float inter) const
	{
		UIElement::draw_sprites(inter);

		if (tab_background[selected_tab].is_valid())
			tab_background[selected_tab].draw(position);

		UIElement::draw_buttons(inter);
	}

	Button::State UIOptionMenu::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::TAB0:
		case Buttons::TAB1:
		case Buttons::TAB2:
		case Buttons::TAB3:
		case Buttons::TAB4:
			change_tab(buttonid);
			return Button::State::IDENTITY;
		case Buttons::CANCEL:
			deactivate();
			return Button::State::NORMAL;
		case Buttons::OK:
			switch (selected_tab)
			{
			case Buttons::TAB0:
			{
				auto it = buttons.find(Buttons::SELECT_RES);
				if (it == buttons.end() || !it->second)
					break;

				uint16_t selected_value = it->second->get_selected();

				int16_t width = Constants::Constants::get().get_viewwidth();
				int16_t height = Constants::Constants::get().get_viewheight();

				switch (selected_value)
				{
				case 0:
					width = 800;
					height = 600;
					break;
				case 1:
					width = 1024;
					height = 768;
					break;
				case 2:
					width = 1280;
					height = 720;
					break;
				case 3:
					width = 1366;
					height = 768;
					break;
				case 4:
					width = 1920;
					height = 1080;
					break;
				case 5:
					width = 1920;
					height = 1200;
					break;
				}

				Setting<Width>::get().save(width);
				Setting<Height>::get().save(height);

				Constants::Constants::get().set_viewwidth(width);
				Constants::Constants::get().set_viewheight(height);
			}
			break;
			case Buttons::TAB1:
			case Buttons::TAB2:
			case Buttons::TAB3:
			case Buttons::TAB4:
			default:
				break;
			}

			deactivate();
			return Button::State::NORMAL;
		case Buttons::UIRESET:
		{
			// 打开 AP/SP SoftKey 壳（UIReset）
			auto reset = UI::get().get_element<UIReset>();
			if (!reset)
				reset = UI::get().emplace<UIReset>();
			else if (!reset->is_active())
				reset->makeactive();
			return Button::State::NORMAL;
		}
		case Buttons::SELECT_RES:
			if (auto it = buttons.find(Buttons::SELECT_RES); it != buttons.end() && it->second)
				it->second->toggle_pressed();
			return Button::State::NORMAL;
		default:
			return Button::State::DISABLED;
		}
	}

	Cursor::State UIOptionMenu::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(clicked, cursorpos);

		if (dragged)
			return dstate;

		auto& button = buttons[Buttons::SELECT_RES];

		if (button && button->is_pressed())
		{
			if (button->in_combobox(cursorpos))
			{
				if (Cursor::State new_state = button->send_cursor(clicked, cursorpos))
					return new_state;
			}
			else
			{
				remove_cursor();
			}
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIOptionMenu::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
				deactivate();
			else if (keycode == KeyAction::Id::RETURN)
				button_pressed(Buttons::OK);
			else if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
				|| keycode == 'O' || keycode == 'o')
			{
				UI::get().emplace<UISoftKey>([this](const std::string& tip) {
					if (tip.empty()) return;
					std::string t = tip;
					for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
					if (t == "ok" || t == "save" || t == "y" || t == "1")
						button_pressed(Buttons::OK);
					else if (t == "cancel" || t == "n" || t == "0")
						button_pressed(Buttons::CANCEL);
					else if (t.rfind("buff ", 0) == 0 || t == "buffon" || t == "buffoff") {
						bool on = (t.find("off") == std::string::npos);
						if (t.rfind("buff ", 0) == 0) {
							std::string v = t.substr(5);
							on = (v == "on" || v == "1" || v == "true");
						}
						Setting<BuffTimer>::get().save(on);
						LOG(LOG_DEBUG, "UIOptionMenu: BuffTimer=" << on);
					} else if (t.rfind("equip ", 0) == 0 || t == "equipon" || t == "equipoff") {
						bool on = (t.find("off") == std::string::npos);
						if (t.rfind("equip ", 0) == 0) {
							std::string v = t.substr(6);
							on = (v == "on" || v == "1" || v == "true");
						}
						Setting<EquipCompare>::get().save(on);
						LOG(LOG_DEBUG, "UIOptionMenu: EquipCompare=" << on);
					} else if (t.rfind("mpoints ", 0) == 0 || t.rfind("mp ", 0) == 0) {
						size_t sp = t.find(' ');
						try {
							int64_t pts = std::stoll(t.substr(sp + 1));
							if (pts >= 0) {
								Setting<MaplePointsLocal>::get().save(static_cast<uint64_t>(pts));
								LOG(LOG_DEBUG, "UIOptionMenu: MaplePointsLocal=" << pts);
							}
						} catch (...) {}
					} else if (t.find('x') != std::string::npos) {
						// WxH → 写分辨率并 OK
						size_t x = t.find('x');
						try {
							int16_t w = static_cast<int16_t>(std::stoi(t.substr(0, x)));
							int16_t h = static_cast<int16_t>(std::stoi(t.substr(x + 1)));
							if (w >= 640 && h >= 480) {
								Setting<Width>::get().save(w);
								Setting<Height>::get().save(h);
								Constants::Constants::get().set_viewwidth(w);
								Constants::Constants::get().set_viewheight(h);
								deactivate();
								LOG(LOG_DEBUG, "UIOptionMenu: SoftKey res " << w << "x" << h);
							}
						} catch (...) {}
					}
				}, []() {}, "ok|WxH|buff on|equip on|mp N");
			}
		}
	}

	UIElement::Type UIOptionMenu::get_type() const
	{
		return TYPE;
	}

	void UIOptionMenu::change_tab(uint16_t tabid)
	{
		auto it_old = buttons.find(selected_tab);
		auto it_new = buttons.find(tabid);

		if (it_old != buttons.end() && it_old->second)
			it_old->second->set_state(Button::State::NORMAL);
		if (it_new != buttons.end() && it_new->second)
			it_new->second->set_state(Button::State::PRESSED);

		selected_tab = tabid;

		auto it_res = buttons.find(Buttons::SELECT_RES);

		if (it_res == buttons.end() || !it_res->second)
			return;

		switch (tabid)
		{
		case Buttons::TAB0:
			it_res->second->set_active(true);
			break;
		case Buttons::TAB1:
		case Buttons::TAB2:
		case Buttons::TAB3:
		case Buttons::TAB4:
			it_res->second->set_active(false);
			break;
		default:
			break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// SettingsSlider —— Basic.img/Slider 拼出的横向设置滑条
	////////////////////////////////////////////////////////////////////////////////

	void SettingsSlider::init(int16_t x0, int16_t x1, int16_t y)
	{
		nl::node slider = nl::nx::UI["Basic.img"]["Slider"];

		track_left = Texture(slider["left"]);
		track_mid = Texture(slider["mid"]);
		track_right = Texture(slider["right"]);
		thumb = Texture(slider["thumbNormal"]);

		track_x0 = x0;
		track_x1 = x1;
		row_y = y;
	}

	void SettingsSlider::set_value(uint8_t v)
	{
		value = v > 100 ? 100 : v;
	}

	uint8_t SettingsSlider::get_value() const
	{
		return value;
	}

	int16_t SettingsSlider::thumb_x() const
	{
		// 滑钮左沿的可行程 = [track_x0, track_x1 - THUMB_W + 1]
		int16_t span = track_x1 - track_x0 + 1 - THUMB_W;

		if (span < 1)
			span = 1;

		return static_cast<int16_t>(track_x0 + (static_cast<int32_t>(value) * span) / 100);
	}

	void SettingsSlider::draw(Point<int16_t> parent) const
	{
		// Slider 三件的 origin 是 (0, -2)，绘制点比视觉顶边低 2；实测轨道视觉
		// 顶边在 row_y + 3，所以绘制 y = row_y + 1（见 _settings_map2.txt 行剖）。
		int16_t track_y = row_y + 1;

		Point<int16_t> lp(parent.x() + track_x0, parent.y() + track_y);

		track_left.draw(lp);
		lp.shift_x(track_left.width());

		int16_t mid_last = track_x1 - track_right.width() + 1;
		int16_t mid_w = track_mid.width();

		if (mid_w < 1)
			mid_w = 1;

		while (lp.x() - parent.x() < mid_last)
		{
			track_mid.draw(lp);
			lp.shift_x(mid_w);
		}

		track_right.draw(Point<int16_t>(parent.x() + mid_last, parent.y() + track_y));

		// thumbNormal 的 origin 是 (0, 0)，直接落在实测行上。
		thumb.draw(Point<int16_t>(parent.x() + thumb_x(), parent.y() + row_y));
	}

	Rectangle<int16_t> SettingsSlider::bounds(Point<int16_t> parent) const
	{
		Point<int16_t> lt(parent.x() + track_x0, parent.y() + row_y);
		Point<int16_t> rb(parent.x() + track_x1 + 1, parent.y() + row_y + thumb.height());

		return Rectangle<int16_t>(lt, rb);
	}

	bool SettingsSlider::apply_cursor(Point<int16_t> parent, Point<int16_t> cursor, bool pressed)
	{
		if (!pressed)
		{
			dragging = false;
			return false;
		}

		// 只有「按下时就在轨道里」或「已经在拖」才接管本次光标。
		if (!dragging && !bounds(parent).contains(cursor))
			return false;

		dragging = true;

		int16_t span = track_x1 - track_x0 + 1 - THUMB_W;

		if (span < 1)
			span = 1;

		// 光标对准滑钮中心，而不是让滑钮左上角跟着指针。
		int32_t local = cursor.x() - parent.x() - THUMB_W / 2 - track_x0;
		int32_t v = (local * 100) / span;

		if (v < 0)
			v = 0;
		else if (v > 100)
			v = 100;

		value = static_cast<uint8_t>(v);

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	// UIGameOpt —— v83「游戏设置」
	//
	// 预留说明（B7-4）：这 12 个同意/拒绝位**已落盘并可查询**，但拦截点尚未接线。
	// 邀请回包的收发在 Net/Handlers/PlayerHandlers.cpp（本轮由并行会话持有，
	// 见 docs/.../notes 的禁改清单），故本版只把 UI 与存档做齐：
	//   消费方 = UIGameOpt::accepts(Option)，接线时只需在对应 *_REQ 的 handler
	//   里查一次并回绝包，UI 侧无需再改。
	////////////////////////////////////////////////////////////////////////////////

	UIGameOpt::UIGameOpt()
		: UIDragElement<PosGAMEOPT>(), mask(0xFFF), dirty(false)
	{
		nl::node GameOpt = UiAssetResolve::window("GameOpt");

		if (!UiAssetResolve::usable(GameOpt) || !GameOpt["backgrnd"])
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			UiAssetResolve::diag_once("uigameopt bg", "UIWindow.img/GameOpt/backgrnd missing");
			return;
		}

		backgrnd = Texture(GameOpt["backgrnd"]);
		dimension = backgrnd.get_dimensions();
		dragarea = Point<int16_t>(dimension.x(), 20);

		nl::node Basic = nl::nx::UI["Basic.img"];
		nl::node checkbox = Basic["CheckBox"];

		if (UiAssetResolve::usable(checkbox))
		{
			cb_off = Texture(checkbox[0]);
			cb_on = Texture(checkbox[1]);
		}
		else
		{
			UiAssetResolve::diag_once("uigameopt cb", "Basic.img/CheckBox missing");
		}

		mask = Setting<GameOptMask>::get().load();

		// 12 个 12x12 命中区：勾选态由 mask 现算，所以钮本身用不可见的 AreaButton，
		// 避免 TwoSpriteButton 的「悬浮=选中」语义把未勾的行画成已勾。
		for (uint16_t i = 0; i < OPTION_COUNT; i++)
			buttons[i] = std::make_unique<AreaButton>(
				Point<int16_t>(CHECK_X, static_cast<int16_t>(CHECK_TOP + i * CHECK_PITCH)),
				Point<int16_t>(12, 12));

		// try_maple_btn / MapleButton 吃的是**按钮根节点**（内部自己取 normal/pressed/
		// mouseOver/disabled）；传 ["normal"][0] 那片叶子会让四态全空、画不出任何东西，
		// 而且 usable() 仍为真，连 diag 都不会报——实测就是这么丢的 确定/取消。
		nl::node bt_ok = Basic["BtOK2"];
		nl::node bt_cancel = Basic["BtCancel2"];

		// try_maple_btn 会把 normal 态 origin 加回去：实测格位是**贴图左上角**，
		// 而 MapleButton 把 position 当锚点（draw = pos - origin）。
		if (!try_maple_btn(buttons, OPTIONOK, bt_ok, Point<int16_t>(OK_X, BTN_Y), "uigameopt ok"))
			UiAssetResolve::diag_once("uigameopt ok", "Basic.img/BtOK2 missing");

		if (!try_maple_btn(buttons, OPTIONCANCEL, bt_cancel, Point<int16_t>(CANCEL_X, BTN_Y), "uigameopt cancel"))
			UiAssetResolve::diag_once("uigameopt cancel", "Basic.img/BtCancel2 missing");
	}

	void UIGameOpt::draw(float inter) const
	{
		backgrnd.draw(position);

		for (uint16_t i = 0; i < OPTION_COUNT; i++)
		{
			const Texture& state = (mask & (1 << i)) ? cb_on : cb_off;

			if (!state.is_valid())
				continue;

			state.draw(Point<int16_t>(position.x() + CHECK_X,
				position.y() + CHECK_TOP + static_cast<int16_t>(i * CHECK_PITCH)));
		}

		UIElement::draw_buttons(inter);
	}

	void UIGameOpt::update()
	{
		UIElement::update();
	}

	Button::State UIGameOpt::button_pressed(uint16_t buttonid)
	{
		if (buttonid < OPTION_COUNT)
		{
			mask ^= static_cast<uint16_t>(1 << buttonid);
			dirty = true;

			// IDENTITY => 不改 Button 状态（AreaButton 本来就无态），勾选由 mask 决定。
			return Button::State::IDENTITY;
		}

		switch (buttonid)
		{
		case Buttons::OPTIONOK:
			commit();
			deactivate();
			return Button::State::NORMAL;
		case Buttons::OPTIONCANCEL:
			// 取消 = 丢弃未提交的勾选，回到打开时的存档值。
			mask = Setting<GameOptMask>::get().load();
			dirty = false;
			deactivate();
			return Button::State::NORMAL;
		default:
			return Button::State::DISABLED;
		}
	}

	void UIGameOpt::commit()
	{
		if (!dirty)
			return;

		Setting<GameOptMask>::get().save(mask);
		// Setting<> 只改内存项；落盘要显式 save()（见 Configuration.h 顶部约定）。
		Configuration::get().save();

		dirty = false;

		LOG(LOG_INFO, "UIGameOpt: accept mask saved = " << mask);
	}

	void UIGameOpt::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed)
			return;

		if (escape)
			deactivate();
	}

	Cursor::State UIGameOpt::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		return UIDragElement::send_cursor(clicked, cursorpos);
	}

	bool UIGameOpt::accepts(Option option)
	{
		if (option >= OPTION_COUNT)
			return true;

		uint16_t saved = Setting<GameOptMask>::get().load();

		return (saved & (1 << option)) != 0;
	}

	UIElement::Type UIGameOpt::get_type() const
	{
		return TYPE;
	}

	////////////////////////////////////////////////////////////////////////////////
	// UISysOpt —— v83「系统设置」
	//
	// 消费方现状（B7-4）：
	//   背景音乐 / 效果音响 两滑条 + 两个静音勾 —— 接 Audio（真生效）。
	//   画质 / 鼠标速度 / HP·MP 警报 / 画面效果 / 画面模式两勾 / 两个下拉 ——
	//   本包无对应管线，仅按 ui.sysopt.* 落盘，窗体可开可关、值可改可存。
	//   下拉的**展开列表**未做：MapleComboBox 的命中区按绝对屏幕坐标算，父窗一拖
	//   就错位（open-issues 已记），且原生展开态截帧尚未取证，故只画收起态。
	////////////////////////////////////////////////////////////////////////////////

	UISysOpt::UISysOpt()
		: UIDragElement<PosSYSOPT>(), bgm_stash(50), sfx_stash(50)
		, skill_effects(true), window_mode(false), alt_enter(false)
		, mob_info(0), screenshot_path(0)
	{
		nl::node SysOpt = UiAssetResolve::window("SysOpt");

		if (!UiAssetResolve::usable(SysOpt) || !SysOpt["backgrnd"])
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			UiAssetResolve::diag_once("uisysopt bg", "UIWindow.img/SysOpt/backgrnd missing");
			return;
		}

		backgrnd = Texture(SysOpt["backgrnd"]);
		dimension = backgrnd.get_dimensions();
		dragarea = Point<int16_t>(dimension.x(), 20);

		nl::node Basic = nl::nx::UI["Basic.img"];
		nl::node checkbox = Basic["CheckBox"];

		if (UiAssetResolve::usable(checkbox))
		{
			cb_off = Texture(checkbox[0]);
			cb_on = Texture(checkbox[1]);
		}
		else
		{
			UiAssetResolve::diag_once("uisysopt cb", "Basic.img/CheckBox missing");
		}

		for (uint8_t i = 0; i < SL_COUNT; i++)
			sliders[i].init(TRACK_X0, TRACK_X1, SLIDER_ROW[i]);

		// 5 个勾选命中区（12x12），位置同 CHECK_BOX 表。
		for (uint8_t i = 0; i < 5; i++)
		{
			static constexpr uint16_t ids[] = { MUTE_BGM, MUTE_SFX, SKILL_EFFECTS, WINDOW_MODE, ALT_ENTER };

			buttons[ids[i]] = std::make_unique<AreaButton>(
				Point<int16_t>(CHECK_BOX[i][0], CHECK_BOX[i][1]), Point<int16_t>(12, 12));
		}

		// 两个下拉：只画收起态，整块 83x17（5 + 60 + 18）作为命中区预留。
		buttons[SCREENSHOT_PATH] = std::make_unique<AreaButton>(
			Point<int16_t>(COMBO_X, COMBO_Y_PATH),
			Point<int16_t>(5 + COMBO_W + 18, 17));
		buttons[MOB_INFO] = std::make_unique<AreaButton>(
			Point<int16_t>(COMBO_X, COMBO_Y_MOB),
			Point<int16_t>(5 + COMBO_W + 18, 17));

		// 同 UIGameOpt：传按钮根节点，不是 ["normal"][0] 叶子。
		nl::node bt_ok = Basic["BtOK2"];
		nl::node bt_cancel = Basic["BtCancel2"];

		if (!try_maple_btn(buttons, OK, bt_ok, Point<int16_t>(OK_X, BTN_Y), "uisysopt ok"))
			UiAssetResolve::diag_once("uisysopt ok", "Basic.img/BtOK2 missing");

		if (!try_maple_btn(buttons, CANCEL, bt_cancel, Point<int16_t>(CANCEL_X, BTN_Y), "uisysopt cancel"))
			UiAssetResolve::diag_once("uisysopt cancel", "Basic.img/BtCancel2 missing");

		load_values();
	}

	void UISysOpt::load_values()
	{
		sliders[SL_QUALITY].set_value(Setting<SysOptQuality>::get().load());
		sliders[SL_BGM].set_value(Setting<BGMVolume>::get().load());
		sliders[SL_SFX].set_value(Setting<SFXVolume>::get().load());
		sliders[SL_MOUSE].set_value(Setting<SysOptMouseSpeed>::get().load());
		sliders[SL_HP].set_value(Setting<SysOptHpWarn>::get().load());
		sliders[SL_MP].set_value(Setting<SysOptMpWarn>::get().load());

		// 静音 = 该路音量为 0；取消静音时恢复到静音前的值。
		mute_state(bgm_stash, sliders[SL_BGM]);
		mute_state(sfx_stash, sliders[SL_SFX]);

		skill_effects = Setting<SysOptSkillEffects>::get().load();
		window_mode = Setting<SysOptWindowMode>::get().load();
		alt_enter = Setting<SysOptAltEnter>::get().load();
		mob_info = Setting<SysOptMobInfo>::get().load();
		screenshot_path = Setting<SysOptScreenshotPath>::get().load();
	}

	void UISysOpt::mute_state(uint8_t& stash, SettingsSlider& slider)
	{
		if (slider.get_value() == 0)
		{
			if (stash == 0)
				stash = 50;
		}
		else
		{
			stash = slider.get_value();
		}
	}

	void UISysOpt::draw(float inter) const
	{
		backgrnd.draw(position);

		for (uint8_t i = 0; i < SL_COUNT; i++)
			sliders[i].draw(position);

		// 勾选态与 CHECK_BOX 表同序：背景音乐静音 / 效果音响静音 / 画面效果 /
		// 画面模式（窗口化）/ 画面模式（ALT+回车可切换）。
		bool checked[5] = {
			sliders[SL_BGM].get_value() == 0,
			sliders[SL_SFX].get_value() == 0,
			skill_effects,
			window_mode,
			alt_enter
		};

		for (uint8_t i = 0; i < 5; i++)
		{
			const Texture& state = checked[i] ? cb_on : cb_off;

			if (!state.is_valid())
				continue;

			state.draw(Point<int16_t>(position.x() + CHECK_BOX[i][0], position.y() + CHECK_BOX[i][1]));
		}

		draw_combo(COMBO_Y_PATH, Locale::text("sysopt.combo.screenshot_folder"));
		draw_combo(COMBO_Y_MOB, Locale::text("sysopt.combo.mob_name_hp"));

		UIElement::draw_buttons(inter);
	}

	void UISysOpt::draw_combo(int16_t y, const std::string& option) const
	{
		nl::node src = nl::nx::UI["Basic.img"]["ComboBox"]["normal"][0];

		if (!UiAssetResolve::usable(src))
			return;

		Texture left(src);
		Texture mid(nl::nx::UI["Basic.img"]["ComboBox"]["normal"][1]);
		Texture arrow(nl::nx::UI["Basic.img"]["ComboBox"]["normal"][2]);

		Point<int16_t> lp(position.x() + COMBO_X, position.y() + y);

		left.draw(lp);
		lp.shift_x(left.width());

		int16_t mid_w = mid.width();

		if (mid_w < 1)
			mid_w = 1;

		int16_t drawn = 0;

		while (drawn < COMBO_W)
		{
			mid.draw(lp);
			lp.shift_x(mid_w);
			drawn += mid_w;
		}

		arrow.draw(lp);

		Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK, option)
			.draw(Point<int16_t>(position.x() + COMBO_X + 6, position.y() + y + 1));
	}

	void UISysOpt::update()
	{
		UIElement::update();
	}

	Button::State UISysOpt::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::MUTE_BGM:
			toggle_mute(bgm_stash, sliders[SL_BGM]);
			return Button::State::IDENTITY;
		case Buttons::MUTE_SFX:
			toggle_mute(sfx_stash, sliders[SL_SFX]);
			return Button::State::IDENTITY;
		case Buttons::SKILL_EFFECTS:
			skill_effects = !skill_effects;
			return Button::State::IDENTITY;
		case Buttons::WINDOW_MODE:
			window_mode = !window_mode;
			return Button::State::IDENTITY;
		case Buttons::ALT_ENTER:
			alt_enter = !alt_enter;
			return Button::State::IDENTITY;
		case Buttons::MOB_INFO:
		case Buttons::SCREENSHOT_PATH:
			// 展开列表预留：见类顶注释。点一下不改值，只给一条可观测日志。
			LOG(LOG_INFO, "UISysOpt: combo " << buttonid << " expanded list not implemented");
			return Button::State::IDENTITY;
		case Buttons::OK:
			commit();
			deactivate();
			return Button::State::NORMAL;
		case Buttons::CANCEL:
			load_values();
			deactivate();
			return Button::State::NORMAL;
		default:
			return Button::State::DISABLED;
		}
	}

	void UISysOpt::toggle_mute(uint8_t& stash, SettingsSlider& slider)
	{
		if (slider.get_value() != 0)
		{
			stash = slider.get_value();
			slider.set_value(0);
		}
		else
		{
			if (stash == 0)
				stash = 50;

			slider.set_value(stash);
		}
	}

	void UISysOpt::commit()
	{
		Setting<SysOptQuality>::get().save(sliders[SL_QUALITY].get_value());
		Setting<BGMVolume>::get().save(sliders[SL_BGM].get_value());
		Setting<SFXVolume>::get().save(sliders[SL_SFX].get_value());
		Setting<SysOptMouseSpeed>::get().save(sliders[SL_MOUSE].get_value());
		Setting<SysOptHpWarn>::get().save(sliders[SL_HP].get_value());
		Setting<SysOptMpWarn>::get().save(sliders[SL_MP].get_value());
		Setting<SysOptSkillEffects>::get().save(skill_effects);
		Setting<SysOptWindowMode>::get().save(window_mode);
		Setting<SysOptAltEnter>::get().save(alt_enter);
		Setting<SysOptMobInfo>::get().save(mob_info);
		Setting<SysOptScreenshotPath>::get().save(screenshot_path);

		// 音量立刻生效（与 UI::send_focus 走同一对入口）。
		Music::set_bgmvolume(sliders[SL_BGM].get_value());
		Sound::set_sfxvolume(sliders[SL_SFX].get_value());

		Configuration::get().save();

		LOG(LOG_INFO, "UISysOpt: saved bgm=" << static_cast<int>(sliders[SL_BGM].get_value())
			<< " sfx=" << static_cast<int>(sliders[SL_SFX].get_value()));
	}

	void UISysOpt::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed)
			return;

		if (escape)
			deactivate();
	}

	Cursor::State UISysOpt::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(clicked, cursorpos);

		if (dragged)
			return dstate;

		// 滑条自持拖动：命中即吞掉本次光标，不再走 Button 的「悬浮→点击」派发。
		// apply_cursor 在 clicked=false 时自己就把 dragging 清了，无需额外 release。
		for (uint8_t i = 0; i < SL_COUNT; i++)
		{
			if (sliders[i].apply_cursor(position, cursorpos, clicked))
				return Cursor::State::CLICKING;
		}

		return dstate;
	}

	UIElement::Type UISysOpt::get_type() const
	{
		return TYPE;
	}
}