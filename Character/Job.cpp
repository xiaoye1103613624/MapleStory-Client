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
#include "../Util/Locale.h"
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

		if (id == 0 || id == 1000 || id == 2000)
			// 三个家族的初心者：冒险家 0、骑士团 1000、战神 2000。
			// 证据：BeiDou-Server_s9 client/Character.java:5920 isBeginnerJob()
			// 原来只看 id % 100 == 0，把 1000/2000 当成一转，家族共享技能因此判定不可用。
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
					// 家族初心者命名空间 = 职业 id 去掉千位以下：冒险家 0、骑士团 1000、
					// 战神 2000。can_use() 用它放行 000.img / 1000.img / 2000.img 的家族技能。
					return (id / 1000) * 1000;
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

	std::string Job::get_name() const
	{
		return Locale::text(name);
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
	// 骑士团（1000..1512）与战神（2000..2112）在这张表里没有对应项（参考端只到冒险家），
	// 名称取对端服务端的中文 i18n：BeiDou-Server_s9 gms-server/src/main/resources/
	// i18n/message_zh_CN.properties 的 job.name.<id> 项。
	std::string Job::get_name(uint16_t jid) const
	{
		switch (jid)
		{
			case 0:
				return "job.0";
			case 100:
				return "job.100";
			case 110:
				return "job.110";
			case 111:
				return "job.111";
			case 112:
				return "job.112";
			case 120:
				return "job.120";
			case 121:
				return "job.121";
			case 122:
				return "job.122";
			case 130:
				return "job.130";
			case 131:
				return "job.131";
			case 132:
				return "job.132";
			case 200:
				return "job.200";
			case 210:
				return "job.210";
			case 211:
				return "job.211";
			case 212:
				return "job.212";
			case 220:
				return "job.220";
			case 221:
				return "job.221";
			case 222:
				return "job.222";
			case 230:
				return "job.230";
			case 231:
				return "job.231";
			case 232:
				return "job.232";
			case 300:
				return "job.300";
			case 310:
				return "job.310";
			case 311:
				return "job.311";
			case 312:
				return "job.312";
			case 320:
				return "job.320";
			case 321:
				return "job.321";
			case 322:
				return "job.322";
			case 400:
				return "job.400";
			case 410:
				return "job.410";
			case 411:
				return "job.411";
			case 412:
				return "job.412";
			case 420:
				return "job.420";
			case 421:
				return "job.421";
			case 422:
				return "job.422";
			case 500:
				return "job.500";
			case 510:
				return "job.510";
			case 511:
				return "job.511";
			case 512:
				return "job.512";
			case 520:
				return "job.520";
			case 521:
				return "job.521";
			case 522:
				return "job.522";
			case 1000:
				return "job.1000";
			case 1100:
				return "job.1100";
			case 1110:
				return "job.1110";
			case 1111:
				return "job.1111";
			case 1112:
				return "job.1112";
			case 1200:
				return "job.1200";
			case 1210:
				return "job.1210";
			case 1211:
				return "job.1211";
			case 1212:
				return "job.1212";
			case 1300:
				return "job.1300";
			case 1310:
				return "job.1310";
			case 1311:
				return "job.1311";
			case 1312:
				return "job.1312";
			case 1400:
				return "job.1400";
			case 1410:
				return "job.1410";
			case 1411:
				return "job.1411";
			case 1412:
				return "job.1412";
			case 1500:
				return "job.1500";
			case 1510:
				return "job.1510";
			case 1511:
				return "job.1511";
			case 1512:
				return "job.1512";
			case 2000:
				return "job.2000";
			case 2001:
				return "job.2001";
			case 2100:
				return "job.2100";
			case 2110:
				return "job.2110";
			case 2111:
				return "job.2111";
			case 2112:
				return "job.2112";
			case 800:
			case 900:
			case 910:
				return "job.910";
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