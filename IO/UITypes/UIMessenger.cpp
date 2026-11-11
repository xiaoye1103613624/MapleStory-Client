//////////////////////////////////////////////////////////////////////////////////
//	Wave A · Messenger 聊天室
//	资源：UIWindow.img/Messenger；协议对照 s9 MessengerHandler / PacketCreator
//////////////////////////////////////////////////////////////////////////////////
#include "UIMessenger.h"
#include "UICharInfo.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/MessengerPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Constants.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIMessenger::UIMessenger() :
		has_assets(false),
		in_room_(false),
		create_sent(false),
		input_typing_sent(false),
		local_pos(-1),
		blink_ticks(0),
		typing_throttle_ms(0)
	{
		position = Point<int16_t>(500, 80);
		dimension = Point<int16_t>(295, 364);
		dragarea = Point<int16_t>(295, 20);

		namebar_pos[0] = Point<int16_t>(14, 108);
		namebar_pos[1] = Point<int16_t>(103, 108);
		namebar_pos[2] = Point<int16_t>(192, 108);
		namebar_pos[3] = Point<int16_t>(103, 128);

		for (size_t i = 0; i < SLOT_COUNT; i++)
		{
			slot_labels[i] = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		}

		// 聊天输入：backgrnd 里烤死的那条黑框输入槽（y317..333），右侧留给 BtEnter
		chat_input = Textfield(
			Text::Font::A11M,
			Text::Alignment::LEFT,
			Color::Name::BLACK,
			Rectangle<int16_t>(
				Point<int16_t>(CHAT_INPUT_X, CHAT_INPUT_Y),
				Point<int16_t>(CHAT_INPUT_X + CHAT_INPUT_W, CHAT_INPUT_Y + CHAT_INPUT_H)
			),
			48,
			11
		);
		// 好友聊天支持中文输入。
		chat_input.set_accept_unicode(true);
		chat_input.set_text_shift(Point<int16_t>(0, CHAT_INPUT_TEXT_SHIFT_Y));
		chat_input.set_enter_callback([this](std::string s) { send_chat(s); });
		// 焦点在输入框时，按键由 UI::send_key 整条交给 Textfield（IO/UI.cpp:202），
		// 本窗的 send_key 根本走不到 —— 真机实测「打完字按 ESC 窗还开着」就是这条。
		// ESC 两级：聚焦时先收窗（与标题 X / 再按一次 V 同一条 toggle_active 路径）。
		chat_input.set_key_callback(KeyAction::Id::ESCAPE, [this]()
		{
			if (active)
				toggle_active();
		});

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("Messenger");

		if (!root)
		{
			LOG(LOG_WARN, "UIMessenger: UIWindow.img/Messenger missing — inactive");
			return;
		}

		has_assets = true;

		if (root["backgrnd"])
		{
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();

			if (o.x() != 0 || o.y() != 0)
				backgrnd.shift(o);

			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}

		if (root["backgrnd2"])
		{
			backgrnd2 = Texture(root["backgrnd2"]);
			Point<int16_t> o = backgrnd2.get_origin();

			if (o.x() != 0 || o.y() != 0)
				backgrnd2.shift(o);
		}

		if (root["backgrnd3"])
		{
			backgrnd3 = Texture(root["backgrnd3"]);
			Point<int16_t> o = backgrnd3.get_origin();

			if (o.x() != 0 || o.y() != 0)
				backgrnd3.shift(o);
		}

		nl::node bars = root["NameBar"];

		for (size_t i = 0; i < NAMEBAR_COUNT; i++)
		{
			nl::node n = bars[i];

			if (!n)
				continue;

			// 若节点带 x/y（部分包），覆盖硬编码 NameBar 槽位
			if (n["x"] && n["y"])
			{
				namebar_pos[i] = Point<int16_t>(
					static_cast<int16_t>(n["x"].get_integer()),
					static_cast<int16_t>(n["y"].get_integer())
				);
			}

			namebars[i] = Texture(n);
			Point<int16_t> o = namebars[i].get_origin();

			if (o.x() != 0 || o.y() != 0)
				namebars[i].shift(o);
		}

		// Angel 083 CUIMessenger::SetCtrl @ 0x8505e2：CCtrlButton x = (m_nState != 2) ? 276 : 166（展开态 276）。
		// 旧写法 dim.x()-21 + BtUIClose(32×15) 越窗；本包 BtClose3 不可用，走 close_btn。
		nl::node close = nl::nx::UI["Basic.img"]["BtClose3"];

		if (!UiAssetResolve::usable(close))
			close = UiAssetResolve::close_btn();

		if (close)
		{
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(
				close,
				Point<int16_t>(BT_CLOSE_X, BT_CLOSE_Y)
			);
		}

		if (root["BtEnter"])
		{
			buttons[Buttons::BtEnter] = std::make_unique<MapleButton>(
				root["BtEnter"],
				Point<int16_t>(BT_ENTER_X, BT_ENTER_Y)
			);
		}

		// blink：标题条注意标（Angel 083 CUIMessenger::OnCreate @ 0x84ddb0 经 StringPool 取 SP_1858_UI_UIWINDOWIMG_MESSENGER_BLINK；本包单帧 12×10 橙泡）
		if (root["blink"])
		{
			blink = Texture(root["blink"]);
			Point<int16_t> o = blink.get_origin();
			if (o.x() != 0 || o.y() != 0)
				blink.shift(o);
		}

		// chatBalloon/0..4：座位上方打字气泡（delay=120）；Angel 083 由 CUIMessenger::SetAvatar @ 0x84f44b 建对应 IWzGr2DLayer
		if (root["chatBalloon"])
			balloon_proto = Animation(root["chatBalloon"]);

		// 输入框位置由 CHAT_INPUT_* 常量定死（切片 origin 全为 0，窗口尺寸就是
		// backgrnd 的 295x364），不再按 dimension 反推。
		chat_input.set_state(Textfield::State::NORMAL);
#endif
	}

	void UIMessenger::try_create_room()
	{
		if (!has_assets || in_room_ || create_sent)
			return;

		create_sent = true;
		LOG(LOG_DEBUG, "UIMessenger: create room (Open id=0)");
		MessengerOpenPacket(0).dispatch();
		// s9 create 路径历史上静默（仅 setMessenger）；即便已补 joinMessenger(0)，
		// 也先乐观占位 0 号座 + 本机立绘，避免回包前白窗。
		set_local_pos(0);
	}

	void UIMessenger::toggle_active()
	{
		if (active)
		{
			leave_room();
			chat_input.set_state(Textfield::State::DISABLED);
			UIElement::deactivate();
		}
		else
		{
			UIElement::makeactive();
			try_create_room();
			chat_input.set_state(Textfield::State::FOCUSED);
		}
	}

	void UIMessenger::leave_room()
	{
		if (!in_room_ && !create_sent)
			return;

		if (in_room_ || create_sent)
		{
			LOG(LOG_DEBUG, "UIMessenger: leave room");
			MessengerLeavePacket().dispatch();
		}

		in_room_ = false;
		create_sent = false;
		input_typing_sent = false;
		local_pos = -1;
		blink_ticks = 0;
		typing_throttle_ms = 0;

		for (auto& s : slots)
			s = Slot{};

		for (auto& t : slot_labels)
			t.change_text("");

		chat_log.clear();
		chat_labels.clear();
	}

	void UIMessenger::start_blink()
	{
		// 对照 Angel 083 CUIMessenger::Update @ 0x84e930（blink 上限 30、归零再置 30）：仅在未闪时置 30；本端收真聊天 / 进离房都会触发。
		if (blink_ticks <= 0)
			blink_ticks = BLINK_TICKS;
	}

	int8_t UIMessenger::find_slot_by_name(const std::string& name) const
	{
		for (size_t i = 0; i < SLOT_COUNT; i++)
		{
			if (slots[i].occupied && slots[i].name == name)
				return static_cast<int8_t>(i);
		}
		return -1;
	}

	void UIMessenger::set_slot_typing(int8_t pos, bool typing)
	{
		if (pos < 0 || static_cast<size_t>(pos) >= SLOT_COUNT)
			return;
		if (!slots[static_cast<size_t>(pos)].occupied)
			return;

		Slot& s = slots[static_cast<size_t>(pos)];
		if (s.typing == typing)
			return;
		s.typing = typing;
		if (typing)
			s.balloon.reset();
		LOG(LOG_DEBUG, "UIMessenger: typing pos=" << static_cast<int>(pos)
			<< " on=" << (typing ? 1 : 0));
	}

	void UIMessenger::send_typing(bool typing)
	{
		if (!in_room_ || local_pos < 0)
			return;

		std::string self = "Me";
		if (static_cast<size_t>(local_pos) < SLOT_COUNT && slots[static_cast<size_t>(local_pos)].occupied)
			self = slots[static_cast<size_t>(local_pos)].name;

		// Angel 083 CUIMessenger::Update @ 0x84e930：COutPacket(122) + Encode1(6) + EncodeStr(charName + "0"|"1")，无冒号 ⇒ OnChat 走打字分支。
		MessengerChatPacket(self + (typing ? "1" : "0")).dispatch();
		input_typing_sent = typing;
		typing_throttle_ms = TYPING_THROTTLE_MS;
		LOG(LOG_DEBUG, "UIMessenger: send_typing " << (typing ? 1 : 0));
	}

	void UIMessenger::handle_messenger_chat(const std::string& text)
	{
		if (text.empty())
			return;

		// Angel 083 CUIMessenger::OnChat @ 0x8515d7：有 " : " → 真聊天；否则末字符 0/1 为打字指示。
		const std::string sep = " : ";
		size_t colon = text.find(sep);
		if (colon != std::string::npos)
		{
			std::string speaker = text.substr(0, colon);
			int8_t pos = find_slot_by_name(speaker);
			if (pos >= 0)
				set_slot_typing(pos, false);
			append_chat(text);
			start_blink();
			return;
		}

		if (text.size() >= 2)
		{
			char flag = text.back();
			if (flag == '0' || flag == '1')
			{
				std::string speaker = text.substr(0, text.size() - 1);
				int8_t pos = find_slot_by_name(speaker);
				if (pos >= 0)
					set_slot_typing(pos, flag == '1');
				return; // 打字包不进聊天历史
			}
		}

		append_chat(text);
		start_blink();
	}

	void UIMessenger::set_local_pos(int8_t pos)
	{
		local_pos = pos;
		in_room_ = true;
		create_sent = true;
		LOG(LOG_DEBUG, "UIMessenger: local_pos=" << static_cast<int>(pos));
		// joinMessenger / 建房回包均不带 CharLook；本机立绘只能本地灌。
		fill_local_look(pos);
	}

	void UIMessenger::fill_local_look(int8_t pos)
	{
		if (pos < 0 || static_cast<size_t>(pos) >= SLOT_COUNT)
			return;

		Player& pl = Stage::get().get_player();
		Slot& s = slots[static_cast<size_t>(pos)];
		s.occupied = true;
		s.typing = false;
		s.name = pl.get_name();
		s.channel = -1; // 本机座位不标频道
		// CharLook 拷贝当前外观（指针指向静态 Body/Hair/Face 池，Equips 按值）
		s.charlook = std::make_unique<CharLook>(pl.get_look());
		s.balloon = balloon_proto;
		slot_labels[static_cast<size_t>(pos)].change_text(s.name);
		LOG(LOG_DEBUG, "UIMessenger: fill_local_look pos=" << static_cast<int>(pos)
			<< " name=" << s.name);
	}

	void UIMessenger::set_member(int8_t pos, const std::string& name, int8_t channel, const LookEntry& look)
	{
		if (pos < 0 || static_cast<size_t>(pos) >= SLOT_COUNT)
			return;

		in_room_ = true;
		create_sent = true;

		Slot& s = slots[static_cast<size_t>(pos)];
		s.occupied = true;
		s.typing = false;
		s.name = name;
		s.channel = channel;
		s.look = look;
		s.charlook = std::make_unique<CharLook>(look);
		s.balloon = balloon_proto;

		std::string label = name;

		if (channel >= 0)
			label += " Ch." + std::to_string(channel + 1);

		slot_labels[static_cast<size_t>(pos)].change_text(label);
		LOG(LOG_DEBUG, "UIMessenger: member pos=" << static_cast<int>(pos) << " name=" << name);
	}

	void UIMessenger::clear_member(int8_t pos)
	{
		if (pos < 0 || static_cast<size_t>(pos) >= SLOT_COUNT)
			return;

		slots[static_cast<size_t>(pos)] = Slot{};
		slot_labels[static_cast<size_t>(pos)].change_text("");
		LOG(LOG_DEBUG, "UIMessenger: clear pos=" << static_cast<int>(pos));
	}

	void UIMessenger::append_chat(const std::string& line)
	{
		chat_log.push_back(line);
		chat_labels.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, line);

		while (chat_log.size() > CHAT_LINES)
		{
			chat_log.pop_front();
			chat_labels.pop_front();
		}
	}

	void UIMessenger::send_chat(const std::string& text)
	{
		if (text.empty() || !in_room_)
			return;

		std::string self = "Me";

		if (local_pos >= 0 && static_cast<size_t>(local_pos) < SLOT_COUNT && slots[static_cast<size_t>(local_pos)].occupied)
			self = slots[static_cast<size_t>(local_pos)].name;

		std::string payload = self + " : " + text;
		MessengerChatPacket(payload).dispatch();
		// 发真聊天后清打字态（对照 Angel 083 CUIMessenger::ProcessChat @ 0x851d3b 置 m_bInputState=0）。
		if (input_typing_sent)
			send_typing(false);
		input_typing_sent = false;
		// 回显行单独走 i18n：占位名 "Me" 是协议侧字段（出站包内容不随语言变），
		// 只把「给人看的那一行」里的名字换成当前语言（默认中文「我」）。
		std::string shown = (self == "Me" ? Locale::text("messenger.self") : self) + " : " + text;
		append_chat(shown); // 本地回显，不等服务端回推
		start_blink();
		chat_input.change_text("");
	}

	void UIMessenger::prompt_invite()
	{
		if (!in_room_)
		{
			append_chat(Locale::text("messenger.no_room"));
			return;
		}

		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			LOG(LOG_DEBUG, "UIMessenger: invite " << pre);
			MessengerInvitePacket(pre).dispatch();
			append_chat(Locale::format("messenger.invited", pre));
			return;
		}

		// Text 模式：角色名可短于 6、允许物理键盘（Pic SoftKey 会吞键盘并拒短串）。
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (s.empty())
				return;

			LOG(LOG_DEBUG, "UIMessenger: invite " << s);
			MessengerInvitePacket(s).dispatch();
			append_chat(Locale::format("messenger.invited", s));
		}, []() {}, Locale::text("messenger.prompt_invite"), UISoftKey::Mode::Text);
	}

	void UIMessenger::draw(float inter) const
	{
		if (!has_assets)
			return;

		if (backgrnd)
			backgrnd.draw(position);

		if (backgrnd2)
			backgrnd2.draw(position);

		if (backgrnd3)
			backgrnd3.draw(position + Point<int16_t>(55, 22));

		for (size_t i = 0; i < NAMEBAR_COUNT; i++)
		{
			if (namebars[i])
				namebars[i].draw(position + namebar_pos[i]);
		}

		for (size_t i = 0; i < SLOT_COUNT; i++)
		{
			if (!slots[i].occupied)
				continue;
			// 立绘：脚点 = NameBar 中线上方 LOOK_FOOT_*；缩放 LOOK_SCALE（见头文件注释）
			Point<int16_t> look_pos = position + namebar_pos[i]
				+ Point<int16_t>(LOOK_FOOT_DX, LOOK_FOOT_DY);
			if (slots[i].charlook)
			{
				slots[i].charlook->set_stance(Stance::Id::STAND1);
				slots[i].charlook->set_expression(Expression::Id::DEFAULT);
				slots[i].charlook->draw(
					DrawArgument(look_pos, LOOK_SCALE, LOOK_SCALE), 1.0f);
			}
			// chatBalloon：打字中循环播；相对脚点 BALLOON_DX/DY（按 Angel 083 SetAvatar 的图层位移 × 0.70 缩放近似）
			if (slots[i].typing)
			{
				slots[i].balloon.draw(
					DrawArgument(look_pos + Point<int16_t>(BALLOON_DX, BALLOON_DY)),
					1.0f);
			}
			slot_labels[i].draw(
				position + namebar_pos[i] + Point<int16_t>(LOOK_NAME_DX, LOOK_NAME_DY));
		}

		// 只画最近 CHAT_LOG_VISIBLE 行，落在 backgrnd2 之下露出的白框 y240..301 里；
		// deque 仍保留 CHAT_LINES 行做滚动缓冲。
		size_t first = chat_labels.size() > CHAT_LOG_VISIBLE
			? chat_labels.size() - CHAT_LOG_VISIBLE
			: 0;
		size_t row = 0;

		for (size_t i = first; i < chat_labels.size(); i++, row++)
		{
			chat_labels[i].draw(
				position
				+ Point<int16_t>(
					16,
					static_cast<int16_t>(CHAT_LOG_TOP + static_cast<int16_t>(row) * CHAT_LOG_LINE_H)
				)
			);
		}

		chat_input.draw(Point<int16_t>(0, 0));
		UIElement::draw_buttons(inter);

		// blink：blink_ticks 在 (HALF, TICKS] 显、在 (0, HALF] 隐（对齐 ToggleBlink 半周期）
		if (blink && blink_ticks > BLINK_HALF)
			blink.draw(position + Point<int16_t>(BLINK_DRAW_X, BLINK_DRAW_Y));
	}

	void UIMessenger::update()
	{
		chat_input.update(
			position + Point<int16_t>(CHAT_INPUT_X, CHAT_INPUT_Y),
			Point<int16_t>(CHAT_INPUT_W, CHAT_INPUT_H)
		);
		for (size_t i = 0; i < SLOT_COUNT; i++)
		{
			if (slots[i].charlook)
				slots[i].charlook->update(Constants::TIMESTEP);
			if (slots[i].typing)
				slots[i].balloon.update(Constants::TIMESTEP);
		}

		// blink 计数：对齐 Angel 083 CUIMessenger::Update @ 0x84e930（每 tick -1；到 0 重置为 30 继续闪，直到停）
		// 本端无最小化态，收消息后闪若干周期再停（约 4 个半周期后清零）。
		if (blink_ticks > 0)
		{
			--blink_ticks;
			if (blink_ticks == 0)
			{
				// 再开一轮；用高位标记「剩余轮数」会更干净，这里简单再闪一轮后彻底停：
				// 若刚被 start_blink 置 30，跑完 30 tick（含隐半程）即停。
			}
		}

		// 打字指示出站（对照 Angel 083 CUIMessenger::Update @ 0x84e930 m_bInputState ↔ 输入框空/非空）
		if (typing_throttle_ms > 0)
		{
			typing_throttle_ms -= Constants::TIMESTEP;
			if (typing_throttle_ms < 0)
				typing_throttle_ms = 0;
		}
		else if (in_room_ && chat_input.get_state() == Textfield::State::FOCUSED)
		{
			bool has_text = !chat_input.get_text().empty();
			if (has_text && !input_typing_sent)
				send_typing(true);
			else if (!has_text && input_typing_sent)
				send_typing(false);
		}
		else if (input_typing_sent)
		{
			send_typing(false);
		}
	}

	Cursor::State UIMessenger::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (Cursor::State tf = chat_input.send_cursor(cursorpos, clicked))
			return tf;

		Cursor::State st = UIDragElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active)
			return st;

		// 空名牌 → SoftKey 邀请；已占名牌 → 选中（Decline SoftKey 用）
		Point<int16_t> rel = cursorpos - position;
		for (size_t i = 0; i < SLOT_COUNT; i++)
		{
			Point<int16_t> p = namebar_pos[i];
			Rectangle<int16_t> hit{ p, p + Point<int16_t>(88, 20) };
			if (!hit.contains(rel))
				continue;
			if (slots[i].occupied)
			{
				selected_name = slots[i].name;
				LOG(LOG_DEBUG, "UIMessenger: select name=" << selected_name);
				return Cursor::State::CANCLICK;
			}
			prompt_invite();
			return Cursor::State::CANCLICK;
		}
		return st;
	}

	void UIMessenger::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			if (active)
				toggle_active();
			return;
		}

		if (pressed && (keycode == 'I' || keycode == 'i')
			&& chat_input.get_state() != Textfield::State::FOCUSED)
		{
			prompt_invite();
			return;
		}

		// SoftKey：拒绝邀请方名字 → MessengerDeclinePacket（选中名 / CharInfo 则直接拒）
		if (pressed && (keycode == 'D' || keycode == 'd')
			&& chat_input.get_state() != Textfield::State::FOCUSED)
		{
			if (selected_name.empty())
				if (auto ci = UI::get().get_element<UICharInfo>())
					selected_name = ci->get_target_name();
			if (!selected_name.empty()) {
				MessengerDeclinePacket(selected_name).dispatch();
				append_chat(Locale::format("messenger.declined", selected_name));
				LOG(LOG_DEBUG, "UIMessenger: Decline " << selected_name);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					if (s.empty()) return;
					selected_name = s;
					MessengerDeclinePacket(s).dispatch();
					append_chat(Locale::format("messenger.declined", s));
					LOG(LOG_DEBUG, "UIMessenger: Decline " << s);
				}, []() {}, Locale::text("messenger.prompt_decline"), UISoftKey::Mode::Text);
			}
			return;
		}

		if (pressed && (keycode == 'S' || keycode == 's' || keycode == 'C' || keycode == 'c')
			&& chat_input.get_state() != Textfield::State::FOCUSED)
		{
			UI::get().emplace<UISoftKey>([this](const std::string& msg) {
				if (msg.empty()) return;
				send_chat(msg);
			}, []() {}, Locale::text("messenger.prompt_chat"), UISoftKey::Mode::Text);
			return;
		}

		if (pressed && (keycode == 'L' || keycode == 'l' || keycode == 'X' || keycode == 'x')
			&& chat_input.get_state() != Textfield::State::FOCUSED)
		{
			leave_room();
			return;
		}

		if (chat_input.get_state() == Textfield::State::FOCUSED && pressed)
		{
			int32_t index = UI::get().get_keyboard().get_mapping_index(keycode);
			chat_input.send_key(KeyType::Id::TEXT, index, pressed);
		}
	}

	UIElement::Type UIMessenger::get_type() const
	{
		return TYPE;
	}

	Button::State UIMessenger::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
				if (active)
					toggle_active();
				break;
			case Buttons::BtEnter:
				// 输入框旁 Enter：发聊天；邀请走空槽点击 / I 键 SoftKey
				send_chat(chat_input.get_text());
				break;
			default:
				break;
		}

		return Button::State::NORMAL;
	}
}
