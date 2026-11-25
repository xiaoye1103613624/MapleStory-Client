////////////////////////////////////////////////////////////////////////////////
//  神秘之力 / 奥术之力 S→C handler 实现
//
//  【字段对齐铁律】服务端写什么顺序，客户端就按什么顺序读。FORCE_INFO 固定
//  4×int = 16B，读之前先 recv.available() 校验整条长度，不足直接 return，
//  避免半截包导致 read_int 越界抛 PacketError；多余字节忽略（协议预留）。
////////////////////////////////////////////////////////////////////////////////
#include "ForceHandlers.h"

#include "../../IO/UITypes/UIForceState.h"
#include "../../Util/Misc.h"

namespace ms
{
    void ForceInfoHandler::handle(InPacket& recv) const
    {
        // FORCE_INFO 固定 4 个 int = 16B，边界校验。
        if (recv.available() < 16)
        {
            LOG(LOG_WARN, "[FORCE] info truncated: available=" << recv.available() << " < 16");
            return;
        }

        ForceInfo info;
        info.arcaneForce = recv.read_int();
        info.authenticForce = recv.read_int();
        info.arcaneLevel = recv.read_int();
        info.authenticLevel = recv.read_int();

        UIForceState::get().set_info(info);
        LOG(LOG_DEBUG, "[DBG-FORCE] arcane=" << info.arcaneForce
            << " authentic=" << info.authenticForce
            << " arcaneLv=" << info.arcaneLevel
            << " authenticLv=" << info.authenticLevel);
    }
}
