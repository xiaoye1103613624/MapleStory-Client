//////////////////////////////////////////////////////////////////////////////////
// UIDamageSkinPicker.cpp — 见 .h。纯文本列表，数据源 DamageSkinState。
//////////////////////////////////////////////////////////////////////////////////
#include "UIDamageSkinPicker.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../../Net/Packets/DamageSkinPackets.h"
#include "../../Util/UiAssetResolve.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIDamageSkinPicker::UIDamageSkinPicker()
		: title(Text::Font::A13M, Text::Alignment::LEFT, Color::Name::YELLOW, u8"伤害皮肤"),
		  hint(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "")
	{
		dimension = Point<int16_t>(WIN_W, WIN_H);
		dragarea = Point<int16_t>(WIN_W, 24);
		layout_policy = LayoutPolicy::SavedPosClamped;
#ifdef USE_NX
		// 复用已入库的通用窗体底图（与魔女森林同款），拉伸填满窗口，避免白字裸叠在游戏画面上。
		nl::node root = nl::nx::UI["UIWindowCN.img"]["CN_50100"];
		if (!UiAssetResolve::usable(root)) root = UiAssetResolve::window("Admin")["Block"];
		if (UiAssetResolve::usable(root["backgrnd"]))
		{
			backgrnd = Texture(root["backgrnd"]);
			has_assets = true;
		}
#endif
		rebuild();
	}

	void UIDamageSkinPicker::rebuild()
	{
		buttons.clear();
		row_ids.clear();
		row_texts.clear();

		buttons[BtClose] = std::make_unique<AreaButton>(
			Point<int16_t>(WIN_W - 24, 4), Point<int16_t>(20, 18));

		const auto& cat = DamageSkinState::catalog();
		const auto& owned = DamageSkinState::owned();
		int row = 0;
		for (const auto& e : cat)
		{
			if (row >= MAX_ROWS) break;
			const int32_t id = e.first;
			row_ids.push_back(id);
			const bool have = owned.find(id) != owned.end();
			const bool active = DamageSkinState::active() == id;
			std::string label = std::to_string(id) + "  价:" + std::to_string(e.second)
				+ (active ? "  [使用中]" : (have ? "  [已拥有]" : ""));
			row_texts.push_back(Text(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, label));

			// 每行两个可点区：购买（左）/应用（右）；无图 AreaButton。
			const int16_t y = static_cast<int16_t>(ROW_Y0 + row * ROW_H);
			buttons[static_cast<uint16_t>(BtBuy + row)] = std::make_unique<AreaButton>(
				Point<int16_t>(ROW_X, y), Point<int16_t>(60, 20));
			buttons[static_cast<uint16_t>(BtApply + row)] = std::make_unique<AreaButton>(
				Point<int16_t>(ROW_X + 70, y), Point<int16_t>(60, 20));
			++row;
		}
		dirty = false;
	}

	void UIDamageSkinPicker::draw(float inter) const
	{
		UIDragElement<PosDAMAGESKINPICKER>::draw(inter);
		if (has_assets)
		{
			const float sx = static_cast<float>(WIN_W) / std::max<int16_t>(1, backgrnd.width());
			const float sy = static_cast<float>(WIN_H) / std::max<int16_t>(1, backgrnd.height());
			backgrnd.draw(DrawArgument(position, sx, sy));
		}
		title.draw(position + Point<int16_t>(10, 4));
		hint.draw(position + Point<int16_t>(10, 20));
		for (size_t i = 0; i < row_texts.size(); ++i)
			row_texts[i].draw(position + Point<int16_t>(ROW_X + 140,
				static_cast<int16_t>(ROW_Y0 + static_cast<int16_t>(i) * ROW_H)));
	}

	void UIDamageSkinPicker::update()
	{
		UIDragElement<PosDAMAGESKINPICKER>::update();
		// 有新的操作结果 → 更新提示
		int32_t op = 0, skin = 0, mesos = 0; bool ok = false;
		if (DamageSkinState::take_last_result(op, ok, skin, mesos))
		{
			const char* what = op == 1 ? "应用" : "购买";
			hint.change_text(std::string(what) + " " + std::to_string(skin) + "："
				+ (ok ? "成功" : "失败"));
			dirty = true;
		}
		if (dirty) rebuild();
	}

	void UIDamageSkinPicker::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) deactivate();
	}

	Button::State UIDamageSkinPicker::button_pressed(uint16_t buttonid)
	{
		if (buttonid == BtClose)
		{
			deactivate();
			return Button::State::NORMAL;
		}
		if (buttonid >= BtBuy && buttonid < BtBuy + MAX_ROWS)
		{
			const size_t idx = buttonid - BtBuy;
			if (idx < row_ids.size())
				DamageSkinPurchasePacket(row_ids[idx]).dispatch();
			return Button::State::NORMAL;
		}
		if (buttonid >= BtApply && buttonid < BtApply + MAX_ROWS)
		{
			const size_t idx = buttonid - BtApply;
			if (idx < row_ids.size())
				DamageSkinApplyPacket(row_ids[idx]).dispatch();
			return Button::State::NORMAL;
		}
		return Button::State::DISABLED;
	}

	UIElement::Type UIDamageSkinPicker::get_type() const { return TYPE; }
}
