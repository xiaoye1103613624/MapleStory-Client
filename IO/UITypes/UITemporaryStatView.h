////////////////////////////////////////////////////////////////////////////////
//	W5 TemporaryStatView — 本地 buff 明细板（与 UIBuffList 分工：列表取消 vs 明细）
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <map>
#include <string>
#include <vector>

namespace ms
{
	class UITemporaryStatView : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::TEMPORARYSTATVIEW;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UITemporaryStatView();

		void draw(float alpha) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

	private:
		struct Row
		{
			int32_t skillid = 0;
			int16_t value = 0;
			int32_t remain_ms = 0;
			std::string title;
		};

		void rebuild_rows();

		Texture back_icon;
		bool has_assets = false;
		Text title_label;
		std::vector<Text> line_labels;
		std::vector<Row> rows;
		// skillid → remain ms（本地倒计时）
		std::map<int32_t, int32_t> remain_by_skill;
	};
}
