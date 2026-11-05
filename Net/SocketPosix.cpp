////////////////////////////////////////////////////////////////////////////////
//	POSIX (BSD) socket backend for non-Windows platforms.
////////////////////////////////////////////////////////////////////////////////
#include "SocketPosix.h"

#if defined(USE_POSIX_SOCKET)
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>

namespace ms
{
	SocketPosix::SocketPosix() : sock(-1)
	{
	}

	SocketPosix::~SocketPosix()
	{
		if (sock >= 0)
			::close(sock);
	}

	bool SocketPosix::open(const char* iaddr, const char* port)
	{
		struct addrinfo hints;
		std::memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		struct addrinfo* addr_info = nullptr;
		int gai = getaddrinfo(iaddr, port, &hints, &addr_info);
		if (gai != 0)
		{
			return false;
		}

		bool connected = false;
		for (struct addrinfo* ptr = addr_info; ptr != nullptr; ptr = ptr->ai_next)
		{
			sock = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (sock < 0)
			{
				continue;
			}

			if (::connect(sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen)) == 0)
			{
				connected = true;
				break;
			}

			::close(sock);
			sock = -1;
		}

		freeaddrinfo(addr_info);

		if (!connected)
			return false;

		// Read the server handshake (the MapleStory login handshake is sent
		// immediately after connect).
		ssize_t result = ::recv(sock, reinterpret_cast<char*>(buffer), 32, 0);

		return result == static_cast<ssize_t>(HANDSHAKE_LEN);
	}

	bool SocketPosix::close()
	{
		if (sock < 0)
			return true;

		int error = ::close(sock);
		sock = -1;

		return error == 0;
	}

	bool SocketPosix::dispatch(const int8_t* bytes, size_t length) const
	{
		if (sock < 0)
			return false;

		return ::send(sock, reinterpret_cast<const char*>(bytes),
					  static_cast<int>(length), 0) == static_cast<int>(length);
	}

	size_t SocketPosix::receive(bool* success)
	{
		*success = true;

		if (sock < 0)
		{
			*success = false;
			return 0;
		}

		// Peek without blocking to find out if there is data; the original
		// Winsock implementation used select() with a zero timeout.
		fd_set sockset;
		FD_ZERO(&sockset);
		FD_SET(sock, &sockset);

		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		int result = ::select(sock + 1, &sockset, nullptr, nullptr, &timeout);

		if (result > 0)
		{
			result = static_cast<int>(::recv(sock, reinterpret_cast<char*>(buffer),
											MAX_PACKET_LENGTH, 0));
		}

		if (result < 0)
		{
			*success = false;
			return 0;
		}

		return static_cast<size_t>(result);
	}

	const int8_t* SocketPosix::get_buffer() const
	{
		return buffer;
	}

	bool SocketPosix::rtt_us(uint64_t& out) const
	{
		// Linux/Android：TCP_INFO 的 tcpi_rtt 单位为微秒（内核 4.x 起与 Windows/BSD 对齐）。
		// 未连接或内核不支持时 getsockopt 失败 → false，标题显示 "--"。
		if (sock < 0)
			return false;

		struct tcp_info ti;
		socklen_t len = sizeof(ti);

		if (getsockopt(sock, IPPROTO_TCP, TCP_INFO, &ti, &len) != 0)
			return false;

		out = ti.tcpi_rtt;
		return out > 0;
	}
}
#endif
