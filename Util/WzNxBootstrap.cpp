//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client                    //
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton                      //
//                                                                              //
//	Fyms085 WZ cache bootstrap additions Copyright (C) 2026                    //
//                                                                              //
//	This program is free software: you can redistribute it and/or modify        //
//	it under the terms of the GNU Affero General Public License as published by //
//	the Free Software Foundation, either version 3 of the License, or            //
//	(at your option) any later version.                                         //
//////////////////////////////////////////////////////////////////////////////////
#include "WzNxBootstrap.h"

#include "../MapleStory.h"

#if defined(USE_NX) && defined(AUTO_CONVERT_WZ)
#include <Windows.h>

#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

namespace ms
{
	namespace WzNxBootstrap
	{
		namespace
		{
			constexpr std::size_t NUM_FILES = 16;

			constexpr std::array<const char*, NUM_FILES> wz_filenames =
			{
				"Base.wz", "Character.wz", "Effect.wz", "Etc.wz",
				"Item.wz", "Map.wz", "Mob.wz", "Morph.wz",
				"Npc.wz", "Quest.wz", "Reactor.wz", "Skill.wz",
				"Sound.wz", "String.wz", "TamingMob.wz", "UI.wz"
			};

			constexpr std::array<const char*, NUM_FILES> nx_filenames =
			{
				"Base.nx", "Character.nx", "Effect.nx", "Etc.nx",
				"Item.nx", "Map.nx", "Mob.nx", "Morph.nx",
				"Npc.nx", "Quest.nx", "Reactor.nx", "Skill.nx",
				"Sound.nx", "String.nx", "TamingMob.nx", "UI.nx"
			};

			constexpr std::array<const char*, 3> converter_paths =
			{
				"NoLifeWzToNx.exe",
				"tools\\NoLifeWzToNx.exe",
				"wz-tools\\NoLifeWzToNx.exe"
			};

			bool exists(const char* path)
			{
				return std::ifstream{ path, std::ios::binary }.good();
			}

			template<std::size_t N>
			bool all_exist(const std::array<const char*, N>& paths)
			{
				for (const char* path : paths)
					if (!exists(path))
						return false;

				return true;
			}

			const char* find_converter()
			{
				for (const char* path : converter_paths)
					if (exists(path))
						return path;

				return nullptr;
			}

			bool run_converter(const char* executable)
			{
				std::string command = std::string{ "\"" } + executable + "\" . -c";
				STARTUPINFOA startup = {};
				PROCESS_INFORMATION process = {};
				startup.cb = sizeof(startup);

				if (!CreateProcessA(
					nullptr,
					&command[0],
					nullptr,
					nullptr,
					FALSE,
					0,
					nullptr,
					nullptr,
					&startup,
					&process
				))
					return false;

				WaitForSingleObject(process.hProcess, INFINITE);
				DWORD exit_code = 1;
				GetExitCodeProcess(process.hProcess, &exit_code);
				CloseHandle(process.hThread);
				CloseHandle(process.hProcess);

				return exit_code == 0;
			}
		}

		Error prepare()
		{
			if (all_exist(nx_filenames))
				return Error::Code::NONE;

			for (const char* wz : wz_filenames)
				if (!exists(wz))
					return Error(Error::Code::MISSING_FILE, wz);

			const char* converter = find_converter();
			if (!converter)
				return Error(
					Error::Code::WZ_CONVERTER_MISSING,
					"Install NoLifeWzToNx.exe beside MapleStory.exe or under wz-tools, then enter retry."
				);

			std::cout << "WZ files detected. Creating the first-run NX cache..." << std::endl;
			if (!run_converter(converter) || !all_exist(nx_filenames))
				return Error(
					Error::Code::WZ_CONVERSION_FAILED,
					"Check converter output and free disk space, then enter retry."
				);

			std::cout << "WZ cache complete. Starting Fyms085 client." << std::endl;
			return Error::Code::NONE;
		}
	}
}
#else
namespace ms
{
	namespace WzNxBootstrap
	{
		Error prepare()
		{
			return Error::Code::NONE;
		}
	}
}
#endif
