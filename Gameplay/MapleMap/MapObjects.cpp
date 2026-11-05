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
#include "MapObjects.h"

#include "../../Constants.h"

namespace ms
{
	namespace
	{
		// 视口剔除余量（像素）。对象锚点（脚/中心）落在屏幕外这么远就不画。
		// 余量要盖住最高的怪物名牌+血条、掉落标签与常见贴图越界，512 足以
		// 宽容全部 v83 素材。收益在大场景（几百怪 / 满地掉落）：屏幕外对象
		// 不再逐帧走动画取帧与 quad 提交——这是渲染侧最大的白烧。
		constexpr int32_t CULL_MARGIN = 512;
	}

	void MapObjects::draw(Layer::Id layer, double viewx, double viewy, float alpha) const
	{
		const int32_t vw = Constants::Constants::get().get_viewwidth();
		const int32_t vh = Constants::Constants::get().get_viewheight();

		for (auto& oid : layers[layer])
		{
			auto mmo = get(oid);

			if (!mmo || !mmo->is_active())
				continue;

			// 屏幕坐标 = 世界坐标 + 相机偏移（viewx/viewy 为负向）。剔除只查
			// 锚点，插值抖动由余量吸收；不取包围盒，免得把剔除做成新的开销。
			const Point<int16_t> pos = mmo->get_position();
			const int32_t sx = pos.x() + static_cast<int32_t>(viewx);
			const int32_t sy = pos.y() + static_cast<int32_t>(viewy);

			if (sx < -CULL_MARGIN || sx > vw + CULL_MARGIN
				|| sy < -CULL_MARGIN || sy > vh + CULL_MARGIN)
				continue;

			mmo->draw(viewx, viewy, alpha);
		}
	}

	void MapObjects::update(const Physics& physics)
	{
		for (auto iter = objects.begin(); iter != objects.end();)
		{
			bool remove_mob = false;

			if (auto& mmo = iter->second)
			{
				int8_t oldlayer = mmo->get_layer();
				int8_t newlayer = mmo->update(physics);

				if (newlayer == -1)
				{
					remove_mob = true;

					// 死亡/失效时也要从旧图层摘掉 oid：只 erase objects 的话 layers[oldlayer]
					// 里会残留，服务端回收同一个 oid 重新 spawn 后，新对象会被旧图层再画一遍
					// （draw 只按 oid 查对象，不校验它现在到底在哪一层）。
					layers[oldlayer].erase(iter->first);
				}
				else if (newlayer != oldlayer)
				{
					int32_t oid = iter->first;
					layers[oldlayer].erase(oid);
					layers[newlayer].insert(oid);
				}
			}
			else
			{
				remove_mob = true;
			}

			if (remove_mob)
				iter = objects.erase(iter);
			else
				iter++;
		}
	}

	void MapObjects::clear()
	{
		objects.clear();

		for (auto& layer : layers)
			layer.clear();
	}

	bool MapObjects::contains(int32_t oid) const
	{
		return objects.count(oid) > 0;
	}

	void MapObjects::add(std::unique_ptr<MapObject> toadd)
	{
		int32_t oid = toadd->get_oid();
		int8_t layer = toadd->get_layer();
		objects[oid] = std::move(toadd);
		layers[layer].insert(oid);
	}

	void MapObjects::remove(int32_t oid)
	{
		auto iter = objects.find(oid);

		if (iter != objects.end() && iter->second)
		{
			int8_t layer = iter->second->get_layer();
			objects.erase(iter);

			layers[layer].erase(oid);
		}
	}

	Optional<MapObject> MapObjects::get(int32_t oid)
	{
		auto iter = objects.find(oid);

		return iter != objects.end() ? iter->second.get() : nullptr;
	}

	Optional<const MapObject> MapObjects::get(int32_t oid) const
	{
		auto iter = objects.find(oid);

		return iter != objects.end() ? iter->second.get() : nullptr;
	}

	MapObjects::underlying_t::iterator MapObjects::begin()
	{
		return objects.begin();
	}

	MapObjects::underlying_t::iterator MapObjects::end()
	{
		return objects.end();
	}

	MapObjects::underlying_t::const_iterator MapObjects::begin() const
	{
		return objects.begin();
	}

	MapObjects::underlying_t::const_iterator MapObjects::end() const
	{
		return objects.end();
	}

	MapObjects::underlying_t::size_type MapObjects::size() const
	{
		return objects.size();
	}
}