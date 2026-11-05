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
#include "SocketWinsock.h"

#if defined(_WIN32) && !defined(USE_ASIO)
#include <ws2tcpip.h>
// SIO_TCP_INFO 与 TCP_INFO_v0 的真身在 mstcpip.h（ws2tcpip 不自动带它）。
#include <mstcpip.h>
#include "../Util/PerfLog.h"

// TODO: Can this be moved?
#pragma comment (lib, "Ws2_32.lib")

namespace ms
{
	bool SocketWinsock::open(const char* iaddr, const char* port)
	{
		WSADATA wsa_info;
		sock = INVALID_SOCKET;

		struct addrinfo* addr_info = NULL;
		struct addrinfo* ptr = NULL;
		struct addrinfo hints;

		int result = WSAStartup(MAKEWORD(2, 2), &wsa_info);

		if (result != 0)
			return false;

		ZeroMemory(&hints, sizeof(hints));

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		result = getaddrinfo(iaddr, port, &hints, &addr_info);

		if (result != 0)
		{
			WSACleanup();

			return false;
		}

		for (ptr = addr_info; ptr != NULL; ptr = ptr->ai_next)
		{
			sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (sock == INVALID_SOCKET)
			{
				WSACleanup();

				return false;
			}

			result = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);

			if (result == SOCKET_ERROR)
			{
				closesocket(sock);

				sock = INVALID_SOCKET;

				continue;
			}

			break;
		}

		freeaddrinfo(addr_info);

		if (sock == INVALID_SOCKET)
		{
			WSACleanup();

			return false;
		}

		result = recv(sock, (char*)buffer, 32, 0);

		if (result == HANDSHAKE_LEN)
		{
			return true;
		}
		else
		{
			WSACleanup();

			return false;
		}
	}

	bool SocketWinsock::close()
	{
		int error = closesocket(sock);

		WSACleanup();

		return error != SOCKET_ERROR;
	}

	bool SocketWinsock::dispatch(const int8_t* bytes, size_t length) const
	{
		return send(sock, (char*)bytes, static_cast<int>(length), 0) != SOCKET_ERROR;
	}

	size_t SocketWinsock::receive(bool* success)
	{
		timeval timeout = { 0, 0 };
		fd_set sockset = { 0 };

		FD_SET(sock, &sockset);

		int result = select(0, &sockset, 0, 0, &timeout);

		if (result > 0)
			result = recv(sock, (char*)buffer, MAX_PACKET_LENGTH, 0);

		if (result == SOCKET_ERROR)
		{
			*success = false;

			return 0;
		}
		else
		{
			return result;
		}
	}

	const int8_t* SocketWinsock::get_buffer() const
	{
		return buffer;
	}

	bool SocketWinsock::rtt_us(uint64_t& out) const
	{
		// Windows 内核平滑 RTT：SIO_TCP_INFO（mstcpip.h TCP_INFO_v0.RttUs，微秒）。
		// 【实测根因，2026-11-04，见 tools/_rtt_probe.c】：回环链路（127.0.0.1，
		// 单机部署常态）上内核直接回 WSAEINVAL(10022) —— tcpip.sys 不持有 loopback
		// 连接，无从查起；对照组 getsockopt(TCP_MAXSEG) 同 socket 成功(mss=65495)
		// 证明 socket/IOCTL 机制本身正常，两种 in-buffer 写法(NULL / &IPPROTO_TCP)
		// + v0/v1 结构都失败。结论：本机自连时延迟必然显示 "--"（且回环延迟本身
		// 无网络意义）；连真实 LAN / 远端服时本调用有效。要单机也出数需应用层
		// 回声 RTT（另立项，见 docs 预留）。
		// 每条失败路径限 3 次打 perf 通道（带错误码），定位不靠猜。
		static int diag_budget = 3;
		auto diag = [](const std::string& why)
		{
			if (diag_budget-- > 0)
				perf::mark("RTT", "winsock " + why);
		};

		if (sock == INVALID_SOCKET)
		{
			diag("invalid socket");
			return false;
		}

		TCP_INFO_v0 ti;
		DWORD bytes = 0;

		if (WSAIoctl(sock, SIO_TCP_INFO, nullptr, 0, &ti, sizeof(ti),
			&bytes, nullptr, nullptr) != 0)
		{
			diag("WSAIoctl fail wsa=" + std::to_string(WSAGetLastError()));
			return false;
		}

		if (bytes < sizeof(DWORD) /* State 至少可读 */)
		{
			diag("short reply bytes=" + std::to_string(bytes));
			return false;
		}

		if (ti.State != TCPSTATE_ESTABLISHED)
		{
			diag("state=" + std::to_string((int)ti.State) + " bytes=" + std::to_string(bytes));
			return false;
		}

		// 回环/局域网平滑 RTT 会取整到 0 —— 这是"真快"不是"没数据"，
		// 按 0ms 显示；仅当查询失败或非 ESTABLISHED 才回 "--"。
		out = ti.RttUs ? ti.RttUs : ti.MinRttUs;
		return true;
	}
}
#endif