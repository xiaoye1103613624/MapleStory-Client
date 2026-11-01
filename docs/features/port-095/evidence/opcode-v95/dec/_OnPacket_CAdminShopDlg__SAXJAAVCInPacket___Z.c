void __cdecl CAdminShopDlg::OnPacket(ZXString<char> *nType, CInPacket *iPacket)
{
  CInPacket *v2; // esi
  unsigned int v3; // ebx
  unsigned __int16 v4; // ax
  int v5; // edi
  CUtilDlgEx *v6; // eax
  char *v7; // ecx
  int v8; // eax
  int v9; // eax
  CUtilDlgEx *v10; // esi
  StringPool *v11; // eax
  CAdminShopDlg *v12; // ecx
  CAdminShopDlg *v13; // eax
  CAdminShopDlg *v14; // eax
  CAdminShopDlg *v15; // esi
  CInPacket *v16; // edi
  CInPacket *v17; // ecx
  unsigned __int8 v18; // al
  char *v19; // ecx
  int v20; // edi
  int m_nSnapshotTI; // eax
  int m_nCurPos; // ebx
  int v23; // eax
  int v24; // ebp
  unsigned int m_dwNpcTemplateID; // ebp
  void (__thiscall *Update)(IGObj *); // edx
  int v27; // ebx
  StringPool *Instance; // eax
  ZXString<char> *String; // eax
  char *m_pStr; // eax
  CUtilDlgEx *v31; // eax
  char *v32; // ecx
  int v33; // eax
  int v34; // eax
  CUtilDlgEx *v35; // esi
  int v36; // edx
  StringPool *v37; // eax
  ZXString<char> *v38; // eax
  StringPool *v39; // eax
  ZXString<char> *v40; // eax
  StringPool *v41; // eax
  ZXString<char> *v42; // eax
  StringPool *v43; // eax
  ZXString<char> *v44; // eax
  StringPool *v45; // eax
  ZXString<char> *v46; // eax
  StringPool *v47; // eax
  ZXString<char> *v48; // eax
  StringPool *v49; // eax
  ZXString<char> *v50; // eax
  StringPool *v51; // eax
  ZXString<char> v52; // [esp-14h] [ebp-88h] BYREF
  const wchar_t *v53; // [esp-10h] [ebp-84h]
  ZXString<char> v54; // [esp-Ch] [ebp-80h] BYREF
  int v55; // [esp-8h] [ebp-7Ch]
  int v56; // [esp-4h] [ebp-78h]
  int bReOpenDlg; // [esp+14h] [ebp-60h]
  CMSException v58; // [esp+18h] [ebp-5Ch] BYREF
  HRESULT pExceptionObject; // [esp+1Ch] [ebp-58h] BYREF
  ZXString<char> result; // [esp+20h] [ebp-54h] BYREF
  ZXString<char> v61; // [esp+24h] [ebp-50h] BYREF
  ZXString<char> v62; // [esp+28h] [ebp-4Ch] BYREF
  ZXString<char> v63; // [esp+2Ch] [ebp-48h] BYREF
  ZXString<char> v64; // [esp+30h] [ebp-44h] BYREF
  ZXString<char> v65; // [esp+34h] [ebp-40h] BYREF
  ZXString<char> v66; // [esp+38h] [ebp-3Ch] BYREF
  ZXString<char> v67; // [esp+3Ch] [ebp-38h] BYREF
  ZRef<CUtilDlgEx> pQDlg; // [esp+40h] [ebp-34h] BYREF
  COutPacket v69; // [esp+48h] [ebp-2Ch] BYREF
  COutPacket oPacket; // [esp+58h] [ebp-1Ch] BYREF
  int v71; // [esp+70h] [ebp-4h]

  if ( nType == (ZXString<char> *)366 ) /*0x431120*/
  {
    if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr /*0x4312f9*/
      && TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf(
           this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
           a2: &CAdminShopDlg::ms_RTTI_CAdminShopDlg) != 0 )
    {
      v15 = (CAdminShopDlg *)TSingleton<CUniqueModeless>::ms_pInstance; /*0x4312ff*/
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x431307*/
      {
        if ( TSingleton<CUniqueModeless>::ms_pInstance[19].m_lpChildren.CDialog::CWnd::vfptr == nullptr ) /*0x431314*/
        {
          CMSException::CMSException(this: &v58, hr: 553648131); /*0x43131f*/
          pExceptionObject = v58.m_hr; /*0x431332*/
          _CxxThrowException(&pExceptionObject, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x431336*/
        }
        v16 = iPacket; /*0x43133b*/
        v17 = iPacket; /*0x43133f*/
        TSingleton<CUniqueModeless>::ms_pInstance[19].m_lpChildren.CDialog::CWnd::vfptr = nullptr; /*0x431341*/
        if ( CInPacket::Decode1(this: v17) == 4 ) /*0x431352*/
        {
          v18 = CInPacket::Decode1(this: v16); /*0x43135a*/
          v20 = v18; /*0x43135f*/
          if ( v18 == 0 ) /*0x431364*/
          {
            m_nSnapshotTI = v15->m_nSnapshotTI; /*0x431366*/
            if ( m_nSnapshotTI > 0 && m_nSnapshotTI <= 5 ) /*0x431373*/
            {
              m_nCurPos = v15->m_pSBUser.p->m_nCurPos; /*0x43137b*/
              v23 = CAdminShopDlg::CmpSellItem(this: v15); /*0x431380*/
              v24 = v23; /*0x431385*/
              if ( v23 < m_nCurPos ) /*0x431389*/
                CCtrlScrollBar::SetCurPos(this: v15->m_pSBUser.p, nPos: v23); /*0x431392*/
              if ( v24 > m_nCurPos + 4 ) /*0x43139c*/
                CCtrlScrollBar::SetCurPos(this: v15->m_pSBUser.p, nPos: v24 - 4); /*0x4313a8*/
              v15->m_nUserSelected = v24; /*0x4313ad*/
            }
          }
          m_dwNpcTemplateID = v15->m_dwNpcTemplateID; /*0x4313b3*/
          if ( v20 == 1 || v20 == 2 || v20 == 3 || v20 == 6 || v20 == 7 || v20 == 8 || v20 == 11 ) /*0x4313da*/
          {
            Update = v15->CUniqueModeless::CDialog::CWnd::IGObj::vfptr[13].Update; /*0x4313e8*/
            bReOpenDlg = 1; /*0x4313ef*/
            ((void (__thiscall *)(CAdminShopDlg *, int))Update)(a1: v15, a2: 1); /*0x4313f7*/
          }
          else
          {
            bReOpenDlg = 0; /*0x4313dc*/
          }
          v27 = 0; /*0x4313f9*/
          nType = nullptr; /*0x4313fb*/
          v71 = 4; /*0x431402*/
          switch ( v20 ) /*0x431413*/
          {
            case 1: /*0x431413*/
            case 2: /*0x431413*/
            case 3: /*0x431413*/
              Instance = StringPool::GetInstance(); /*0x431424*/
              String = StringPool::GetString( /*0x43142b*/
                         this: Instance,
                         &result,
                         nIdx: SP_4665_THATS_NOT_SOMETHING_I_BARTER_ILL_SHOW_YOU_THE_LIST_AGAIN_FOR_YOU_TO_CHOOSE_FROM);
              LOBYTE(v71) = 5; /*0x431435*/
              ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: String); /*0x43143a*/
              m_pStr = result._m_pStr; /*0x43143f*/
              LOBYTE(v71) = 4; /*0x431443*/
              goto LABEL_46; /*0x431443*/
            case 4: /*0x431413*/
              v37 = StringPool::GetInstance(); /*0x43158b*/
              v38 = StringPool::GetString(this: v37, result: &v61, nIdx: SP_4666_ARE_YOU_BEGGING); /*0x431592*/
              LOBYTE(v71) = 6; /*0x43159c*/
              ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v38); /*0x4315a1*/
              m_pStr = v61._m_pStr; /*0x4315a6*/
              LOBYTE(v71) = 4; /*0x4315aa*/
              goto LABEL_46; /*0x4315af*/
            case 5: /*0x431413*/
              v39 = StringPool::GetInstance(); /*0x4315be*/
              v40 = StringPool::GetString( /*0x4315c5*/
                      this: v39,
                      result: &v62,
                      nIdx: SP_4667_WHY_DO_YOU_CARRY_AROUND_SO_MUCH_MESOS_COME_BACK_AFTER_EMPTYING_YOUR_POCKETS);
              LOBYTE(v71) = 7; /*0x4315cf*/
              ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v40); /*0x4315d4*/
              m_pStr = v62._m_pStr; /*0x4315d9*/
              LOBYTE(v71) = 4; /*0x4315dd*/
              goto LABEL_46; /*0x4315e2*/
            case 6: /*0x431413*/
              v41 = StringPool::GetInstance(); /*0x4315f1*/
              v42 = StringPool::GetString( /*0x4315f8*/
                      this: v41,
                      result: &v63,
                      nIdx: SP_4668_TAKING_YOUR_PRECIOUS_TIME_HUH_I_DONT_BARTER_THIS_ITEM_ANYMORE);
              LOBYTE(v71) = 8; /*0x431602*/
              ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v42); /*0x431607*/
              m_pStr = v63._m_pStr; /*0x43160c*/
              LOBYTE(v71) = 4; /*0x431610*/
              goto LABEL_46; /*0x431615*/
            case 7: /*0x431413*/
              v43 = StringPool::GetInstance(); /*0x431624*/
              v44 = StringPool::GetString( /*0x43162b*/
                      this: v43,
                      result: &v64,
                      nIdx: SP_4669_IM_DONE_BARTERING_THIS_ITEM_FOR_THE_DAY_COME_BACK_TOMORROW);
              LOBYTE(v71) = 9; /*0x431635*/
              ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v44); /*0x43163a*/
              m_pStr = v64._m_pStr; /*0x43163f*/
              LOBYTE(v71) = 4; /*0x431643*/
              goto LABEL_46; /*0x431648*/
            case 8: /*0x431413*/
              v45 = StringPool::GetInstance(); /*0x431657*/
              v46 = StringPool::GetString( /*0x43165e*/
                      this: v45,
                      result: &v65,
                      nIdx: SP_4670_WHY_ARE_YOU_OBSESSING_OVER_THIS_ITEM_I_WONT_BARTER_ANYMORE);
              LOBYTE(v71) = 10; /*0x431668*/
              ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v46); /*0x43166d*/
              m_pStr = v65._m_pStr; /*0x431672*/
              LOBYTE(v71) = 4; /*0x431676*/
              goto LABEL_46; /*0x43167b*/
            case 9: /*0x431413*/
              v47 = StringPool::GetInstance(); /*0x43168a*/
              v48 = StringPool::GetString( /*0x431691*/
                      this: v47,
                      result: &v66,
                      nIdx: SP_4671_YOURE_GREEDY_HOW_ABOUT_YOU_LOWER_THE_QUANTITY);
              LOBYTE(v71) = 11; /*0x43169b*/
              ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v48); /*0x4316a0*/
              m_pStr = v66._m_pStr; /*0x4316a5*/
              LOBYTE(v71) = 4; /*0x4316a9*/
              goto LABEL_46; /*0x4316ae*/
            case 10: /*0x431413*/
              v56 = 0; /*0x4316e6*/
              v55 = 0; /*0x4316e8*/
              v54._m_pStr = nullptr; /*0x4316ea*/
              v53 = nullptr; /*0x4316ec*/
              v52._m_pStr = v19; /*0x4316ee*/
              *(_DWORD *)pQDlg.gap0 = &v52; /*0x4316f1*/
              v51 = StringPool::GetInstance(); /*0x4316fb*/
              StringPool::GetString( /*0x431702*/
                this: v51,
                result: &v52,
                nIdx: SP_5869_ITEMS_OR_MESOS_CANNOT_BE_MOVED_R_NPLEASE_CONTACT_CUSTOMER_SUPPORT);
              CUtilDlg::Notice( /*0x431707*/
                sMsg: v52,
                sSndName: v53,
                ppDialog: (ZRef<CDialog> *)v54._m_pStr,
                bAutoSeparated: v55,
                bIsTightLine: v56);
              break; /*0x43170f*/
            case 11: /*0x431413*/
              v49 = StringPool::GetInstance(); /*0x4316bd*/
              v50 = StringPool::GetString( /*0x4316c4*/
                      this: v49,
                      result: &v67,
                      nIdx: SP_4672_IM_A_LITTLE_PREOCCUPIED_RIGHT_NOW_SO_COME_BACK_LATER);
              LOBYTE(v71) = 12; /*0x4316ce*/
              ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v50); /*0x4316d3*/
              m_pStr = v67._m_pStr; /*0x4316d8*/
              LOBYTE(v71) = 4; /*0x4316dc*/
