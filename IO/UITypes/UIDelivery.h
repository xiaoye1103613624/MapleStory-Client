//////////////////////////////////////////////////////////////////////////////////
//	Delivery / Duey — 收件列表 + 寄件/领取出站
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <string>
#include <vector>

namespace ms
{
	struct DueyPackage
	{
		int32_t id = 0;
		std::string sender;
		int32_t mesos = 0;
		std::string message;
		int32_t itemid = 0;
	};

	class UIDelivery : public UIDragElement<PosDELIVERY>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::DELIVERY;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIDelivery();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		void doubleclick(Point<int16_t> cursorpos) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

		void set_packages(std::vector<DueyPackage> pkgs);
		void remove_package(int32_t package_id);
		void show_notice(const std::string& msg);
		void set_tab(uint8_t tab);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtClose, TabSend, TabRecv, TabTax };
		enum Tab : uint8_t { SEND = 0, RECV = 1, TAX = 2 };

		int16_t row_at(Point<int16_t> cursorpos) const;
		void claim_selected();
		void discard_selected();
		void prompt_send();

		bool has_assets;
		uint8_t tab;
		int16_t selected;
		Texture backgrnd;
		Texture send_tb;
		Texture recv_tb;
		std::vector<DueyPackage> packages;
		std::vector<Text> row_labels;
		Text notice;
	};
}
