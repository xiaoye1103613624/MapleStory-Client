/* CUIItem::OnSort(void) @ 0x7cbb70 */

void __thiscall CUIItem::OnSort(CUIItem *this)
{
  if ( ((int)TSingleton<CUserLocal>::ms_pInstance.m_Data[133].m_str & 0xFFFFFFFE) != 0x12
    && TSingleton<CUniqueModeless>::ms_pInstance == nullptr )
  {
    CWvsContext::SendSortItemRequest((CWvsContext *)TSingleton<CWvsContext>::ms_pInstance._m_pStr, this->m_nItemTI);
  }
}

