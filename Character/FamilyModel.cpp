////////////////////////////////////////////////////////////////////////////////
//	FamilyModel 实现
////////////////////////////////////////////////////////////////////////////////
#include "FamilyModel.h"

namespace ms
{
	FamilyModel& FamilyModel::get()
	{
		static FamilyModel model;
		return model;
	}

	void FamilyModel::set_info(const FamilyInfoData& value)
	{
		data = value;
	}

	void FamilyModel::set_privileges(const std::vector<FamilyPrivilegeDef>& value)
	{
		defs = value;
	}

	void FamilyModel::add_rep(int32_t gain)
	{
		data.cur_rep += gain;
		data.today_rep += gain;
		if (data.cur_rep < 0) data.cur_rep = 0;
		if (data.today_rep < 0) data.today_rep = 0;
	}
}
