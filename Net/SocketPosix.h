////////////////////////////////////////////////////////////////////////////////
//	POSIX (BSD) socket backend for non-Windows platforms.
//	Mirrors the interface of SocketWinsock / SocketAsio so Session can pick
//	whichever backend the build selects.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../MapleStory.h"

#if defined(USE_POSIX_SOCKET)
#include "NetConstants.h"

#include <cstdint>

namespace ms
{
#ifdef USE_CRYPTO
	const size_t HANDSHAKE_LEN = 16;
#else
	const size_t HANDSHAKE_LEN = 2;
#endif

	// Class that wraps a BSD socket.
	class SocketPosix
	{
	public:
		SocketPosix();
		~SocketPosix();

		bool open(const char* address, const char* port);
		bool close();

		bool dispatch(const int8_t* bytes, size_t length) const;
		size_t receive(bool* connected);
		const int8_t* get_buffer() const;

		// 内核层 TCP 平滑往返时延（微秒，tcpi_rtt）。零发包成本；未连接或
		// 平台不支持时返回 false。
		bool rtt_us(uint64_t& out) const;

	private:
		int sock;
		int8_t buffer[MAX_PACKET_LENGTH];
	};
}
#endif
