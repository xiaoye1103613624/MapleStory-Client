//////////////////////////////////////////////////////////////////////////////////
//	Admin Block panel shell
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <vector>
#include <string>

namespace ms
{
	class UIAdmin : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ADMIN;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIAdmin();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_position) override;
		UIElement::Type get_type() const override;

		void set_message(const std::string& msg);

	private:
		struct GMQuickEntry
		{
			Text text;
			Point<int16_t> pos;
			std::string cmd;
		};

		bool has_assets = false;
		Texture backgrnd;
		Text msg_label;
		std::vector<GMQuickEntry> gm_quick; // GM 命令快捷（无参数常用命令），点击派发
	};
}
