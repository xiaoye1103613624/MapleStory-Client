//////////////////////////////////////////////////////////////////////////////////
// UIBeautySalon.cpp — 见 .h。
//////////////////////////////////////////////////////////////////////////////////
#include "UIBeautySalon.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../../Net/Packets/BeautyPackets.h"
#include "../../Util/UiAssetResolve.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIBeautySalon::UIBeautySalon()
		: title(Text::Font::A13M, Text::Alignment::LEFT, Color::Name::YELLOW, u8"美容院"),
		  info(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "")
	{
		dimension = Point<int16_t>(WIN_W, WIN_H);
		dragarea = Point<int16_t>(WIN_W, 24);
		layout_policy = LayoutPolicy::SavedPosClamped;
#ifdef USE_NX
		nl::node root = nl::nx::UI["UIWindowCN.img"]["CN_50100"];
		if (!UiAssetResolve::usable(root)) root = UiAssetResolve::window("Admin")["Block"];
		if (UiAssetResolve::usable(root["backgrnd"]))
		{
			backgrnd = Texture(root["backgrnd"]);
			has_assets = true;
		}
#endif
		BeautyRequestPacket().dispatch();
		rebuild();
	}

	void UIBeautySalon::rebuild()
	{
		buttons.clear();
		row_texts.clear();
		buttons[BtClose] = std::make_unique<AreaButton>(Point<int16_t>(WIN_W - 24, 4), Point<int16_t>(20, 18));
		buttons[BtTab0] = std::make_unique<AreaButton>(Point<int16_t>(8, 26), Point<int16_t>(60, 20));
		buttons[BtTab1] = std::make_unique<AreaButton>(Point<int16_t>(72, 26), Point<int16_t>(60, 20));
		buttons[BtTab2] = std::make_unique<AreaButton>(Point<int16_t>(136, 26), Point<int16_t>(60, 20));
		buttons[BtRequest] = std::make_unique<AreaButton>(Point<int16_t>(210, 26), Point<int16_t>(60, 20));
		buttons[BtUnlock] = std::make_unique<AreaButton>(Point<int16_t>(274, 26), Point<int16_t>(80, 20));

		const char* tabs[3] = { u8"发型", u8"脸型", u8"肤色" };
		info.change_text(std::string(tabs[tab]) + u8"  解锁:" + std::to_string(BeautyState::unlocked()));
		for (int i = 0; i < 6; ++i)
		{
			const BeautyState::Slot s = BeautyState::slot(tab, i);
			const bool locked = i >= BeautyState::unlocked();
			std::string label = std::to_string(i + 1) + ": "
				+ (locked ? u8"[未解锁]" : (s.saved ? std::to_string(s.id) : u8"[空]"));
			row_texts.push_back(Text(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, label));
			const int16_t y = static_cast<int16_t>(ROW_Y0 + i * ROW_H);
			buttons[static_cast<uint16_t>(BtSave + i)] = std::make_unique<AreaButton>(
				Point<int16_t>(110, y), Point<int16_t>(50, 20));
			buttons[static_cast<uint16_t>(BtApply + i)] = std::make_unique<AreaButton>(
				Point<int16_t>(164, y), Point<int16_t>(50, 20));
			buttons[static_cast<uint16_t>(BtDelete + i)] = std::make_unique<AreaButton>(
				Point<int16_t>(218, y), Point<int16_t>(50, 20));
		}
	}

	void UIBeautySalon::draw(float inter) const
	{
		UIDragElement<PosBEAUTYSALON>::draw(inter);
		if (has_assets)
		{
			const float sx = static_cast<float>(WIN_W) / std::max<int16_t>(1, backgrnd.width());
			const float sy = static_cast<float>(WIN_H) / std::max<int16_t>(1, backgrnd.height());
			backgrnd.draw(DrawArgument(position, sx, sy));
		}
		title.draw(position + Point<int16_t>(8, 4));
		info.draw(position + Point<int16_t>(8, 28));
		for (size_t i = 0; i < row_texts.size(); ++i)
			row_texts[i].draw(position + Point<int16_t>(8, static_cast<int16_t>(ROW_Y0 + i * ROW_H)));
	}

	void UIBeautySalon::update()
	{
		UIDragElement<PosBEAUTYSALON>::update();
		// 数据到达后（首次或切换）刷新列表
		static bool last_data = false;
		if (BeautyState::has_data() != last_data)
		{
			last_data = BeautyState::has_data();
			rebuild();
		}
	}

	void UIBeautySalon::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) deactivate();
	}

	Button::State UIBeautySalon::button_pressed(uint16_t id)
	{
		if (id == BtClose) { deactivate(); return Button::State::NORMAL; }
		if (id >= BtTab0 && id <= BtTab2) { tab = id - BtTab0; rebuild(); return Button::State::NORMAL; }
		if (id == BtRequest) { BeautyRequestPacket().dispatch(); return Button::State::NORMAL; }
		if (id == BtUnlock) { BeautyUnlockPacket(0).dispatch(); return Button::State::NORMAL; }
		if (id >= BtSave && id < BtSave + 6) { BeautySavePacket(id - BtSave, tab).dispatch(); return Button::State::NORMAL; }
		if (id >= BtApply && id < BtApply + 6) { BeautyApplyPacket(id - BtApply, tab).dispatch(); return Button::State::NORMAL; }
		if (id >= BtDelete && id < BtDelete + 6) { BeautyDeletePacket(id - BtDelete, tab).dispatch(); return Button::State::NORMAL; }
		return Button::State::DISABLED;
	}

	UIElement::Type UIBeautySalon::get_type() const { return TYPE; }
}
