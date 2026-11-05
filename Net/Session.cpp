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
#include "Session.h"

#include "../Configuration.h"

#include <cstdio>
#include <fstream>
#include <string>

namespace ms
{
	namespace
	{
		// 对齐参考端 PacketDump 格式（BeiDou.exe 0x6EC7C0：push "PacketDump : "
		// @0xAFCC14 + 循环 "%02x "@0xAFCC0C，小写十六进制）。
		// 由 debug.packet_dump 控制，默认关闭；追加写入 exe 同目录 packet_dump.log，
		// 供联调时与服务端日志逐字节核对。Release 下 LOG() 为 no-op，故直接写文件。
		// outbound 标记：收发两侧写进同一个文件，光靠行序无法区分方向，而 T9 的
		// 配对管线必须按方向配对，故在保留 "PacketDump : " 原格式的前提下前置 [SEND]/[RECV]。
		void packet_dump(const int8_t* bytes, size_t length, bool outbound)
		{
			if (!Setting<PacketDumpLog>::get().load())
				return;

			std::string line = outbound ? "[SEND] PacketDump : " : "[RECV] PacketDump : ";
			line.reserve(line.size() + length * 3 + 1);

			char hex[8];
			for (size_t i = 0; i < length; ++i)
			{
				std::snprintf(hex, sizeof(hex), "%02x ", static_cast<uint8_t>(bytes[i]));
				line += hex;
			}
			line += '\n';

			std::ofstream file("packet_dump.log", std::ios::app);

			if (file.is_open())
				file.write(line.data(), line.size());
		}
	}

	Session::Session()
	{
		connected = false;
		length = 0;
		pos = 0;
	}

	Session::~Session()
	{
		if (connected)
			socket.close();
	}

	bool Session::init(const char* host, const char* port)
	{
		// Connect to the server
		connected = socket.open(host, port);

		if (connected)
		{
			// Read keys necessary for communicating with the server
			cryptography = { socket.get_buffer() };
		}

		return connected;
	}

	Error Session::init()
	{
		std::string HOST = Setting<ServerIP>::get().load();
		std::string PORT = Setting<ServerPort>::get().load();

		if (!init(HOST.c_str(), PORT.c_str()))
			return Error::CONNECTION;

		return Error::NONE;
	}

	void Session::reconnect(const char* address, const char* port)
	{
		// Close the current connection and open a new one
		bool success = socket.close();

		if (success)
			init(address, port);
		else
			connected = false;
	}

	void Session::process(const int8_t* bytes, size_t available)
	{
		if (pos == 0)
		{
			// Position is zero, meaning this is the start of a new packet. Start by determining length.
			length = cryptography.check_length(bytes);

			// 长度守卫（F6 的处置）：s9 收到畸形头部时不断线、不记日志，之后这条连接再也收不到
			// 任何入站包（notes/probe.md §5）。我们若拿着畸形长度往下走，pos 永远追不上 length，
			// memcpy 会一路写穿 buffer[MAX_PACKET_LENGTH]。这里把"静默聋 + 越界"变成一次干净的主动断线：
			// socket.close() 后 connected=false，上层沿既有掉线路径处理。
			// 下界（真正可达）：USE_CRYPTO 恒开（MapleStory.h:42）且 D8 修完后 check_length 只可能给 0..65535，
			// 其中 0/1 会让下面的 pos>=length 立刻成立，拿 0/1 字节去 forward 读 2 字节 opcode 直接越界。
			// 上界（防御性，当前不可达）：65535 < MAX_PACKET_LENGTH=131072，永不命中；留着是给
			// Cryptography.cpp:90-96 那条 4 字节直拼的 #else 分支兜底——注意该分支引用了未定义的
			// HEADERLEN，现在根本编不过，详见 notes/transport-vectors.md §4。
			// 注：Release 下 LOG() 是 no-op（MapleStory.h:77），所以可观测性靠断线本身 + Debug 构建的日志。
			if (length < OPCODE_LENGTH || length > MAX_PACKET_LENGTH)
			{
				LOG(LOG_NETWORK, "invalid packet length " << length << ", dropping connection");
				socket.close();
				connected = false;
				return;
			}

			// Reading the length means we processed the header. Move forward by the header length.
			bytes = bytes + HEADER_LENGTH;
			available -= HEADER_LENGTH;
		}

		// Determine how much we can write. Write data into the buffer.
		size_t towrite = length - pos;

		if (towrite > available)
			towrite = available;

		memcpy(buffer + pos, bytes, towrite);
		pos += towrite;

		// Check if the current packet has been fully processed
		if (pos >= length)
		{
			cryptography.decrypt(buffer, length);

			// 调试：记录解密后的完整入站封包（debug.packet_dump 开启时）。
			packet_dump(buffer, length, false);

			try
			{
				packetswitch.forward(buffer, length);
			}
			catch (const PacketError& err)
			{
				LOG(LOG_NETWORK, err.what());
			}

			pos = 0;
			length = 0;

			// Check if there is more available
			size_t remaining = available - towrite;

			if (remaining >= MIN_PACKET_LENGTH)
			{
				// More packets are available, so we start over.
				process(bytes + towrite, remaining);
			}
		}
	}

	void Session::write(int8_t* packet_bytes, size_t packet_length)
	{
		if (!connected)
			return;

		int8_t header[HEADER_LENGTH];
		cryptography.create_header(header, packet_length);

		// 调试：记录加密前的完整出站封包（debug.packet_dump 开启时）。
		packet_dump(packet_bytes, packet_length, true);

		cryptography.encrypt(packet_bytes, packet_length);

		socket.dispatch(header, HEADER_LENGTH);
		socket.dispatch(packet_bytes, packet_length);
	}

	void Session::read()
	{
		// Check if a packet has arrived. Handle if data is sufficient: 4 bytes (header) + 2 bytes (opcode) = 6 bytes.
		size_t result = socket.receive(&connected);

		if (result >= MIN_PACKET_LENGTH || length > 0)
		{
			// Retrieve buffer from the socket and process it
			const int8_t* bytes = socket.get_buffer();
			process(bytes, result);
		}
	}

	void Session::reconnect()
	{
		std::string HOST = Setting<ServerIP>::get().load();
		std::string PORT = Setting<ServerPort>::get().load();

		reconnect(HOST.c_str(), PORT.c_str());
	}

	bool Session::is_connected() const
	{
		return connected;
	}

	bool Session::tcp_rtt_us(uint64_t& out) const
	{
		if (!connected)
			return false;

		return socket.rtt_us(out);
	}
}