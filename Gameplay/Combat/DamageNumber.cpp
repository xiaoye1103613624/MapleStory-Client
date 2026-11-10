//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "DamageNumber.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif
#include <map>

namespace ms
{
	DamageNumber::DamageNumber(Type t, int64_t damage, int16_t starty, int16_t x, int32_t skin)
	{
		type = t;
		skin_id = skin;
		numscale = 1.0f;
		charset = charsets[type];

		if (skin_id != 0)
			apply_skin(skin_id, charset, type);

		if (damage > 0)
		{
			miss = false;

			std::string number = std::to_string(damage);
			firstnum = number[0];

			if (number.size() > 1)
			{
				restnum = number.substr(1);
				multiple = true;
			}
			else
			{
				restnum = "";
				multiple = false;
			}

			int16_t total = getadvance(firstnum, true);

			for (size_t i = 0; i < restnum.length(); i++)
			{
				char c = restnum[i];
				int16_t advance;

				if (i < restnum.length() - 1)
				{
					char n = restnum[i + 1];
					advance = (getadvance(c, false) + getadvance(n, false)) / 2;
				}
				else
				{
					advance = getadvance(c, false);
				}

				total += advance;
			}

			// 2^53 是 16 位数字；默认图集每字符约 24px，不缩放会横向溢出。
			// 运行时 BasicEff.img 只有 0-9 与 Miss，没有 K/M/B 等单位字符，
			// 因此按最大宽度等比缩放，既保留完整数值又不截断（notes/r001 §4）。
			constexpr int16_t MAX_DAMAGE_WIDTH = 260;

			if (total > MAX_DAMAGE_WIDTH)
				numscale = static_cast<float>(MAX_DAMAGE_WIDTH) / total;

			shift = static_cast<int16_t>(total * numscale / 2);
		}
		else
		{
			shift = charset[true].getw('M') / 2;
			miss = true;
		}

		moveobj.set_x(x);
		moveobj.set_y(starty);
		moveobj.vspeed = -0.25;
		opacity.set(1.5f);
	}

	DamageNumber::DamageNumber() : numscale(1.0f) {}

	void DamageNumber::draw(double viewx, double viewy, float alpha) const
	{
		Point<int16_t> absolute = moveobj.get_absolute(viewx, viewy, alpha);
		Point<int16_t> position = absolute - Point<int16_t>(0, shift);
		float interopc = opacity.get(alpha);

		if (miss)
		{
			charset[true].draw('M', { position, interopc });
		}
		else
		{
			charset[false].draw(firstnum, { position, numscale, numscale, interopc });

			if (multiple)
			{
				int16_t first_advance = static_cast<int16_t>(getadvance(firstnum, true) * numscale);
				position.shift_x(first_advance);

				for (size_t i = 0; i < restnum.length(); i++)
				{
					char c = restnum[i];
					Point<int16_t> yshift = { 0, static_cast<int16_t>((i % 2) ? -2 : 2) };
					charset[true].draw(c, { position + yshift, numscale, numscale, interopc });

					int16_t advance;

					if (i < restnum.length() - 1)
					{
						char n = restnum[i + 1];
						int16_t c_advance = getadvance(c, false);
						int16_t n_advance = getadvance(n, false);
						advance = (c_advance + n_advance) / 2;
					}
					else
					{
						advance = getadvance(c, false);
					}

					position.shift_x(static_cast<int16_t>(advance * numscale));
				}
			}
		}
	}

	int16_t DamageNumber::getadvance(char c, bool first) const
	{
		constexpr size_t LENGTH = 10;

		constexpr int16_t advances[LENGTH] =
		{
			24, 20, 22, 22, 24, 23, 24, 22, 24, 24
		};

		size_t index = c - 48;

		if (index < LENGTH)
		{
			int16_t advance = advances[index];

			switch (type)
			{
				case DamageNumber::Type::CRITICAL:
				{
					if (first)
						advance += 8;
					else
						advance += 4;

					break;
				}
				default:
				{
					if (first)
						advance += 2;

					break;
				}
			}

			return advance;
		}
		else
		{
			return 0;
		}
	}

	void DamageNumber::set_x(int16_t headx)
	{
		moveobj.set_x(headx);
	}

	bool DamageNumber::update()
	{
		moveobj.move();

		constexpr float FADE_STEP = Constants::TIMESTEP * 1.0f / FADE_TIME;
		opacity -= FADE_STEP;

		return opacity.last() <= 0.0f;
	}

	int16_t DamageNumber::rowheight(bool critical)
	{
		return critical ? 36 : 30;
	}

	void DamageNumber::apply_skin(int32_t skin, BoolPair<Charset>& out, Type type)
	{
		static std::map<int32_t, nl::node> skin_nodes;

		auto it = skin_nodes.find(skin);

		if (it == skin_nodes.end())
		{
			nl::node dmg = nl::nx::Effect["DamageSkin.img"][std::to_string(skin)];

			if (!dmg)
				return; // DamageSkin.img 缺失：保持默认 BasicEff

			skin_nodes[skin] = dmg;
			it = skin_nodes.find(skin);
		}

		nl::node dmg = it->second;

		// 至少要有普通数字字形，否则回退默认
		if (!dmg["NoRed1"])
			return;

		if (type == Type::CRITICAL)
		{
			out.set(false, dmg["NoCri1"], Charset::Alignment::LEFT);
			out.set(true, dmg["NoCri0"], Charset::Alignment::LEFT);
		}
		else if (type == Type::NORMAL)
		{
			out.set(false, dmg["NoRed1"], Charset::Alignment::LEFT);
			out.set(true, dmg["NoRed0"], Charset::Alignment::LEFT);
		}
		// TOPLAYER: 保持默认 BasicEff
	}

	void DamageNumber::init()
	{
		nl::node BasicEff = nl::nx::Effect["BasicEff.img"];

		charsets[DamageNumber::Type::NORMAL].set(false, BasicEff["NoRed1"], Charset::Alignment::LEFT);
		charsets[DamageNumber::Type::NORMAL].set(true, BasicEff["NoRed0"], Charset::Alignment::LEFT);
		charsets[DamageNumber::Type::CRITICAL].set(false, BasicEff["NoCri1"], Charset::Alignment::LEFT);
		charsets[DamageNumber::Type::CRITICAL].set(true, BasicEff["NoCri0"], Charset::Alignment::LEFT);
		charsets[DamageNumber::Type::TOPLAYER].set(false, BasicEff["NoViolet1"], Charset::Alignment::LEFT);
		charsets[DamageNumber::Type::TOPLAYER].set(true, BasicEff["NoViolet0"], Charset::Alignment::LEFT);
	}

	BoolPair<Charset> DamageNumber::charsets[NUM_TYPES];
}