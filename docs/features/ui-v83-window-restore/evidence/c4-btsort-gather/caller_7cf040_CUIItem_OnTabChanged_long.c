/* CUIItem::OnTabChanged(long) @ 0x7cf040 */

void __thiscall CUIItem::OnTabChanged(CUIItem *this, int nTab)
{
  CharacterData *p; // ebp
  char *p_m_uTableSize; // edi
  int *p_m_uCount; // ebx
  CCtrlScrollBar *v6; // ecx
  ZRef<GW_ItemSlotBase> *a; // eax
  ZRef<GW_ItemSlotBase> *v8; // eax
  ZRef<CharacterData> result; // [esp+8h] [ebp-8h] BYREF

  this->m_nOption = nTab;
  CUIItem::SetItemTI(this, nTab + 1);
  p = CWvsContext::GetCharacterData((CWvsContext *)TSingleton<CWvsContext>::ms_pInstance._m_pStr, &result)->p;
  if ( result.p != nullptr )
  {
    p_m_uTableSize = (char *)&result.p[-1].m_mVisitorQuestLog._m_uTableSize;
    p_m_uCount = (int *)&result.p[-1].m_mVisitorQuestLog._m_uCount;
    if ( InterlockedDecrement(p_m_uCount) == 0 )
    {
      InterlockedIncrement(p_m_uCount);
      if ( p_m_uTableSize != nullptr )
        (**(void (__thiscall ***)(char *, int))p_m_uTableSize)(p_m_uTableSize, 1);
    }
    result.p = nullptr;
  }
  v6 = this->m_pSBItem.p;
  if ( v6 != nullptr )
  {
    a = p->aaItemSlot[this->m_nItemTI].a;
    if ( a != nullptr )
      a = (ZRef<GW_ItemSlotBase> *)a[-1].p;
    CCtrlScrollBar::SetScrollRange(v6, (int)&a[-3].gap0[2] / 4 + 1);
    CCtrlScrollBar::SetCurPos(this->m_pSBItem.p, CUIItem::ms_anItemScrollPos[this->m_nItemTI]);
    this->m_nFirstPosition = 4 * this->m_pSBItem.p->m_nCurPos + 1;
  }
  else
  {
    this->m_nFirstPosition = 1;
  }
  if ( this->m_bExtended != 0 )
  {
    switch ( this->m_nItemTI )
    {
      case 1:
      case 2:
      case 4:
        this->m_pBtCashShop.p->vfptr->SetEnable(&this->m_pBtCashShop.p->IUIMsgHandler, 1);
        break;
      case 3:
      case 5:
        this->m_pBtCashShop.p->vfptr->SetEnable(&this->m_pBtCashShop.p->IUIMsgHandler, 0);
        break;
      default:
        break;
    }
    if ( this->m_bExtended != 0 )
    {
      v8 = p->aaItemSlot[this->m_nItemTI].a;
      if ( v8 != nullptr )
        v8 = (ZRef<GW_ItemSlotBase> *)v8[-1].p;
      if ( (int)&v8[-1].p + 3 >= 96 )
        this->m_pBtCashShop.p->vfptr->SetEnable(&this->m_pBtCashShop.p->IUIMsgHandler, 0);
    }
  }
  CUIItem::SetTryToReleaseItem(this, 0, -1);
  CUIToolTip::ClearToolTip(&this->m_uiToolTip);
  CUIItem::SetArrangeButton(this, this->m_nItemTI, this->m_anSortBtState[this->m_nItemTI]);
  CWnd::InvalidateRect(this, nullptr);
  CUIItem::SetLastestGetItemEffect(this);
}

