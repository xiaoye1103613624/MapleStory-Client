/* CWvsContext::SendGatherItemRequest(long) @ 0x9d5b70 */

void __thiscall CWvsContext::SendGatherItemRequest(CWvsContext *this, int nType)
{
  unsigned int update_time; // eax
  COutPacket oPacket; // [esp+Ch] [ebp-1Ch] BYREF
  int v5; // [esp+24h] [ebp-4h]

  if ( (unsigned int)(nType - 1) <= 4
    && this->m_bExclRequestSent == 0
    && _ZtlSecureFuse<long>(
         this->m_pCharacterData.p->characterStat._ZtlSecureTear_nHP,
         this->m_pCharacterData.p->characterStat._ZtlSecureTear_nHP_CS) > 0
    && get_update_time() - this->m_tExclRequestSent >= 500 )
  {
    COutPacket::COutPacket(&oPacket, 75);
    v5 = 0;
    update_time = get_update_time();
    COutPacket::Encode4(&oPacket, update_time);
    COutPacket::Encode1(&oPacket, nType);
    CClientSocket::SendPacket(TSingleton<CClientSocket>::ms_pInstance, (int)&oPacket, &oPacket);
    this->m_bExclRequestSent = 1;
    this->m_tExclRequestSent = get_update_time();
    v5 = -1;
    ZArray<unsigned char>::RemoveAll(&oPacket.m_aSendBuff);
  }
}

