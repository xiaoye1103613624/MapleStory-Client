#pragma once

#include "../UIElement.h"

#include <nlnx/nx.hpp>

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
		void send_key(int32_t keycode, bool pressed, bool escape) override;
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
			OutlinedText highlight;
			int32_t cid = -1;
			bool is_char = false;
			bool is_world = false;
			bool use_pic = false;
			nl::node icon; // 世界旗帜 / 职业图标（来自 Login.img/ViewAllChar）
		};
		std::vector<EntryView> entry_views;
		int16_t selected;
		int32_t sel_cid = -1;
		bool sel_use_pic = false;
	};
}
