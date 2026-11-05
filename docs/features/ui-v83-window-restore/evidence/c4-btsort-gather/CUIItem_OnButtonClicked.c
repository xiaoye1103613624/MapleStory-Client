/* CUIItem::OnButtonClicked(uint) @ 0x7cc4f0 */

void __thiscall CUIItem::OnButtonClicked(CUIItem *this, unsigned int nId)
{
  int v2; // edx

  switch ( nId )
  {
    case 0x7D2u:
      CUIItem::OnDropMoney(this);
      break;
    case 0x7D3u:
      CUIItem::OnGather(this);
      break;
    case 0x7D4u:
      CUIItem::OnSort(this);
      break;
    case 0x7D5u:
    case 0x7D6u:
      CUIItem::Toggle(this);
      break;
    case 0x7D7u:
      v2 = 0;
      switch ( this->m_nItemTI )
      {
        case 1:
          v2 = 50200093;
          break;
        case 2:
          v2 = 50200094;
          break;
        case 4:
          v2 = 50200095;
          break;
        default:
          break;
      }
      *((_DWORD *)TSingleton<CWvsContext>::ms_pInstance._m_pStr + 4147) = v2;
      CWvsContext::SendMigrateToShopRequest((CWvsContext *)TSingleton<CWvsContext>::ms_pInstance._m_pStr, nullptr);
      break;
    default:
      CUIWnd::OnButtonClicked(this, nId);
      break;
  }
}

