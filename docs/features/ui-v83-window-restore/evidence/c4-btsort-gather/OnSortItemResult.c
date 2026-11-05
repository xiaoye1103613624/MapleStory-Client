/* CWvsContext::OnSortItemResult(CInPacket &) @ 0x9f12b0 */

void __thiscall CWvsContext::OnSortItemResult(CWvsContext *this, CInPacket *iPacket)
{
  unsigned __int8 v2; // al

  CInPacket::Decode1(iPacket);
  v2 = CInPacket::Decode1(iPacket);
  if ( TSingleton<CUIItem>::ms_pInstance != nullptr )
    CUIItem::SetArrangeButton(TSingleton<CUIItem>::ms_pInstance, v2, 0);
}

