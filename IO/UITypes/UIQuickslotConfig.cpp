////////////////////////////////////////////////////////////////////////////////
//	quickslotConfig — 点槽 + 按键 → CHANGE_QUICKSLOT(183)
////////////////////////////////////////////////////////////////////////////////
#include "UIQuickslotConfig.h"

#include "../UI.h"
#include "../../Constants.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIQuickslotConfig::UIQuickslotConfig()
	{
		dimension = Point<int16_t>(266, 238);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("KeyConfig");
		if (!root)
			root = nl::nx::UI["UIWindow.img"]["KeyConfig"];
		nl::node qs = root["quickslotConfig"];
		if (qs && qs["backgrnd"])
		{
			backgrnd = Texture(qs["backgrnd"]);
			dimension = backgrnd.get_dimensions();
		}
		if (qs && qs["BtQuickSetting"] && qs["BtQuickSetting"]["normal"])
			select_overlay = Texture(qs["BtQuickSetting"]["normal"]["0"]);

		nl::node key = qs && qs["key"] ? qs["key"] : root["key"];
		if (key)
		{
			auto load = [&](KeyConfig::Key k, int idx) {
				if (key[idx])
					key_textures[k] = Texture(key[idx]);
			};
			load(KeyConfig::Key::NUM1, 2); load(KeyConfig::Key::NUM2, 3);
			load(KeyConfig::Key::NUM3, 4); load(KeyConfig::Key::NUM4, 5);
			load(KeyConfig::Key::NUM5, 6); load(KeyConfig::Key::NUM6, 7);
			load(KeyConfig::Key::NUM7, 8); load(KeyConfig::Key::NUM8, 9);
			load(KeyConfig::Key::NUM9, 10); load(KeyConfig::Key::NUM0, 11);
			load(KeyConfig::Key::MINUS, 12); load(KeyConfig::Key::EQUAL, 13);
			load(KeyConfig::Key::Q, 16); load(KeyConfig::Key::W, 17);
			load(KeyConfig::Key::E, 18); load(KeyConfig::Key::R, 19);
			load(KeyConfig::Key::T, 20); load(KeyConfig::Key::Y, 21);
			load(KeyConfig::Key::U, 22); load(KeyConfig::Key::I, 23);
			load(KeyConfig::Key::O, 24); load(KeyConfig::Key::P, 25);
			load(KeyConfig::Key::LEFT_BRACKET, 26); load(KeyConfig::Key::RIGHT_BRACKET, 27);
			load(KeyConfig::Key::LEFT_CONTROL, 29); load(KeyConfig::Key::RIGHT_CONTROL, 29);
			load(KeyConfig::Key::A, 30); load(KeyConfig::Key::S, 31);
			load(KeyConfig::Key::D, 32); load(KeyConfig::Key::F, 33);
			load(KeyConfig::Key::G, 34); load(KeyConfig::Key::H, 35);
			load(KeyConfig::Key::J, 36); load(KeyConfig::Key::K, 37);
			load(KeyConfig::Key::L, 38); load(KeyConfig::Key::SEMICOLON, 39);
			load(KeyConfig::Key::APOSTROPHE, 40); load(KeyConfig::Key::GRAVE_ACCENT, 41);
			load(KeyConfig::Key::LEFT_SHIFT, 42); load(KeyConfig::Key::RIGHT_SHIFT, 42);
			load(KeyConfig::Key::BACKSLASH, 43);
			load(KeyConfig::Key::Z, 44); load(KeyConfig::Key::X, 45);
			load(KeyConfig::Key::C, 46); load(KeyConfig::Key::V, 47);
			load(KeyConfig::Key::B, 48); load(KeyConfig::Key::N, 49);
			load(KeyConfig::Key::M, 50); load(KeyConfig::Key::COMMA, 51);
			load(KeyConfig::Key::PERIOD, 52);
			load(KeyConfig::Key::LEFT_ALT, 56); load(KeyConfig::Key::RIGHT_ALT, 56);
			load(KeyConfig::Key::SPACE, 57);
			load(KeyConfig::Key::F1, 59); load(KeyConfig::Key::F2, 60);
			load(KeyConfig::Key::F3, 61); load(KeyConfig::Key::F4, 62);
			load(KeyConfig::Key::F5, 63); load(KeyConfig::Key::F6, 64);
			load(KeyConfig::Key::F7, 65); load(KeyConfig::Key::F8, 66);
			load(KeyConfig::Key::F9, 67); load(KeyConfig::Key::F10, 68);
			load(KeyConfig::Key::HOME, 71); load(KeyConfig::Key::PAGE_UP, 73);
			load(KeyConfig::Key::END, 79); load(KeyConfig::Key::PAGE_DOWN, 81);
			load(KeyConfig::Key::INSERT, 82); load(KeyConfig::Key::DELETE, 83);
			load(KeyConfig::Key::ESCAPE, 1); load(KeyConfig::Key::SCROLL_LOCK, 70);
			load(KeyConfig::Key::F11, 87); load(KeyConfig::Key::F12, 88);
		}
#endif

		int16_t vw = Constants::Constants::get().get_viewwidth();
		int16_t vh = Constants::Constants::get().get_viewheight();
		position = Point<int16_t>(
			static_cast<int16_t>((vw - dimension.x()) / 2),
			static_cast<int16_t>((vh - dimension.y()) / 2));

		refresh_key_caps();
		LOG(LOG_INFO, "UIQuickslotConfig open dim=" << dimension.x() << "x" << dimension.y());
	}

	void UIQuickslotConfig::refresh_key_caps()
	{
		const Keyboard& kb = UI::get().get_keyboard();
		for (uint8_t i = 0; i < 8; ++i)
		{
			uint32_t mk = kb.get_quickslot_key(i);
			KeyConfig::Key k = KeyConfig::actionbyid(static_cast<int32_t>(mk));
			if (k < KeyConfig::Key::LENGTH && key_textures[k])
				slot_caps[i] = key_textures[k];
			else
				slot_caps[i] = Texture();
		}
	}

	int16_t UIQuickslotConfig::slot_at(Point<int16_t> cursorpos) const
	{
		Point<int16_t> rel = cursorpos - position;
		for (int row = 0; row < 2; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				int16_t x = SLOT_X[col];
				int16_t y = SLOT_Y[row];
				Rectangle<int16_t> cell(
					Point<int16_t>(x, y),
					Point<int16_t>(static_cast<int16_t>(x + SLOT_W), static_cast<int16_t>(y + SLOT_H)));
				if (cell.contains(rel))
					return static_cast<int16_t>(row * 4 + col);
			}
		}
		return -1;
	}

	void UIQuickslotConfig::apply_slot_key(uint8_t slot, uint32_t maple_key)
	{
		if (slot >= 8)
			return;
		// 同键占用其它槽时互换，避免 8 触发键撞车
		Keyboard& kb = UI::get().get_keyboard();
		std::array<uint32_t, 8> keys = kb.quickslots();
		for (uint8_t i = 0; i < 8; ++i)
		{
			if (i != slot && keys[i] == maple_key)
				keys[i] = keys[slot];
		}
		keys[slot] = maple_key;
		for (uint8_t i = 0; i < 8; ++i)
			kb.set_quickslot(i, keys[i]);
		kb.commit_quickslots();
		refresh_key_caps();
		selected_slot = -1;
		LOG(LOG_INFO, "UIQuickslotConfig slot=" << (int)slot << " key=" << maple_key << " → CHANGE_QUICKSLOT");
	}

	void UIQuickslotConfig::capture_maple_key(uint8_t maple_key)
	{
		if (selected_slot < 0 || maple_key == 0)
			return;
		if (maple_key == KeyConfig::Key::ESCAPE)
		{
			selected_slot = -1;
			return;
		}
		apply_slot_key(static_cast<uint8_t>(selected_slot), maple_key);
	}

	void UIQuickslotConfig::draw(float) const
	{
		if (backgrnd)
			backgrnd.draw(position);

		for (uint8_t i = 0; i < 8; ++i)
		{
			int row = i / 4;
			int col = i % 4;
			Point<int16_t> tl = position + Point<int16_t>(SLOT_X[col], SLOT_Y[row]);
			if (slot_caps[i])
			{
				// 键帽居中落入白格
				Point<int16_t> sz = slot_caps[i].get_dimensions();
				Point<int16_t> off(
					static_cast<int16_t>((SLOT_W - sz.x()) / 2),
					static_cast<int16_t>((SLOT_H - sz.y()) / 2));
				slot_caps[i].draw(tl + off);
			}
			if (selected_slot == static_cast<int16_t>(i) && select_overlay)
			{
				Point<int16_t> osz = select_overlay.get_dimensions();
				Point<int16_t> off(
					static_cast<int16_t>((SLOT_W - osz.x()) / 2),
					static_cast<int16_t>((SLOT_H - osz.y()) / 2));
				select_overlay.draw(tl + off);
			}
		}
	}

	Cursor::State UIQuickslotConfig::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked)
		{
			int16_t slot = slot_at(cursorpos);
			if (slot >= 0)
			{
				selected_slot = slot;
				return Cursor::State::CLICKING;
			}
			// 点底板外：关窗
			if (!Rectangle<int16_t>(position, position + dimension).contains(cursorpos))
			{
				close();
				return Cursor::State::IDLE;
			}
		}
		if (slot_at(cursorpos) >= 0)
			return Cursor::State::CANCLICK;
		return Cursor::State::IDLE;
	}

	void UIQuickslotConfig::send_key(int32_t, bool pressed, bool escape)
	{
		if (!pressed)
			return;
		if (escape)
		{
			if (selected_slot >= 0)
				selected_slot = -1;
			else
				close();
		}
	}

	void UIQuickslotConfig::close()
	{
		deactivate();
	}

	UIElement::Type UIQuickslotConfig::get_type() const
	{
		return TYPE;
	}
}
