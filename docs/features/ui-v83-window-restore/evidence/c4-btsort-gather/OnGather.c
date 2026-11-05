/* CUIItem::OnGather(void) @ 0x7cbb40 */

void __thiscall CUIItem::OnGather(CUIItem *this)
{
  if ( ((int)TSingleton<CUserLocal>::ms_pInstance.m_Data[133].m_str & 0xFFFFFFFE) != 0x12
    && TSingleton<CUniqueModeless>::ms_pInstance == nullptr )
  {
    CWvsContext::SendGatherItemRequest((CWvsContext *)TSingleton<CWvsContext>::ms_pInstance._m_pStr, this->m_nItemTI);
  }
}

