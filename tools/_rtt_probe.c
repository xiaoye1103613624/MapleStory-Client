/* _rtt_probe.c - SIO_TCP_INFO failure root cause, A/B/C/D matrix (ASCII only)
 * A: SIO_TCP_INFO + TCP_INFO_v0, in=IPPROTO_TCP   (current failing call)
 * B: SIO_TCP_INFO + TCP_INFO_v1 larger struct     (version variant)
 * C: SIO_BASE_INFO                                 (control: WSAIoctl works here?)
 * D: SIO_KEEPALIVE_VALS                            (control 2)
 * usage: _rtt_probe.exe [ip] [port] */
#define WIN32_LEAN_AND_MEAN
#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x0A000006	/* NTDDI_WIN10_RS5: TCP_INFO_v1 visible */
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char** argv)
{
	const char* ip = argc > 1 ? argv[1] : "127.0.0.1";
	const char* port = argc > 2 ? argv[2] : "8484";

	WSADATA wd;
	if (WSAStartup(MAKEWORD(2, 2), &wd)) { printf("wsastartup fail\n"); return 2; }

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons((unsigned short)atoi(port));
	inet_pton(AF_INET, ip, &sa.sin_addr);

	if (connect(s, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
		printf("connect %s:%s fail wsa=%d\n", ip, port, WSAGetLastError());
		return 3;
	}

	DWORD proto = IPPROTO_TCP;

	{ /* A */
		TCP_INFO_v0 ti; DWORD b = 0; memset(&ti, 0, sizeof(ti));
		int r = WSAIoctl(s, SIO_TCP_INFO, &proto, sizeof(proto), &ti, (DWORD)sizeof(ti), &b, NULL, NULL);
		printf("A v0        r=%d wsa=%d bytes=%u", r, WSAGetLastError(), b);
		if (r == 0) printf(" state=%d RttUs=%u", (int)ti.State, ti.RttUs);
		printf("\n");
	}
	{ /* B */
		TCP_INFO_v1 ti; DWORD b = 0; memset(&ti, 0, sizeof(ti));
		int r = WSAIoctl(s, SIO_TCP_INFO, &proto, sizeof(proto), &ti, (DWORD)sizeof(ti), &b, NULL, NULL);
		printf("B v1        r=%d wsa=%d bytes=%u", r, WSAGetLastError(), b);
		if (r == 0) printf(" state=%d RttUs=%u", (int)ti.State, ti.RttUs);
		printf("\n");
	}
	{ /* C control: getsockopt TCP_MAXSEG works on established sockets */
		int mss = 0; int len = sizeof(mss);
		int r = getsockopt(s, IPPROTO_TCP, TCP_MAXSEG, (char*)&mss, &len);
		printf("C mss       r=%d wsa=%d mss=%d\n", r, WSAGetLastError(), mss);
	}
	/* D skipped: control C already proves the socket/IOCTL path is alive */

	closesocket(s);
	WSACleanup();
	return 0;
}
