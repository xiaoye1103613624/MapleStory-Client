//////////////////////////////////////////////////////////////////////////////////
//	Wave A #4 · StoreBank / Fredrick 寄存窗
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <string>
#include <vector>

namespace ms
{
	struct FredrickItem
	{
		int32_t itemid = 0;
		int16_t quantity = 1;
	};

	class UIStoreBank : public UIDragElement<PosSTOREBANK>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::STOREBANK;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIStoreBank();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void load(int32_t mesos, std::vector<FredrickItem> items);
		void show_notice(const std::string& msg);
		void clear_and_close();

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtExit, BtGetAll };

		void send_exit();

		bool has_assets;
		int32_t mesos;
		Texture backgrnd;
		Texture en;
		std::vector<FredrickItem> items;
		Text mesolabel;
		Text notice;
	};
}
