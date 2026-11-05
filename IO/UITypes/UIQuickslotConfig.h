////////////////////////////////////////////////////////////////////////////////
//	quickslotConfig — KeyConfig「变更快捷键」：点槽再按键 → CHANGE_QUICKSLOT(183)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../KeyConfig.h"
#include "../../Graphics/Texture.h"
#include "../../Template/EnumMap.h"

#include <array>

namespace ms
{
	// 原生 UIWindow.img/KeyConfig/quickslotConfig：底板文案已烘焙；
	// 交互 = 点 8 槽之一 → 按键盘键改触发键 → 立刻上行 183（s9 QuickslotBinding）。
	class UIQuickslotConfig : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::QUICKSLOTCONFIG;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIQuickslotConfig();

		void draw(float alpha) const override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		bool is_capturing() const { return selected_slot >= 0; }
		void capture_maple_key(uint8_t maple_key);

		void close();

	private:
		int16_t slot_at(Point<int16_t> cursorpos) const;
		void refresh_key_caps();
		void apply_slot_key(uint8_t slot, uint32_t maple_key);

		Texture backgrnd;
		Texture select_overlay;
		EnumMap<KeyConfig::Key, Texture> key_textures;
		std::array<Texture, 8> slot_caps{};

		static constexpr int16_t SLOT_X[4] = { 67, 102, 137, 172 };
		static constexpr int16_t SLOT_Y[2] = { 114, 148 };
		static constexpr int16_t SLOT_W = 28;
		static constexpr int16_t SLOT_H = 25;

		int16_t selected_slot = -1;
	};
}
