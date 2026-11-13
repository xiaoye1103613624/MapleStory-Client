////////////////////////////////////////////////////////////////////////////////
//	BuddyListModel 实现
////////////////////////////////////////////////////////////////////////////////
#include "BuddyListModel.h"

namespace ms
{
	BuddyListModel& BuddyListModel::get()
	{
		static BuddyListModel model;
		return model;
	}

	void BuddyListModel::set_capacity(int32_t cap)
	{
		if (cap > 0)
			list.capacity = cap;
	}

	void BuddyListModel::update_channel(int32_t cid, int32_t channel)
	{
		for (auto& b : list.buddies)
		{
			if (b.cid == cid)
			{
				b.channel = channel;
				b.online = (channel != -2 && channel != -1);
				return;
			}
		}
	}

	const Buddy* BuddyListModel::find(int32_t cid) const
	{
		for (const auto& b : list.buddies)
			if (b.cid == cid)
				return &b;
		return nullptr;
	}
}
