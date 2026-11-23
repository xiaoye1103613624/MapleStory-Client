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
#include "SkillUseEffect.h"

#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 第二轮：技能 WZ 额外施放特效节点（prepare/keydown/keydownend/screen/
	// special/tile，D3 跨 img 实证为 CANVAS 帧动画）。走 Effect 基类 =
	// show_attack_effect，尊重 SysOptSkillEffects 画面开关。
	ExtraNodeUseEffect::ExtraNodeUseEffect(nl::node src) : effect(src) {}

	void ExtraNodeUseEffect::apply(Char& target) const
	{
		effect.apply(target);
	}

	void CompositeUseEffect::add(std::unique_ptr<SkillUseEffect> e)
	{
		effects.push_back(std::move(e));
	}

	void CompositeUseEffect::apply(Char& target) const
	{
		for (const auto& e : effects)
			e->apply(target);
	}

	// 按节点名从 Effect/BasicEff.img 加载施放动画。
	// 与 SingleUseEffect 的差别：后者读 Skill.img/<skill>/effect，而瞬移(Flying/Teleport)
	// 与二段跳(Flying) 的动画是原生直接挂在 BasicEff.img 上的，Skill.img 里没有 effect 节点。
	// Effect 基类会读取节点自身的 z 并调用 show_attack_effect，保留画面效果开关语义。
	BasicEffUseEffect::BasicEffUseEffect(const std::string& path)
		: effect(nl::nx::Effect["BasicEff.img"][path])
	{
		// 关键节点日志：确认动画节点是否命中（Release 下 LOG 为 no-op，无额外开销）
		LOG(LOG_DEBUG, "[SkillUseEffect] BasicEffUseEffect load path=" << path
			<< " valid=" << (nl::nx::Effect["BasicEff.img"][path] ? 1 : 0));
	}

	void BasicEffUseEffect::apply(Char& target) const
	{
		effect.apply(target);
	}

	SingleUseEffect::SingleUseEffect(nl::node src) : effect(src["effect"]) {}

	void SingleUseEffect::apply(Char& target) const
	{
		effect.apply(target);
	}

	TwoHandedUseEffect::TwoHandedUseEffect(nl::node src) : effects(src["effect"]["0"], src["effect"]["1"]) {}

	void TwoHandedUseEffect::apply(Char& target) const
	{
		bool twohanded = target.is_twohanded();
		effects[twohanded].apply(target);
	}

	MultiUseEffect::MultiUseEffect(nl::node src)
	{
		int8_t no = -1;
		nl::node sub = src["effect"];

		while (sub)
		{
			effects.push_back(sub);

			no++;
			sub = src["effect" + std::to_string(no)];
		}
	}

	void MultiUseEffect::apply(Char& target) const
	{
		for (auto& effect : effects)
			effect.apply(target);
	}

	ByLevelUseEffect::ByLevelUseEffect(nl::node src)
	{
		for (auto sub : src["CharLevel"])
		{
			auto level = string_conversion::or_zero<uint16_t>(sub.name());
			effects.emplace(level, sub["effect"]);
		}
	}

	void ByLevelUseEffect::apply(Char& target) const
	{
		if (effects.empty())
			return;

		uint16_t level = target.get_level();
		auto iter = effects.begin();
		for (; iter != effects.end() && level > iter->first; ++iter) {}

		if (iter != effects.begin())
			iter--;

		iter->second.apply(target);
	}

	void IronBodyUseEffect::apply(Char& target) const
	{
		target.show_iron_body();
	}
}