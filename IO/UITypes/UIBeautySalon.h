//////////////////////////////////////////////////////////////////////////////////
// UIBeautySalon — 美容院造型槽窗（3 tab × 6 槽）。数据源 BeautyState（0x174 DATA）。
// 纯文本列表 + AreaButton，无 avatar 预览（预览为后续增强）；功能：保存/应用/删除/解锁。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"
#include "../../Util/BeautyState.h"

#include <vector>

namespace ms
{
	class UIBeautySalon : public UIDragElement<PosBEAUTYSALON>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::BEAUTYSALON;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIBeautySalon();

		void draw(float inter) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Button::State button_pressed(uint16_t buttonid) override;
		UIElement::Type get_type() const override;

	private:
		enum Buttons : uint16_t
		{
			BtClose = 1,
			BtTab0 = 10, BtTab1, BtTab2,
			BtRequest = 20,
			BtUnlock = 21,
			BtSave = 100,    // + slot
			BtApply = 200,   // + slot
			BtDelete = 300   // + slot
		};

		static constexpr int16_t WIN_W = 380;
		static constexpr int16_t WIN_H = 320;
		static constexpr int16_t ROW_H = 26;
		static constexpr int16_t ROW_Y0 = 60;

		int32_t tab = 0;
		void rebuild();
		Texture backgrnd;   // 复用原生通用窗体底图，避免白字裸叠
		bool has_assets = false;
		mutable std::vector<Text> row_texts;
		Text title;
		Text info;
	};
}
