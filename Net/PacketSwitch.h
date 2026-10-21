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
#pragma once

#include "PacketHandler.h"

#include <memory>
#include <type_traits>
#include <unordered_map>

namespace ms
{
	// Class which contains the array of handler classes to use
	class PacketSwitch
	{
	public:
		// Register all handlers
		PacketSwitch();

		// Forward a packet to the correct handler
		void forward(const int8_t* bytes, size_t length) const;

	private:
		// Print a warning
		void warn(const std::string& message, size_t opcode) const;

		// Get the string value of the Opcode
		std::string OpcodeName(size_t opcode) const;

		// Opcodes for which handlers can be registered
		enum Opcode : uint16_t;

		// Message when an unhandled packet is received
		static constexpr const char* MSG_UNHANDLED = "Unhandled packet detected";
		// Message when a packet with a larger opcode than the array size is received
		static constexpr const char* MSG_OUTOFBOUNDS = "Large opcode detected";
		// Message when a packet with a larger opcode than the array size is received
		static constexpr const char* MSG_REREGISTER = "Handler was registered twice";
		// Maximum number of handlers needed
		static constexpr const size_t NUM_HANDLERS = 500;

		std::unique_ptr<PacketHandler> handlers[NUM_HANDLERS];
		// 北斗的自定义 opcode 落在 0x37xx，远大于常规 opcode 表（500），
		// 用一个稀疏表单独存放，避免为几个自定义包撑大上面的数组。
		std::unordered_map<uint16_t, std::unique_ptr<PacketHandler>> extended_handlers;

		// Register a handler for the specified opcode.
		// Handler 落在哪个容器由 opcode 决定（编译期分支，用标签分发避免常量条件告警）。
		template <size_t O, typename T, typename...Args>
		void emplace(Args&& ...args)
		{
			static_assert(std::is_base_of<PacketHandler, T>::value, "Error: Packet handlers must derive from PacketHandler");

			emplace_impl<O, T>(
				std::integral_constant<bool, (O < NUM_HANDLERS)>(),
				std::forward<Args>(args)...
				);
		}

		// 常规 opcode：放进定长数组
		template <size_t O, typename T, typename...Args>
		void emplace_impl(std::true_type, Args&& ...args)
		{
			if (handlers[O])
				warn(MSG_REREGISTER, O);

			handlers[O] = std::make_unique<T>(
				std::forward<Args>(args)...
				);
		}

		// 北斗自定义 opcode（>= NUM_HANDLERS）：放进稀疏表
		template <size_t O, typename T, typename...Args>
		void emplace_impl(std::false_type, Args&& ...args)
		{
			auto opcode = static_cast<uint16_t>(O);

			if (extended_handlers.count(opcode))
				warn(MSG_REREGISTER, O);

			extended_handlers[opcode] = std::make_unique<T>(
				std::forward<Args>(args)...
				);
		}
	};
}