LABEL_46:
              if ( m_pStr != nullptr ) /*0x43144a*/
                ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)m_pStr - 1); /*0x431450*/
              v27 = (int)nType; /*0x431458*/
              if ( nType != nullptr && LOBYTE(nType->_m_pStr) != 0 ) /*0x431467*/
              {
                v31 = (CUtilDlgEx *)ZAllocEx<ZAllocAnonSelector>::Alloc( /*0x431477*/
                                      this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc,
                                      uSize: 0xC50u);
                *(_DWORD *)pQDlg.gap0 = v31; /*0x43147c*/
                LOBYTE(v71) = 13; /*0x431480*/
                if ( v31 != nullptr && (CUtilDlgEx::CUtilDlgEx(this: v31), v33 != 0) && (v34 = v33 + 8) != 0 ) /*0x43149f*/
                {
                  v35 = (CUtilDlgEx *)(v34 - 8); /*0x4314a5*/
                  v69.m_aSendBuff.a = (char *)(v34 - 8); /*0x4314a8*/
                  if ( v34 != 8 ) /*0x4314ae*/
                    InterlockedIncrement(lpAddend: &v35->_m_nRef); /*0x4314b4*/
                }
                else
                {
                  v35 = nullptr; /*0x431714*/
                  v69.m_aSendBuff.a = nullptr; /*0x431716*/
                }
                v56 = 0; /*0x4314ba*/
                v55 = 0; /*0x4314bc*/
                v54._m_pStr = v32; /*0x4314be*/
                *(_DWORD *)pQDlg.gap0 = &v54; /*0x4314c8*/
                LOBYTE(v71) = 14; /*0x4314cd*/
                ZXString<char>::ZXString<char>(this: &v54, s: (ZXString<char> *)&nType); /*0x4314d5*/
                CUtilDlgEx::SetUtilDlgEx( /*0x4314df*/
                  this: v35,
                  dlgType: 0,
                  nTemplateID: m_dwNpcTemplateID,
                  sText: v54,
                  bNoNpc: v55,
                  bQuest: v56);
                CUtilDlgEx::SetUtilDlgEx_TEXT(this: v35, bPrev: 0, bNext: 0); /*0x4314ea*/
                CUtilDlgEx::CreateUtilDlgEx(this: v35); /*0x4314f1*/
                CDialog::DoModal(this: v35); /*0x4314f8*/
                LOBYTE(v71) = 4; /*0x431501*/
                ZRef<CUtilDlgEx>::~ZRef<CUtilDlgEx>(this: (ZRef<CUtilDlgEx> *)&v69); /*0x431506*/
              }
              break; /*0x431506*/
            default:
              break;
          }
          if ( bReOpenDlg != 0 ) /*0x431510*/
          {
            COutPacket::COutPacket(this: &v69, nType: 74); /*0x431518*/
            LOBYTE(v71) = 15; /*0x431523*/
            COutPacket::Encode1(this: &v69, n: 0); /*0x431528*/
            COutPacket::Encode4(this: &v69, n: m_dwNpcTemplateID); /*0x431532*/
            CClientSocket::SendPacket(this: TSingleton<CClientSocket>::ms_pInstance, a2: v36, oPacket: &v69); /*0x431542*/
            LOBYTE(v71) = 4; /*0x43154b*/
            ZArray<unsigned char>::RemoveAll(this: &v69.m_aSendBuff); /*0x431550*/
          }
          v71 = -1; /*0x431555*/
          if ( v27 != 0 ) /*0x43155f*/
            ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)(v27 - 12)); /*0x431565*/
        }
      }
    }
  }
  else
  {
    if ( nType != (ZXString<char> *)367 ) /*0x431129*/
      return; /*0x431129*/
    v2 = iPacket; /*0x43112f*/
    v3 = CInPacket::Decode4(this: iPacket); /*0x43113c*/
    v4 = CInPacket::Decode2(this: v2); /*0x43113e*/
    v5 = v4; /*0x431143*/
    if ( v4 != 0 ) /*0x431148*/
    {
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x431262*/
      {
        if ( TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf( /*0x431275*/
               this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
               a2: &CAdminShopDlg::ms_RTTI_CAdminShopDlg) == 0 )
          return; /*0x431275*/
        v12 = (CAdminShopDlg *)TSingleton<CUniqueModeless>::ms_pInstance; /*0x43127b*/
      }
      else
      {
        v13 = (CAdminShopDlg *)ZAllocEx<ZAllocAnonSelector>::Alloc( /*0x43128d*/
                                 this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc,
                                 uSize: 0xB78u);
        nType = (ZXString<char> *)v13; /*0x431292*/
        v71 = 3; /*0x431296*/
        if ( v13 != nullptr ) /*0x4312a0*/
          CAdminShopDlg::CAdminShopDlg(this: v13); /*0x4312a4*/
        else
          v14 = nullptr; /*0x4312ab*/
        v71 = -1; /*0x4312ad*/
        v12 = v14; /*0x4312b5*/
      }
      if ( v12 != nullptr ) /*0x4312b9*/
        CAdminShopDlg::SetAdminShopDlg(this: v12, iPacket: v2, dwNpcTemplateID: v3, nCommodityCount: v5); /*0x4312c2*/
    }
    else
    {
      v6 = (CUtilDlgEx *)ZAllocEx<ZAllocAnonSelector>::Alloc( /*0x431158*/
                           this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc,
                           uSize: 0xC50u);
      nType = (ZXString<char> *)v6; /*0x43115d*/
      v71 = v5; /*0x431161*/
      if ( v6 != nullptr && (CUtilDlgEx::CUtilDlgEx(this: v6), v8 != 0) && (v9 = v8 + 8) != 0 ) /*0x43117f*/
      {
        v10 = (CUtilDlgEx *)(v9 - 8); /*0x431185*/
        pQDlg.p = (CUtilDlgEx *)(v9 - 8); /*0x431188*/
        if ( v9 != 8 ) /*0x43118e*/
          InterlockedIncrement(lpAddend: &v10->_m_nRef); /*0x431194*/
      }
      else
      {
        v10 = nullptr; /*0x431250*/
        pQDlg.p = nullptr; /*0x431252*/
      }
      v56 = 0; /*0x43119a*/
      v55 = 0; /*0x43119c*/
      v54._m_pStr = v7; /*0x43119e*/
      nType = &v54; /*0x4311a1*/
      v71 = 1; /*0x4311ae*/
      v11 = StringPool::GetInstance(); /*0x4311b9*/
      StringPool::GetString( /*0x4311c0*/
        this: v11,
        result: &v54,
        nIdx: SP_4664_IM_NOT_DOING_BUSINESS_AT_THIS_TIME_DONT_I_DESERVE_SOME_REST_TOO);
      CUtilDlgEx::SetUtilDlgEx(this: v10, dlgType: 0, nTemplateID: v3, sText: v54, bNoNpc: v55, bQuest: v56); /*0x4311ca*/
      CUtilDlgEx::SetUtilDlgEx_TEXT(this: v10, bPrev: 0, bNext: 0); /*0x4311d5*/
      CUtilDlgEx::CreateUtilDlgEx(this: v10); /*0x4311dc*/
      CDialog::DoModal(this: v10); /*0x4311e3*/
      COutPacket::COutPacket(this: &oPacket, nType: 74); /*0x4311ee*/
      LOBYTE(v71) = 2; /*0x4311f9*/
      COutPacket::Encode1(this: &oPacket, n: 2); /*0x4311fe*/
      CClientSocket::SendPacket(this: TSingleton<CClientSocket>::ms_pInstance, a2: (int)&oPacket, &oPacket); /*0x43120e*/
      LOBYTE(v71) = 1; /*0x431217*/
      ZArray<unsigned char>::RemoveAll(this: &oPacket.m_aSendBuff); /*0x43121c*/
      v71 = -1; /*0x431221*/
      if ( v10 != nullptr ) /*0x43122b*/
        ZRef<CUtilDlgEx>::_ReleaseRaw(this: &pQDlg, __formal: nullptr); /*0x431237*/
    }
  }
}