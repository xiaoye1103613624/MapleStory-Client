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
#include "Job.h"

namespace ms
{
	Job::Job(uint16_t i)
	{
		change_job(i);
	}

	Job::Job()
	{
		change_job(0);
	}

	void Job::change_job(uint16_t i)
	{
		id = i;
		name = get_name(id);

		if (id == 0)
			level = Level::BEGINNER;
		else if (id % 100 == 0)
			level = Level::FIRST;
		else if (id % 10 == 0)
			level = Level::SECOND;
		else if (id % 10 == 1)
			level = Level::THIRD;
		else
			level = Level::FOURTH;
	}

	bool Job::is_sub_job(uint16_t subid) const
	{
		for (int32_t lvit = BEGINNER; lvit <= FOURTH; lvit++)
		{
			Level lv = static_cast<Level>(lvit);

			if (subid == get_subjob(lv))
				return true;
		}

		return false;
	}

	bool Job::can_use(int32_t skill_id) const
	{
		uint16_t required = static_cast<uint16_t>(skill_id / 10000);
		return is_sub_job(required);
	}

	uint16_t Job::get_id() const
	{
		return id;
	}

	uint16_t Job::get_subjob(Level lv) const
	{
		if (lv <= level)
		{
			switch (lv)
			{
				case Level::BEGINNER:
					return 0;
				case Level::FIRST:
					return (id / 100) * 100;
				case Level::SECOND:
					return (id / 10) * 10;
				case Level::THIRD:
					return (level == Level::FOURTH) ? id - 1 : id;
				case Level::FOURTH:
					return id;
			}
		}

		return 0;
	}

	const std::string& Job::get_name() const
	{
		return name;
	}

	Job::Level Job::get_level() const
	{
		return level;
	}

	// v83 参考端（BeiDou V17.7）职业名为中文，硬编码于 ijl15.dll 的定长字符串表
	// （文件偏移 0x2c868 起，每项 8 字节，GBK 编码；String.nx 无 Job.img 节点）。
	// 各名称来源（偏移 → 文本，解码后按 UTF-8 写入本文件）：
	// 0x2c868 新手 / 0x2c870 战士 / 0x2c878 弓箭手 / 0x2c880 飞侠 / 0x2c888 海盗 /
	// 0x2c890 管理员 / 0x2c8a8 剑客 / 0x2c8b0 勇士 / 0x2c8b8 英雄 / 0x2c8c0 准骑士 /
	// 0x2c8c8 骑士 / 0x2c8d0 圣骑士 / 0x2c8d8 枪战士 / 0x2c8e0 龙骑士 / 0x2c8e8 黑骑士 /
	// 0x2c8f0 法师 / 0x2c908 法师(火,毒) / 0x2c914 巫师(火,毒) / 0x2c920 魔导师(火,毒) /
	// 0x2c930 法师(冰,雷) / 0x2c93c 巫师(冰,雷) / 0x2c948 魔导师(冰,雷) /
	// 0x2c958 牧师 / 0x2c960 祭司 / 0x2c968 主教 / 0x2c970 猎人 / 0x2c978 射手 /
	// 0x2c980 神射手 / 0x2c988 弩弓手 / 0x2c990 游侠 / 0x2c998 箭神 / 0x2c9a0 刺客 /
	// 0x2c9a8 无影人 / 0x2c9b0 隐士 / 0x2c9b8 侠客 / 0x2c9c0 独行客 / 0x2c9c8 侠盗 /
	// 0x2c9d0 拳手 / 0x2c9d8 斗士 / 0x2c9e0 冲锋队长 / 0x2c9ec 火枪手 / 0x2c9f4 大副 /
	// 0x2c9fc 船长 / 0x2ca04 初心者。
	std::string Job::get_name(uint16_t jid) const
	{
		switch (jid)
		{
			case 0:
				return "新手";
			case 100:
				return "战士";
			case 110:
				return "剑客";
			case 111:
				return "勇士";
			case 112:
				return "英雄";
			case 120:
				return "准骑士";
			case 121:
				return "骑士";
			case 122:
				return "圣骑士";
			case 130:
				return "枪战士";
			case 131:
				return "龙骑士";
			case 132:
				return "黑骑士";
			case 200:
				return "法师";
			case 210:
				return "法师(火,毒)";
			case 211:
				return "巫师(火,毒)";
			case 212:
				return "魔导师(火,毒)";
			case 220:
				return "法师(冰,雷)";
			case 221:
				return "巫师(冰,雷)";
			case 222:
				return "魔导师(冰,雷)";
			case 230:
				return "牧师";
			case 231:
				return "祭司";
			case 232:
				return "主教";
			case 300:
				return "弓箭手";
			case 310:
				return "猎人";
			case 311:
				return "射手";
			case 312:
				return "神射手";
			case 320:
				return "弩弓手";
			case 321:
				return "游侠";
			case 322:
				return "箭神";
			case 400:
				return "飞侠";
			case 410:
				return "刺客";
			case 411:
				return "无影人";
			case 412:
				return "隐士";
			case 420:
				return "侠客";
			case 421:
				return "独行客";
			case 422:
				return "侠盗";
			case 500:
				return "海盗";
			case 510:
				return "拳手";
			case 511:
				return "斗士";
			case 512:
				return "冲锋队长";
			case 520:
				return "火枪手";
			case 521:
				return "大副";
			case 522:
				return "船长";
			case 1000:
				return "初心者";
			case 1100:
			case 1110:
			case 1111:
			case 1112:
			case 1200:
			case 1210:
			case 1211:
			case 1212:
			case 1300:
			case 1310:
			case 1311:
			case 1312:
			case 1400:
			case 1410:
			case 1411:
			case 1412:
			case 1500:
			case 1510:
			case 1511:
			case 1512:
				return "骑士团";
			case 900:
			case 910:
				return "管理员";
			default:
				return "";
		}
	}

	EquipStat::Id Job::get_primary(Weapon::Type weapontype) const
	{
		switch (id / 100)
		{
			case 2:
				return EquipStat::Id::INT;
			case 3:
				return EquipStat::Id::DEX;
			case 4:
				return EquipStat::Id::LUK;
			case 5:
				return (weapontype == Weapon::Type::GUN) ? EquipStat::Id::DEX : EquipStat::Id::STR;
			default:
				return EquipStat::Id::STR;
		}
	}

	EquipStat::Id Job::get_secondary(Weapon::Type weapontype) const
	{
		switch (id / 100)
		{
			case 2:
				return EquipStat::Id::LUK;
			case 3:
				return EquipStat::Id::STR;
			case 4:
				return EquipStat::Id::DEX;
			case 5:
				return (weapontype == Weapon::Type::GUN) ? EquipStat::Id::STR : EquipStat::Id::DEX;
			default:
				return EquipStat::Id::DEX;
		}
	}
}