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
#include "EffectLayer.h"

#include "../Configuration.h"

namespace ms
{
	namespace
	{
		////////////////////////////////////////////////////////////////////////////////
		// 「系统设置 · 画质调整」→ 特效密度（B4-2）
		//
		// 原生滑条两端文案是「普通(快) / 好(慢)」：低档换帧数、高档换观感。这里把滑条
		// 值（ui.sysopt.quality，0..100）折成同一个 EffectLayer 内同屏特效的条数上限，
		// 区间 8..100；超限的新特效不入队，已在放的照常放完 —— 只挡新增，不会出现
		// 整屏特效突然消失。默认 98 档上限 98，正常打怪够不到，等于不设限。
		//
		// 阈值与区间没有原生依据（原生具体怎么降档没取证），是按用户决策「画质接
		// VSync + 特效密度」落的第一版；改数值只动这一个函数。
		////////////////////////////////////////////////////////////////////////////////
		size_t effect_cap()
		{
			const uint8_t quality = Setting<SysOptQuality>::get().load();

			return static_cast<size_t>(8 + (static_cast<uint32_t>(quality) * 92 / 100));
		}
	}

	size_t EffectLayer::count() const
	{
		size_t total = 0;

		for (const auto& layer : effects)
			total += layer.second.size();

		return total;
	}

	void EffectLayer::drawbelow(Point<int16_t> position, float alpha) const
	{
		for (auto iter = effects.begin(); iter != effects.upper_bound(-1); ++iter)
			for (auto& effect : iter->second)
				effect.draw(position, alpha);
	}

	void EffectLayer::drawabove(Point<int16_t> position, float alpha) const
	{
		for (auto iter = effects.upper_bound(-1); iter != effects.end(); ++iter)
			for (auto& effect : iter->second)
				effect.draw(position, alpha);
	}

	void EffectLayer::update()
	{
		for (auto& effectlist : effects)
		{
			effectlist.second.remove_if(
				[](Effect& effect)
				{
					return effect.update();
				}
			);
		}
	}

	void EffectLayer::add(const Animation& animation, const DrawArgument& args, int8_t z, float speed)
	{
		if (count() >= effect_cap())
			return;

		effects[z].emplace_back(animation, args, speed);
	}

	void EffectLayer::add(const Animation& animation, const DrawArgument& args, int8_t z)
	{
		add(animation, args, z, 1.0f);
	}

	void EffectLayer::add(const Animation& animation, const DrawArgument& args)
	{
		add(animation, args, 0, 1.0f);
	}

	void EffectLayer::add(const Animation& animation)
	{
		add(animation, {}, 0, 1.0f);
	}
}