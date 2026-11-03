// summon_probe 专用桩：替代完整 Configuration/Text 图形链。
// 只为 nl::wz::load_all 提供 KeyResolver 所需的 keymode/keyfile 默认值。
// 单机只读探针，不影响主工程。
#include "../../Configuration.h"

#include "../../Util/Resource/Keyring.h"

namespace ms
{
	// Singleton<Configuration>::instance 的探针侧定义（主工程在 Configuration.cpp）。
	template <>
	Configuration Singleton<Configuration>::instance;

	Configuration::Configuration() {}
	Configuration::~Configuration() {}
	void Configuration::load() {}

	ms::res::KeyMode Configuration::get_keymode() const
	{
		return ms::res::KeyMode::Auto;
	}

	std::string Configuration::get_keyfile() const
	{
		return {};
	}
}
