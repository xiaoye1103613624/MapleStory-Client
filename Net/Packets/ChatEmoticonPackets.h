//////////////////////////////////////////////////////////////////////////////////
// ChatEmoticonPackets — Kaentake 动态聊天表情出站包
// 对接 s9 RecvOpcode.CHAT_EMOTICON(0x3715) → ChatEmoticonHandler：
//   body = int emoticonId（4B），服务端白名单校验 + 1.5s 冷却后同图广播。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class ChatEmoticonPacket : public OutPacket
	{
	public:
		explicit ChatEmoticonPacket(int32_t emoticon_id)
			: OutPacket(OutPacket::Opcode::CHAT_EMOTICON)
		{
			write_int(emoticon_id);
		}
	};
}
