//////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton					//
//																			//
//	Fyms085 resource diagnostics additions Copyright (C) 2026				//
//																			//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.										//
//																			//
//	This program is distributed in the hope that it will be useful,			//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.						//
//																			//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

namespace ms
{
	// Resource layer diagnostics.
	//
	// Rationale: ms::client_init() starts with Session::get().init(), which needs a
	// real handshake on the login port. When no server is available the process
	// exits before DataFiles::init() ever runs, so nothing about the data layer can
	// be observed or automated.
	//
	// These entry points deliberately bypass Session / Window / Audio and only
	// exercise the data source, which makes resource layout problems reproducible
	// without a server.
	namespace ResourceProbe
	{
		// Default report filename, written next to the executable.
		constexpr const char* LOG_FILE = "_resource_probe.log";

		// Loads the configured data source and writes a report to @log_path.
		// Returns 0 on success, otherwise the number of failures (1).
		int run_probe(bool verbose, const std::string& log_path = LOG_FILE);

		// Runs the headless assertions for the resource layer.
		// Returns the number of failed checks (0 == green).
		int run_selftest(const std::string& log_path = LOG_FILE);
	}
}
