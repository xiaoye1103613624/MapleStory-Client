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
#include "../Window.h"
#include "../KeyAction.h"

#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"
#include "../Components/MapleComboBox.h"
#include "../Components/TwoSpriteButton.h"

#include "../../Audio/Audio.h"
#include "../../Configuration.h"
#include "../../Constants.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/PerfLog.h"
#include "../../Util/ResolutionTiers.h"
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

		// sButtonUOL 末位是 MapleComboBox::Type 的数字后缀（如 ".../ComboBox1"）。
		// get_string() 在节点缺失 / 非 string 类型时返回空串（nlnx/node.cpp:199-217）；
		// 空串 .back() 是 UB，std::stoi("") / 非数字字符会抛 invalid_argument，构造期崩。
		// 空串或末位非数字 → 回退 DEFAULT（同 UIGender.cpp:56 口径），合法数字路径逐字等价。
		MapleComboBox::Type type = MapleComboBox::Type::DEFAULT;
		std::string sButtonUOL = graphic["combo:resolution"]["sButtonUOL"].get_string();

		if (!sButtonUOL.empty())
		{
			char ctype = sButtonUOL.back();

			if (ctype >= '0' && ctype <= '9')
				type = static_cast<MapleComboBox::Type>(std::stoi(std::string(1, ctype)));
		}

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
	// 12 个同意/拒绝位落盘于 ui.gameopt.mask；消费方 = UIGameOpt::accepts(Option)。
	// 邀请/频道拦截已接（2026-11-06）：Party/Trade/Messenger/FamilyJoin/FamilySummon
	// 与 Whisper/Buddy/Alliance 聊天入站；未接项见 docs/features/gameopt-invite-mask/。
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
	//   画质 / 鼠标速度 / HP·MP 警报 / 画面效果 / 画面模式两勾 —— 见 PLAN-R28 §B4。
	//   两个下拉（截图保存位置 / 怪物情报）：收起态画当前项，点盒体在正下方展开
	//   条目列表，点条目即改序号（原生即「只有选择框列表选择」，不是点击循环切换）。
	////////////////////////////////////////////////////////////////////////////////

	namespace
	{
		// 两个下拉的条目文案键前缀 + 序号 = 原生注册表里存的那个值。
		//
		// 条目顺序取自原生汉化表 ijl15.dll（BeiDou-ClientV17.7）：
		//   @0x293ec 起四条以 \0 紧邻分隔 —— 不显示 / 只显示名字 / 只显示HP / 名字和HP都显示；
		//   @0x2177c 起两条紧邻 —— MapleStory文件夹 / 桌面。
		// 交叉验证：原生截帧「系统设置.png」当前显示「名字和HP都显示」且注册表
		// soMobInfo=3、「MapleStory文件夹」且 soScreenShot=0 ⇒ 序号自 0 起、顺序一致。
		constexpr const char* MOB_INFO_KEY = "sysopt.combo.mobinfo.";
		constexpr const char* SCREENSHOT_PATH_KEY = "sysopt.combo.screenshot.";

		std::string combo_item(const char* prefix, uint8_t ordinal)
		{
			return Locale::text(std::string(prefix) + std::to_string(ordinal));
		}

		// 展开的是哪一个下拉（0 = 都收起）。放在文件作用域而不是类里：
		// UIOptionMenu.h 被 4 个 TU 包含，给它加数据成员要走全量 Rebuild。
		// 窗体每次打开都由 UI::emplace 重建（UIMiscShells.cpp），构造函数里复位。
		uint16_t sysopt_open = 0;

		// 多分辨率：全部放文件作用域，不给 UIOptionMenu.h 加数据成员
		// （该类头被多个 TU 包含，加成员会踩增量构建旧 obj 坑、需整工程 Rebuild）。
		uint8_t sysopt_res_tier = 5;
		bool sysopt_res_open = false;
		// 插件 rs.cpp L2011-2025：OK 时立即落盘，窗口销毁后再切分辨率，
		// 避免模态窗内 UIStatusBar 重建竞态。这里延迟到「下一帧 update」应用。
		bool sysopt_pending_apply = false;
		int16_t sysopt_pending_w = 0;
		int16_t sysopt_pending_h = 0;
	}

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

		// 两个下拉：盒体整块 83x17（5 + 60 + 18）作为「展开/收起」的命中区。
		buttons[SCREENSHOT_PATH] = std::make_unique<AreaButton>(
			Point<int16_t>(COMBO_X, COMBO_Y_PATH),
			Point<int16_t>(COMBO_TOTAL_W, COMBO_H));
		buttons[MOB_INFO] = std::make_unique<AreaButton>(
			Point<int16_t>(COMBO_X, COMBO_Y_MOB),
			Point<int16_t>(COMBO_TOTAL_W, COMBO_H));

		// 展开后的条目命中区，行顶 = 盒体下沿 + 行高*i；收起时整排置灰不激活。
		sysopt_open = 0;

		for (uint8_t i = 0; i < SCREENSHOT_PATH_COUNT; i++)
			buttons[PATH_OPT0 + i] = std::make_unique<AreaButton>(
				Point<int16_t>(COMBO_X, COMBO_Y_PATH + COMBO_H + COMBO_H * i),
				Point<int16_t>(COMBO_TOTAL_W, COMBO_H));

		for (uint8_t i = 0; i < MOB_INFO_COUNT; i++)
			buttons[MOB_OPT0 + i] = std::make_unique<AreaButton>(
				Point<int16_t>(COMBO_X, COMBO_Y_MOB + COMBO_H + COMBO_H * i),
				Point<int16_t>(COMBO_TOTAL_W, COMBO_H));

		// 多分辨率：第三个下拉盒体 (76,343,166,18) + 9 个向上展开的条目（本端盒体顶边取 343，对齐新底图）。
		sysopt_res_open = false;
		buttons[RESOLUTION] = std::make_unique<AreaButton>(
			Point<int16_t>(RES_COMBO_X, RES_COMBO_Y),
			Point<int16_t>(RES_COMBO_W, RES_COMBO_H));

		for (uint8_t i = 0; i < RES_COUNT; i++)
			buttons[RES_OPT0 + i] = std::make_unique<AreaButton>(
				Point<int16_t>(RES_COMBO_X, RES_LIST_Y + RES_COMBO_H * i),
				Point<int16_t>(RES_COMBO_W, RES_COMBO_H));

		set_combo_rows(false, false);
		set_res_rows(false);

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

		// 多分辨率：显示真实当前档，不能恒显示 0 档（rs.cpp L1988-1996 的坑）。
		// game_tier=255 表示旧配置未显式选档，按 window.game_width/height 反查。
		const uint8_t tier = Setting<GameTier>::get().load();
		const int16_t game_w = static_cast<int16_t>(Setting<GameWidth>::get().load());
		const int16_t game_h = static_cast<int16_t>(Setting<GameHeight>::get().load());
		const int shown = ResolutionTiers::valid(tier)
			? static_cast<int>(tier)
			: ResolutionTiers::tier_from_dims(game_w, game_h);

		sysopt_res_tier = static_cast<uint8_t>(shown);
		sysopt_res_open = false;
		sysopt_pending_apply = false;
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

		draw_combo(COMBO_Y_PATH, combo_item(SCREENSHOT_PATH_KEY, screenshot_path));
		draw_combo(COMBO_Y_MOB, combo_item(MOB_INFO_KEY, mob_info));
		draw_resolution_combo();

		UIElement::draw_buttons(inter);

		// 展开的列表压在窗体（含底部确定/取消钮）之上，故排在 draw_buttons 之后。
		if (sysopt_res_open)
			draw_resolution_list();
		else if (sysopt_open == MOB_INFO)
			draw_combo_list(COMBO_Y_MOB, MOB_INFO_COUNT, MOB_INFO_KEY, mob_info);
		else if (sysopt_open == SCREENSHOT_PATH)
			draw_combo_list(COMBO_Y_PATH, SCREENSHOT_PATH_COUNT, SCREENSHOT_PATH_KEY, screenshot_path);
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

		// 当前项文字限制在盒体内部（左内边距 6px，右预留箭头 18px + 4px），
		// 避免长文案（如「MapleStory文件夹」）溢出选择框。
		Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK,
			option, COMBO_W - 5)
			.draw(Point<int16_t>(position.x() + COMBO_X + 6, position.y() + y + 1));
	}

	void UISysOpt::draw_combo_list(int16_t y, uint8_t count, const char* key_prefix, uint8_t selected) const
	{
		const Point<int16_t> lt(position.x() + COMBO_X, position.y() + y + COMBO_H);

		// 三层：外框 / 内底 / 当前序号那一行的高亮。配色沿用引擎自带 MapleComboBox
		// 的那一套（DUSTYGRAY + GALLERY + GRAYOLIVE）—— 原生展开态没有截帧依据，
		// 不自造观感；行高取盒体贴图的 17，与收起态同一把尺。
		ColorBox frame(COMBO_TOTAL_W, COMBO_H * count + 2, Color::Name::DUSTYGRAY, 1.0f);
		ColorBox inner(COMBO_TOTAL_W - 2, COMBO_H * count, Color::Name::GALLERY, 1.0f);

		frame.draw(lt);
		inner.draw(lt + Point<int16_t>(1, 1));

		if (selected < count)
		{
			ColorBox current(COMBO_TOTAL_W - 2, COMBO_H - 2, Color::Name::GRAYOLIVE, 1.0f);

			current.draw(lt + Point<int16_t>(1, 1 + COMBO_H * selected));
		}

		for (uint8_t i = 0; i < count; i++)
		{
			Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK,
				combo_item(key_prefix, i))
				.draw(lt + Point<int16_t>(6, COMBO_H * i + 1));
		}
	}

	// 多分辨率下拉：复用 Basic.img/ComboBox 三段贴图铺出 166x18 的盒体，
	// 坐标 (76,343) 对齐插件 rs.cpp L1973 同款三段贴图；本端盒体顶边取 343 贴合新底图（窗口大小行）。
	void UISysOpt::draw_resolution_combo() const
	{
		nl::node src = nl::nx::UI["Basic.img"]["ComboBox"]["normal"][0];

		if (!UiAssetResolve::usable(src))
			return;

		Texture left(src);
		Texture mid(nl::nx::UI["Basic.img"]["ComboBox"]["normal"][1]);
		Texture arrow(nl::nx::UI["Basic.img"]["ComboBox"]["normal"][2]);

		if (!mid.is_valid() || !arrow.is_valid())
			return;

		Point<int16_t> lp(position.x() + RES_COMBO_X, position.y() + RES_COMBO_Y);

		left.draw(lp);
		lp.shift_x(left.width());

		int16_t mid_w = mid.width();

		if (mid_w < 1)
			mid_w = 1;

		const int16_t fill_w = RES_COMBO_W - left.width() - arrow.width();
		int16_t drawn = 0;

		while (drawn < fill_w)
		{
			mid.draw(lp);
			lp.shift_x(mid_w);
			drawn += mid_w;
		}

		arrow.draw(lp);

		Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK,
			Locale::text(std::string("sysopt.combo.resolution.") + std::to_string(sysopt_res_tier)),
			RES_COMBO_W - 24)
			.draw(position + Point<int16_t>(RES_COMBO_X + 6, RES_COMBO_Y + 2));
	}

	// 9 条档位向上展开（y=181..343），完整落在 299x396 底板内，不会盖住底部按钮。
	void UISysOpt::draw_resolution_list() const
	{
		const Point<int16_t> lt(position.x() + RES_COMBO_X, position.y() + RES_LIST_Y);
		const int16_t list_h = RES_COUNT * RES_COMBO_H;

		ColorBox frame(RES_COMBO_W, list_h + 2, Color::Name::DUSTYGRAY, 1.0f);
		ColorBox inner(RES_COMBO_W - 2, list_h, Color::Name::GALLERY, 1.0f);

		frame.draw(lt);
		inner.draw(lt + Point<int16_t>(1, 1));

		if (sysopt_res_tier < RES_COUNT)
		{
			ColorBox current(RES_COMBO_W - 2, RES_COMBO_H - 2, Color::Name::GRAYOLIVE, 1.0f);

			current.draw(lt + Point<int16_t>(1,
				static_cast<int16_t>(1 + RES_COMBO_H * sysopt_res_tier)));
		}

		for (uint8_t i = 0; i < RES_COUNT; i++)
		{
			Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK,
				Locale::text(std::string("sysopt.combo.resolution.") + std::to_string(i)))
				.draw(lt + Point<int16_t>(6, static_cast<int16_t>(RES_COMBO_H * i + 1)));
		}
	}

	void UISysOpt::set_res_rows(bool show)
	{
		for (uint8_t i = 0; i < RES_COUNT; i++)
		{
			if (const auto& row = buttons.find(RES_OPT0 + i); row != buttons.end())
			{
				row->second->set_active(show);
				row->second->set_state(Button::State::NORMAL);
			}
		}
	}

	void UISysOpt::set_combo_rows(bool show_mob, bool show_path)
	{
		for (uint8_t i = 0; i < MOB_INFO_COUNT; i++)
		{
			if (const auto& row = buttons.find(MOB_OPT0 + i); row != buttons.end())
			{
				row->second->set_active(show_mob);
				// 收起时复位，否则 MOUSEOVER 残留会让下次展开的第一下点击落空。
				row->second->set_state(Button::State::NORMAL);
			}
		}

		for (uint8_t i = 0; i < SCREENSHOT_PATH_COUNT; i++)
		{
			if (const auto& row = buttons.find(PATH_OPT0 + i); row != buttons.end())
			{
				row->second->set_active(show_path);
				row->second->set_state(Button::State::NORMAL);
			}
		}
	}

	void UISysOpt::update()
	{
		// OK 的下一帧才改 Constants：按钮回调已结束、设置窗随后 deactivate，
		// 再下一帧 Window::check_events 完成 initwindow → reinit → UI 重锚全链。
		if (sysopt_pending_apply)
		{
			sysopt_pending_apply = false;
			ResolutionTiers::apply_viewsize(sysopt_pending_w, sysopt_pending_h);
			LOG(LOG_INFO, "[MR] applied game viewport " << sysopt_pending_w
				<< "x" << sysopt_pending_h << " tier=" << unsigned(sysopt_res_tier));
			deactivate();
		}

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
		{
			// 点盒体只切展开态，不改值；展开另一个时先收起这一个。
			const bool reopen = (sysopt_open != buttonid);

			sysopt_open = reopen ? buttonid : 0;
			sysopt_res_open = false;
			set_res_rows(false);
			set_combo_rows(sysopt_open == Buttons::MOB_INFO, sysopt_open == Buttons::SCREENSHOT_PATH);

			LOG(LOG_INFO, "UISysOpt: combo " << buttonid << " " << (sysopt_open ? "expanded" : "collapsed"));

			return Button::State::IDENTITY;
		}
		case Buttons::MOB_OPT0:
		case Buttons::MOB_OPT1:
		case Buttons::MOB_OPT2:
		case Buttons::MOB_OPT3:
			mob_info = static_cast<uint8_t>(buttonid - Buttons::MOB_OPT0);

			sysopt_open = 0;
			set_combo_rows(false, false);

			LOG(LOG_INFO, "UISysOpt: mob info ordinal " << unsigned(mob_info));

			return Button::State::IDENTITY;
		case Buttons::PATH_OPT0:
		case Buttons::PATH_OPT1:
			screenshot_path = static_cast<uint8_t>(buttonid - Buttons::PATH_OPT0);

			sysopt_open = 0;
			set_combo_rows(false, false);

			LOG(LOG_INFO, "UISysOpt: screenshot path ordinal " << unsigned(screenshot_path));

			return Button::State::IDENTITY;
		case Buttons::RESOLUTION:
		{
			// 第三个下拉：与另两个互斥，展开时收起它们。
			sysopt_res_open = !sysopt_res_open;
			sysopt_open = 0;
			set_combo_rows(false, false);
			set_res_rows(sysopt_res_open);

			LOG(LOG_INFO, "[MR] UISysOpt resolution combo "
				<< (sysopt_res_open ? "expanded" : "collapsed"));

			return Button::State::IDENTITY;
		}
		case Buttons::RES_OPT0:
		case Buttons::RES_OPT1:
		case Buttons::RES_OPT2:
		case Buttons::RES_OPT3:
		case Buttons::RES_OPT4:
		case Buttons::RES_OPT5:
		case Buttons::RES_OPT6:
		case Buttons::RES_OPT7:
		case Buttons::RES_OPT8:
			sysopt_res_tier = static_cast<uint8_t>(buttonid - Buttons::RES_OPT0);
			sysopt_res_open = false;
			set_res_rows(false);

			LOG(LOG_INFO, "[MR] UISysOpt resolution tier " << unsigned(sysopt_res_tier));

			return Button::State::IDENTITY;
		case Buttons::OK:
			commit();
			// 不在此处 deactivate：分辨率改 Constants 延迟到下一帧 update()，
			// 规避模态窗 UI 回调内触发 UIStatusBar 重建竞态（插件 rs.cpp 实测坑）。
			// commit() 已完成落盘，取消/ESC 通过 sysopt_pending_apply=false 回滚。
			return Button::State::IDENTITY;
		case Buttons::CANCEL:
			load_values();
			sysopt_res_open = false;
			set_res_rows(false);
			sysopt_pending_apply = false;
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
		// 「动过勾才切」的基准必须在覆盖之前取（R28-B4b 修）：下面那排 save() 一执行，
		// 落盘值就和当前值相等，末尾的 saved_window_mode != window_mode 恒为假，
		// 画面模式勾选从 B4 落地起就没真正切过窗口样式 —— 用户点「确定」无响应的实锤之一。
		const bool prev_window_mode = Setting<SysOptWindowMode>::get().load();

		Setting<SysOptQuality>::get().save(sliders[SL_QUALITY].get_value());
		Setting<BGMVolume>::get().save(sliders[SL_BGM].get_value());
		Setting<SFXVolume>::get().save(sliders[SL_SFX].get_value());
		Setting<SysOptMouseSpeed>::get().save(sliders[SL_MOUSE].get_value());
		Setting<SysOptHpWarn>::get().save(sliders[SL_HP].get_value());
		Setting<SysOptMpWarn>::get().save(sliders[SL_MP].get_value());

		// Wave 1-1.1「报警阈值上行」：把本地百分比换算成 s9 挡位（step/20=百分比，
		// 每挡 5%，服务端 HpMpAlertService 限幅 0..19）后发 SET_HPMPALERT(0x1000)。
		// 未登录（无角色）时不发；服务端不在场只是收不到，本地闪铃仍按落盘值工作。
		if (Stage::get().get_player().get_oid() > 0)
		{
			const int hp_pct = sliders[SL_HP].get_value();
			const int mp_pct = sliders[SL_MP].get_value();
			// 百分比 -> 挡位：pct/5 就近取整并钳到 0..19（19=95%，20 不可达）
			auto to_step = [](int pct) -> uint8_t
			{
				int step = (pct + 2) / 5;
				if (step < 0) step = 0;
				if (step > 19) step = 19;
				return static_cast<uint8_t>(step);
			};
			SetHpMpAlertPacket(to_step(hp_pct), to_step(mp_pct)).dispatch();

			// 真机验收判据（perf 通道，Release 可见）：点系统设置「确定」
			// （进游戏态）应出 [PERF] HPA out hp=X mp=Y pct=A/B。
			perf::mark("HPA", std::string("out hp=") + std::to_string(to_step(hp_pct))
				+ " mp=" + std::to_string(to_step(mp_pct))
				+ " pct=" + std::to_string(hp_pct) + "/" + std::to_string(mp_pct));
		}
		Setting<SysOptSkillEffects>::get().save(skill_effects);
		Setting<SysOptWindowMode>::get().save(window_mode);
		Setting<SysOptAltEnter>::get().save(alt_enter);
		Setting<SysOptMobInfo>::get().save(mob_info);
		Setting<SysOptScreenshotPath>::get().save(screenshot_path);

		////////////////////////////////////////////////////////////////////////////////
		// 多分辨率（用户决策 D2/D4）：档位合法性 + 桌面校验 → 写 3 个新键 → 排队延迟应用。
		// 超桌面时回落到最大可用档并给 WARN；连 800x600 都放不下则拒绝并保留原档。
		// 写盘由本函数末尾的 Configuration::get().save() 统一完成（旧空壳 SELECT_RES 漏了它）。
		////////////////////////////////////////////////////////////////////////////////
		const int16_t max_w = Configuration::get().get_max_width();
		const int16_t max_h = Configuration::get().get_max_height();
		const int16_t fallback_w = static_cast<int16_t>(Setting<GameWidth>::get().load());
		const int16_t fallback_h = static_cast<int16_t>(Setting<GameHeight>::get().load());

		int16_t game_w = 0;
		int16_t game_h = 0;
		const int chosen = ResolutionTiers::resolve_game_size(
			sysopt_res_tier, fallback_w, fallback_h, max_w, max_h, game_w, game_h);

		if (chosen < 0)
		{
			LOG(LOG_WARN, "[MR] resolution tier " << unsigned(sysopt_res_tier)
				<< " exceeds desktop " << max_w << "x" << max_h
				<< " and no tier fits; keeping previous setting");
		}
		else
		{
			if (chosen != sysopt_res_tier)
			{
				LOG(LOG_WARN, "[MR] resolution tier " << unsigned(sysopt_res_tier)
					<< " exceeds desktop; fall back to tier " << chosen);
				sysopt_res_tier = static_cast<uint8_t>(chosen);
			}

			Setting<GameTier>::get().save(sysopt_res_tier);
			Setting<GameWidth>::get().save(static_cast<uint16_t>(game_w));
			Setting<GameHeight>::get().save(static_cast<uint16_t>(game_h));

			sysopt_pending_w = game_w;
			sysopt_pending_h = game_h;
			sysopt_pending_apply = true;
		}

		////////////////////////////////////////////////////////////////////////////////
		// 画面模式（B4-4）
		//
		// 「窗口化模式」与 window.fullscreen 是同一件事的两面，启动真源是
		// window.fullscreen（Window::init 启动时把 window_mode 回灌成 !fullscreen，
		// 保证设置窗打开时的勾与实际一致）。这里只在用户真的动过这一勾时才切：
		// prev_window_mode（覆盖前取的）与当前值一比就是"改没改"，不必给类加字段
		// （改公共头类布局会踩增量构建的旧 obj 坑，见 memory:msbuild-header-layout-stale-objs）。
		////////////////////////////////////////////////////////////////////////////////
		if (prev_window_mode != window_mode)
		{
			Window::get().set_fullscreen(!window_mode);

			LOG(LOG_INFO, "UISysOpt: window mode " << (window_mode ? "windowed" : "fullscreen")
				<< " -> fullscreen=" << Window::get().is_fullscreen());
		}

		// 音量立刻生效（与 UI::send_focus 走同一对入口）。
		Music::set_bgmvolume(sliders[SL_BGM].get_value());
		Sound::set_sfxvolume(sliders[SL_SFX].get_value());

		////////////////////////////////////////////////////////////////////////////////
		// 画质调整（B4-2）
		//
		// 原生滑条两端是「普通(快) / 好(慢)」，即低档换帧数、高档换观感。这一档接两处：
		//   特效密度 —— Graphics/EffectLayer.cpp 每次 add 现读 ui.sysopt.quality，无需回灌；
		//   VSync    —— 过半即开，写进 window.vsync 并立刻落 SDL_GL_SetSwapInterval。
		// window.vsync 从此是这一档的镜像：它原本只有配置文件一个入口，没有别的写方，
		// 接上之后仍然只有一处真值来源（画质滑条），不存在双写打架。
		//
		// 「过半开 VSync」这条阈值没有原生依据（原生具体在哪一档开没取证），
		// 是按用户决策「画质接 VSync + 特效密度」落的第一版。
		////////////////////////////////////////////////////////////////////////////////
		const bool vsync = sliders[SL_QUALITY].get_value() >= 50;

		Setting<VSync>::get().save(vsync);
		Window::get().apply_vsync();

		Configuration::get().save();

		LOG(LOG_INFO, "UISysOpt: saved bgm=" << static_cast<int>(sliders[SL_BGM].get_value())
			<< " sfx=" << static_cast<int>(sliders[SL_SFX].get_value())
			<< " quality=" << static_cast<int>(sliders[SL_QUALITY].get_value())
			<< " vsync=" << vsync);
	}

	void UISysOpt::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed)
			return;

		if (escape)
		{
			// ESC 与「取消」同义：未落盘的分辨率排队必须清掉，不能下一帧偷偷生效。
			sysopt_pending_apply = false;
			sysopt_res_open = false;
			set_res_rows(false);
			deactivate();
		}
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