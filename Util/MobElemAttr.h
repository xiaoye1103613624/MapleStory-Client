#pragma once

#include <cstdint>
#include <string>

namespace ms
{
	namespace data
	{
		// 192 MOB 元素属性表（I=冰 L=雷 F=火 S=毒 H=圣 D=暗 P=物理；数字 0=普通 1=免疫 2=抵抗 3=弱）。
		// 由 192 解包 XML 的 elemAttr 提取，共 2797 条。返回 nullptr 表示该怪无数据。
		const char* mob_elem_attr(int32_t mobid);

		// 把 elemAttr code 解码为可读串，如 "冰:弱 火:抵抗"。返回空串表示无数据。
		std::string elem_attr_display(const std::string& code);
	}
}
