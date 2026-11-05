/* CUIItem::Toggle(void) @ 0x7cbba0 */

void __thiscall CUIItem::Toggle(CUIItem *this)
{
  BOOL v2; // eax
  int v3; // eax

  v2 = this->m_bExtended == 0;
  this->m_bExtended = v2;
  CConfig::SetInventoryExpanded((CConfig *)TSingleton<CConfig>::ms_pInstance._m_pStr, v2);
  CWnd::Destroy(this);
  v3 = this->m_bExtended != 0 ? 594 : 172;
  this->m_nBtCloseX = this->m_bExtended != 0 ? 574 : 150;
  CUIWnd::CreateUIWndPosSaved(this, v3, 293, 10);
}

