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

// If defined use Asio for networking, otherwise use Winsock.
//#define USE_ASIO

// ---- Cross-platform backend selection (overridable from the build system) ----
// BASS audio backend. Windows ships the BASS libs in includes/bass24, so it is
// enabled by default there. On other platforms define USE_BASS=1 only if you
// provide the BASS libs; otherwise a silent audio fallback is compiled in.
#if !defined(USE_BASS)
#ifdef _WIN32
#define USE_BASS
#endif
#endif

// Socket backend for non-Windows platforms. Windows keeps Winsock; the Asio
// path exists but requires the Asio headers (not bundled). On any other
// platform a POSIX (BSD) socket implementation is selected automatically.
#if !defined(_WIN32) && !defined(USE_ASIO) && !defined(USE_POSIX_SOCKET)
#define USE_POSIX_SOCKET
#endif
// -----------------------------------------------------------------------------

// Use cryptography for communication with the server
#define USE_CRYPTO

// If defined use NX, otherwise use WZ.
#define USE_NX

// Debug options
#define LOG_ERROR	1
#define LOG_WARN	2
#define LOG_INFO	3
#define LOG_DEBUG	4
#define LOG_NETWORK	5
#define LOG_UI		6
#define LOG_TRACE	7

// Log Level
#ifdef _DEBUG
	#define LOG_LEVEL LOG_DEBUG
#else
	#define LOG_LEVEL LOG_WARN
#endif

// Log Text
#define LOG_TEXT(level) (\
	level == LOG_ERROR		? "ERROR"	:\
	level == LOG_WARN		? "WARN"	:\
	level == LOG_INFO		? "INFO"	:\
	level == LOG_DEBUG		? "DEBUG"	:\
	level == LOG_NETWORK	? "NETWORK"	:\
	level == LOG_UI			? "UI"		:\
	level == LOG_TRACE		? "TRACE"	: "UNDEFINED")

// Log Commands
#ifdef _DEBUG
	#define LOG(level, message) level <= LOG_LEVEL ? std::cout << "[" << LOG_TEXT(level) << "]: " << message << std::endl : std::cout
#else
	#define LOG(level, message) void(0)
#endif