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

#include "../Configuration.h"

#if defined(_WIN32)
#include <Windows.h>
#include <IPHlpApi.h>
#elif defined(__APPLE__) || defined(__linux__) || defined(__ANDROID__) || defined(__FreeBSD__)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#if defined(__APPLE__) || defined(__linux__) || defined(__ANDROID__)
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#endif
#else
// Unknown platform: provide a no-op HWID so the build still compiles.
#endif

namespace ms
{
	class HardwareInfo
	{
	public:
		HardwareInfo()
		{
#if defined(_WIN32)
			compute_windows();
#elif defined(__APPLE__) || defined(__linux__) || defined(__ANDROID__) || defined(__FreeBSD__)
			compute_posix();
#else
			// No HWID source on this platform; leave the configuration defaults.
			(void)0;
#endif
		}

	private:
#if defined(_WIN32)
		void compute_windows()
		{
			size_t size = 18;

			// Hard Drive VolumeSerialNumber
			char* volumeSerialNumber = (char*)malloc(size);

			TCHAR szVolume[MAX_PATH + 1];
			TCHAR szFileSystem[MAX_PATH + 1];

			DWORD dwSerialNumber, dwMaxLen, dwSystemFlags;

			TCHAR szDrives[MAX_PATH + 1];
			DWORD dwLen = GetLogicalDriveStrings(MAX_PATH, szDrives);
			TCHAR* pLetter = szDrives;

			BOOL bSuccess;

			bSuccess = GetVolumeInformation(pLetter, szVolume, MAX_PATH, &dwSerialNumber, &dwMaxLen, &dwSystemFlags, szFileSystem, MAX_PATH);

			if (bSuccess)
			{
				sprintf_s(volumeSerialNumber, size, "%X%X", HIWORD(dwSerialNumber), LOWORD(dwSerialNumber));
			}
			else
			{
				printf("Cannot retrieve Volume information for %s\n", pLetter);
				free(volumeSerialNumber);
				return;
			}

			// HWID/MACS
			PIP_ADAPTER_INFO AdapterInfo;
			DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
			char* hwid = (char*)malloc(size);
			char* macs = (char*)malloc(size);

			AdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));

			if (AdapterInfo == NULL)
			{
				printf("Error allocating memory needed to call GetAdaptersinfo\n");
				free(volumeSerialNumber);
				free(hwid);
				free(macs);
				return;
			}

			// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
			if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
			{
				free(AdapterInfo);
				AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);

				if (AdapterInfo == NULL)
				{
					printf("Error allocating memory needed to call GetAdaptersinfo\n");
					free(volumeSerialNumber);
					free(hwid);
					free(macs);
					return;
				}
			}

			if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
			{
				// Contains pointer to current adapter info
				PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

				// Technically should look at pAdapterInfo->AddressLength and not assume it is 6
				sprintf_s(hwid, size, "%02X%02X%02X%02X%02X%02X",
					pAdapterInfo->Address[0], pAdapterInfo->Address[1],
					pAdapterInfo->Address[2], pAdapterInfo->Address[3],
					pAdapterInfo->Address[4], pAdapterInfo->Address[5]);

				Configuration::get().set_hwid(hwid, volumeSerialNumber);

				pAdapterInfo = pAdapterInfo->Next;

				if (pAdapterInfo)
				{
					// Technically should look at pAdapterInfo->AddressLength and not assume it is 6
					sprintf_s(macs, size, "%02X-%02X-%02X-%02X-%02X-%02X",
						pAdapterInfo->Address[0], pAdapterInfo->Address[1],
						pAdapterInfo->Address[2], pAdapterInfo->Address[3],
						pAdapterInfo->Address[4], pAdapterInfo->Address[5]);

					Configuration::get().set_macs(macs);
				}
			}

			free(AdapterInfo);
			free(volumeSerialNumber);
			free(hwid);
			free(macs);
		}
#elif defined(__APPLE__) || defined(__linux__) || defined(__ANDROID__) || defined(__FreeBSD__)
		void compute_posix()
		{
			// Volume / machine id: prefer /etc/machine-id (Linux), fall back to
			// a hostname-based string. This replaces the Windows volume serial.
			std::string volumeSerial = "00000000";
			{
				std::ifstream f("/etc/machine-id");
				if (f)
				{
					std::string line;
					if (std::getline(f, line) && !line.empty())
						volumeSerial = line.substr(0, 8);
				}
				else
				{
					const char* host = getenv("HOSTNAME");
					if (host && *host)
						volumeSerial = std::string(host).substr(0, 8);
				}
			}

			// First non-loopback MAC address via a socket ioctl.
			char hwid[18] = "000000000000";
			char macs[18] = "00-00-00-00-00-00";

			int fd = socket(AF_INET, SOCK_DGRAM, 0);
			if (fd >= 0)
			{
				struct ifreq ifr;
				struct ifconf ifc;
				char buf[4096];
				std::memset(buf, 0, sizeof(buf));
				ifc.ifc_len = sizeof(buf);
				ifc.ifc_buf = buf;
				if (ioctl(fd, SIOCGIFCONF, &ifc) >= 0)
				{
					int n = ifc.ifc_len / sizeof(struct ifreq);
					for (int i = 0; i < n; ++i)
					{
						struct ifreq* cur = &ifc.ifc_req[i];
						std::memset(&ifr, 0, sizeof(ifr));
						std::strncpy(ifr.ifr_name, cur->ifr_name, IFNAMSIZ - 1);
						if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
							continue;
						unsigned char* mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;
						if (mac[0] == 0 && mac[1] == 0 && mac[2] == 0 &&
							mac[3] == 0 && mac[4] == 0 && mac[5] == 0)
							continue; // skip zero address
						snprintf(hwid, sizeof(hwid), "%02X%02X%02X%02X%02X%02X",
							mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
						snprintf(macs, sizeof(macs), "%02X-%02X-%02X-%02X-%02X-%02X",
							mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
						break;
					}
				}
				close(fd);
			}

			char volbuf[32];
			snprintf(volbuf, sizeof(volbuf), "%s", volumeSerial.c_str());
			Configuration::get().set_hwid(hwid, volbuf);
			Configuration::get().set_macs(macs);
		}
#endif
	};
}