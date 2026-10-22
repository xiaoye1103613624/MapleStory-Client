#pragma once

#include "../UIElement.h"

#include "../../Graphics/Texture.h"
#include "../../Graphics/SpecialText.h"
#include "../Components/Button.h"

#include <vector>
#include <string>

namespace ms
{
	// 查看全部角色：跨世界角色总览窗口。
	// 资源来自 Login.img/ViewAllChar（backgrnd / BtEnter / BtVAC / Job / Select / WorldIcons）。
	// 数据来自 UIStateLogin::viewall_worlds（由 ViewAllCharHandler 按服务端
	// showAllCharacter / showAllCharacterInfo 填充）。
	class UIViewAllChar : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::VIEWALLCHAR;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIViewAllChar();

		void draw(float alpha) const override;
		void update() override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtVAC,
			BtEnter
		};

		struct EntryView
		{
			Point<int16_t> pos;
			OutlinedText label;
		};
		std::vector<EntryView> entry_views;
		int16_t selected;
	};
}
