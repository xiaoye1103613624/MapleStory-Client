//////////////////////////////////////////////////////////////////////////////
//	Patch updater facade — sync final img/wz/nx files from HTTP manifest
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Error.h"

#include <string>

namespace ms
{
	namespace patch
	{
		class PatchUpdater
		{
		public:
			// Run once at boot before ResourceService::init().
			// On soft failure with patch.allow_skip=true returns NONE and logs WARN.
			static Error sync();
		};
	}
}
