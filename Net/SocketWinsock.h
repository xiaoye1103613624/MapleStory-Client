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

// SIO_TCP_INFO / TCP_INFO_v0（rtt_us 用）在 SDK 头里门控于
// NTDDI_VERSION >= NTDDI_WIN10_RS2（0x0A000003）。注意只定义 _WIN32_WINNT
// 不够：sdkddkver 的换算 0x0A00 -> 0x0A000000 达不到 RS2 粒度，直接钉 NTDDI。
#if defined(_WIN32) && !defined(USE_ASIO) && !defined(NTDDI_VERSION)
#define WINVER 0x0A00
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#define NTDDI_VERSION 0x0A000003	// NTDDI_WIN10_RS2
#endif

#include "../MapleStory.h"

#ifndef USE_ASIO
#include "NetConstants.h"

namespace ms
{
#ifdef USE_CRYPTO
	const size_t HANDSHAKE_LEN = 16;
#else
	const size_t HANDSHAKE_LEN = 2;
#endif

	class SocketWinsock
	{
	public:
		bool open(const char* address, const char* port);
		bool close();

		bool dispatch(const int8_t* bytes, size_t length) const;
		size_t receive(bool* connected);
		const int8_t* get_buffer() const;

		// 内核层 TCP 平滑往返时延（微秒，tcpi_rtt）。零发包成本、不依赖服务端
		// 应用层协议；未连接/系统不支持时返回 false。
		bool rtt_us(uint64_t& out) const;

	private:
		uint64_t sock;
		int8_t buffer[MAX_PACKET_LENGTH];
	};
}
#endif