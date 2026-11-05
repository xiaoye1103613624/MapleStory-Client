/* CUIItem::SetArrangeButton(long,long) @ 0x7ccc90 */

void __thiscall CUIItem::SetArrangeButton(CUIItem *this, int nTI, int nState)
{
  ZRef<CCtrlOriginButton> *v4; // eax
  ZRef<CCtrlOriginButton> *p_result; // ecx
  ZRef<CCtrlOriginButton> *v6; // eax
  ZRef<CCtrlOriginButton> result; // [esp+Ch] [ebp-2Ch] BYREF
  ZRef<CCtrlOriginButton> v8; // [esp+14h] [ebp-24h] BYREF
  int paramButton; // [esp+1Ch] [ebp-1Ch]
  int v10; // [esp+20h] [ebp-18h]
  int v11; // [esp+24h] [ebp-14h]
  int v12; // [esp+28h] [ebp-10h]
  int v13; // [esp+34h] [ebp-4h]

  this->m_anSortBtState[nTI] = nState;
  if ( nTI == this->m_nItemTI )
  {
    v12 = 0;
    v10 = 0;
    v11 = 0;
    v13 = 0;
    paramButton = 0;
    if ( nState == 0 )
    {
      v6 = CLayoutMan::AddButton(&this->m_lm, &v8, aUi_435, 0x7D3u, 0, 0, nullptr);
      LOBYTE(v13) = 1;
      ZRef<CCtrlOriginButton>::operator=(&this->m_pBtArrange, v6);
      LOBYTE(v13) = 0;
      if ( v8.p == nullptr )
        return;
      p_result = &v8;
      goto LABEL_8;
    }
    if ( nState == 1 )
    {
      v4 = CLayoutMan::AddButton(&this->m_lm, &result, aUi_159, 0x7D4u, 0, 0, nullptr);
      LOBYTE(v13) = 2;
      ZRef<CCtrlOriginButton>::operator=(&this->m_pBtArrange, v4);
      LOBYTE(v13) = 0;
      if ( result.p != nullptr )
      {
        p_result = &result;
LABEL_8:
        ZRef<CCtrlOriginButton>::_ReleaseRaw(p_result, nullptr);
      }
    }
  }
}

