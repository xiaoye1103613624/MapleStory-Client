
//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//////////////////////////////////////////////////////////////////////////////////
#include "ItemTooltip.h"

#include "../../Constants.h"

#include "../../Data/ItemData.h"

#include "../../Util/Locale.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// 2026-11-16 ͼ2 �����ض��꣨frame 290x116����
		// ͼ���� 68x68 ǳ��(194,194,206) @ ����(10,32)��ͼ�� 2x �޷� 64 ���С�
		constexpr int16_t ICON_BOX_SIZE = 68;
		constexpr int16_t ICON_MAX = 64;
		constexpr float ICON_BOX_SCALE = 2.0f;
		constexpr GLfloat ICON_BOX_R = 194.0f / 255.0f;
		constexpr GLfloat ICON_BOX_G = 194.0f / 255.0f;
		constexpr GLfloat ICON_BOX_B = 206.0f / 255.0f;
		constexpr int16_t NAME_X = 8;
		constexpr int16_t NAME_Y = 8;
		constexpr int16_t QUAL_Y = 24;
		constexpr int16_t BOX_X = 4;
		constexpr int16_t BOX_Y = 26;
		constexpr int16_t DESC_DX = 82;
		constexpr int16_t DESC_DY = 3;
		constexpr int16_t FRAME_W = 262;
		// ����ê�㣺����� pos+(-6) ��� fillwidth+28������ = FRAME_W/2+8��
		constexpr int16_t NAME_CENTER_X = FRAME_W / 2 + 8;
	}

	ItemTooltip::ItemTooltip()
	{
		itemid = 0;
		iconbase_box = ColorBox(ICON_BOX_SIZE, ICON_BOX_SIZE,
			Color(ICON_BOX_R, ICON_BOX_G, ICON_BOX_B, 1.0f), 1.0f);
	}

	bool ItemTooltip::set_item(int32_t iid)
	{
		if (itemid == iid)
			return false;

		itemid = iid;

		if (itemid == 0)
			return false;

		const ItemData& idata = ItemData::get(itemid);

		itemicon = idata.get_icon(idata.get_id() / 10000 == 238);
		untradable = idata.is_untradable();
		unique = idata.is_unique();

		std::string quality = "";

		if (unique && untradable)
			quality = Locale::text("equip.trade.both");
		else if (unique)
			quality = Locale::text("equip.trade.unique");
		else if (untradable)
			quality = Locale::text("equip.trade.untradable");

		// ���ƾ��У��û� 2026-11-16 ָ�������������֣�ͼ2 ʵ�⣩��
		name = Text(Text::Font::A14B, Text::Alignment::CENTER, Color::Name::WHITE, idata.get_name(), FRAME_W);
		desc = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, idata.get_desc(), 185);
		qual = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::ORANGE, quality, FRAME_W);

		fillwidth = FRAME_W;
		fillheight = 86 + name.height();
		descdelta = desc.height() - 80;

		if (descdelta > 0)
			fillheight += descdelta;

		return true;
	}

	void ItemTooltip::draw_item_icon(Point<int16_t> pos) const
	{
		iconbase_box.draw(pos);

		if (!itemicon.is_valid())
			return;

		float s = ICON_BOX_SCALE;
		{
			float sx = static_cast<float>(ICON_MAX) / static_cast<float>(itemicon.width());
			float sy = static_cast<float>(ICON_MAX) / static_cast<float>(itemicon.height());
			float m = sx < sy ? sx : sy;
			float fit = m < 1.0f ? m : 1.0f;
			s = s * fit;
		}

		const Point<int16_t> ivorg = itemicon.get_origin();
		const int16_t hw = static_cast<int16_t>(itemicon.width() * s) / 2;
		const int16_t hh = static_cast<int16_t>(itemicon.height() * s) / 2;
		const Point<int16_t> iconpos(
			pos.x() + ICON_BOX_SIZE / 2 + static_cast<int16_t>(ivorg.x() * s) - hw,
			pos.y() + ICON_BOX_SIZE / 2 + static_cast<int16_t>(ivorg.y() * s) - hh
		);

		itemicon.draw(DrawArgument(iconpos, s, s));
	}

	void ItemTooltip::draw(Point<int16_t> pos) const
	{
		if (itemid == 0)
			return;

		int16_t max_width = Constants::Constants::get().get_viewwidth();
		int16_t max_height = Constants::Constants::get().get_viewheight();
		int16_t cur_width = pos.x() + fillwidth + 32;
		int16_t cur_height = pos.y() + fillheight + 40;

		int16_t adj_x = cur_width - max_width;
		int16_t adj_y = cur_height - max_height;

		int16_t adj_d = descdelta > 0 ? descdelta : 0;
		int16_t adj_t = (untradable || unique) ? 19 : 0;

		if (adj_x > 0)
			pos.shift_x(adj_x * -1);

		if (adj_y > 0)
			pos.shift_y(adj_y * -1);

		// �û� m01377 �ڣ���ʮ��֯�ƣ���װ�� tooltip ���ȱ���ھ�
		backdrop.draw(pos + Point<int16_t>(-6, -6), fillwidth + 28, fillheight + adj_t + adj_d + 18, false);
		name.draw(pos + Point<int16_t>(NAME_CENTER_X, NAME_Y));

		if (untradable || unique)
			qual.draw(pos + Point<int16_t>(NAME_CENTER_X, QUAL_Y));

		pos.shift(BOX_X, BOX_Y + adj_t);

		draw_item_icon(pos);
		desc.draw(pos + Point<int16_t>(DESC_DX, DESC_DY));
	}
}
