//////////////////////////////////////////////////////////////////////////////////
// UIDamageSkinPicker — 伤害皮肤选择窗（侧栏入口）。
// 数据源 DamageSkinState（服务端 0x3746/0x3747/0x3748 下发）；不发假数据。
// 每行显示 skinId / 价格 / 拥有状态，购买发 0x3747，应用发 0x3746；结果由 0x3748 驱动提示。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"
#include "../../Util/DamageSkinState.h"

#include <cstdint>
#include <vector>

namespace ms
{
	class UIDamageSkinPicker : public UIDragElement<PosDAMAGESKINPICKER>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::DAMAGESKINPICKER;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIDamageSkinPicker();

		void draw(float inter) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Button::State button_pressed(uint16_t buttonid) override;
		UIElement::Type get_type() const override;

	private:
		enum Buttons : uint16_t
		{
			BtClose = 1,
			BtBuy = 100,   // + row index
			BtApply = 300  // + row index
		};

		static constexpr int16_t WIN_W = 360;
		static constexpr int16_t WIN_H = 380;
		static constexpr int16_t ROW_H = 28;
		static constexpr int16_t ROW_X = 12;
		static constexpr int16_t ROW_Y0 = 44;
		static constexpr int16_t MAX_ROWS = 10;

		void rebuild();
		std::vector<int32_t> row_ids;
		mutable std::vector<Text> row_texts;

		Texture backgrnd;   // 复用原生通用窗体底图（UIWindowCN/CN_50100），避免白字裸叠
		bool has_assets = false;
		Text title;
		Text hint;
		int32_t last_result_shown_skin = -1;
		bool dirty = true;
	};
}
