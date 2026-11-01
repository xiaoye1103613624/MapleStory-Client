void __thiscall CField::OnPacket(CField *this, int nType, CInPacket *iPacket)
{
  if ( nType > 432 ) /*0x546d59*/
  {
LABEL_40:
    if ( (unsigned int)(nType - 179) > 0x62 ) /*0x546fd7*/
    {
      if ( (unsigned int)(nType - 278) > 5 ) /*0x546ff1*/
      {
        if ( (unsigned int)(nType - 284) > 26 ) /*0x54700b*/
        {
          if ( (unsigned int)(nType - 311) > 7 ) /*0x547025*/
          {
            if ( (unsigned int)(nType - 319) > 2 ) /*0x54703f*/
            {
              if ( (unsigned int)(nType - 322) > 2 ) /*0x547059*/
              {
                if ( (unsigned int)(nType - 325) > 2 ) /*0x547073*/
                {
                  if ( (unsigned int)(nType - 328) > 1 ) /*0x54708d*/
                  {
                    if ( (unsigned int)(nType - 330) > 1 ) /*0x5470a7*/
                    {
                      if ( (unsigned int)(nType - 332) > 1 ) /*0x5470c1*/
                      {
                        if ( (unsigned int)(nType - 334) > 3 ) /*0x5470db*/
                        {
                          if ( nType == 363 ) /*0x5470f4*/
                          {
                            CScriptMan::OnPacket(this: TSingleton<CScriptMan>::ms_pInstance, nType: 363, iPacket); /*0x547100*/
                          }
                          else if ( (unsigned int)(nType - 398) > 2 ) /*0x54710e*/
                          {
                            if ( (unsigned int)(nType - 364) > 1 ) /*0x547128*/
                            {
                              if ( (unsigned int)(nType - 369) > 1 ) /*0x547144*/
                              {
                                if ( (unsigned int)(nType - 141) > 2 ) /*0x547160*/
                                {
                                  if ( (unsigned int)(nType - 144) > 2 ) /*0x547174*/
                                  {
                                    if ( (unsigned int)(nType - 404) > 5 ) /*0x547188*/
                                    {
                                      if ( (unsigned int)(nType - 366) > 1 ) /*0x5471a2*/
                                      {
                                        if ( (unsigned int)(nType - 424) > 3 ) /*0x5471be*/
                                        {
                                          if ( (unsigned int)(nType - 420) > 3 ) /*0x5471d5*/
                                          {
                                            if ( (unsigned int)(nType - 413) > 3 ) /*0x5471ef*/
                                            {
                                              if ( (unsigned int)(nType - 428) <= 3 ) /*0x547206*/
                                                CField::OnVega(this: (CField *)((char *)this - 8), nType, iPacket); /*0x547213*/
                                            }
                                            else
                                            {
                                              CField::OnCharacterSale( /*0x5471f8*/
                                                this: (CField *)((char *)this - 8),
                                                nType,
                                                iPacket);
                                            }
                                          }
                                          else
                                          {
                                            CBattleRecordMan::OnPacket( /*0x5471e1*/
                                              this: TSingleton<CBattleRecordMan>::ms_pInstance,
                                              nType,
                                              iPacket);
                                          }
                                        }
                                        else
                                        {
                                          CField::OnItemUpgrade(this: (CField *)((char *)this - 8), nType, iPacket); /*0x5471c7*/
                                        }
                                      }
                                      else
                                      {
                                        CAdminShopDlg::OnPacket(nType, iPacket); /*0x5471aa*/
                                      }
                                    }
                                    else
                                    {
                                      CMapleTVMan::OnPacket(this: TSingleton<CMapleTVMan>::ms_pInstance, nType, iPacket); /*0x547194*/
                                    }
                                  }
                                  else
                                  {
                                    CMapLoadable::OnPacket(this, nType, iPacket); /*0x54717a*/
                                  }
                                }
                                else
                                {
                                  CStage::OnPacket(this, nType, iPacket); /*0x547166*/
                                }
                              }
                              else
                              {
                                CStoreBankDlg::OnPacket(nType, iPacket); /*0x54714c*/
                              }
                            }
                            else
                            {
                              CShopDlg::OnPacket(nType, iPacket); /*0x547130*/
                            }
                          }
                          else
                          {
                            CFuncKeyMappedMan::OnPacket( /*0x54711a*/
                              this: TSingleton<CFuncKeyMappedMan>::ms_pInstance,
                              nType,
                              iPacket);
                          }
                        }
                        else
                        {
                          CReactorPool::OnPacket(this: TSingleton<CReactorPool>::ms_pInstance, nType, iPacket); /*0x5470e7*/
                        }
                      }
                      else
                      {
                        COpenGatePool::OnPacket(this: TSingleton<COpenGatePool>::ms_pInstance, nType, iPacket); /*0x5470cd*/
                      }
                    }
                    else
                    {
                      CTownPortalPool::OnPacket(this: TSingleton<CTownPortalPool>::ms_pInstance, nType, iPacket); /*0x5470b3*/
                    }
                  }
                  else
                  {
                    CAffectedAreaPool::OnPacket(this: TSingleton<CAffectedAreaPool>::ms_pInstance, nType, iPacket); /*0x547099*/
                  }
                }
                else
                {
                  CMessageBoxPool::OnPacket(this: TSingleton<CMessageBoxPool>::ms_pInstance, nType, iPacket); /*0x54707f*/
                }
              }
              else
              {
                CDropPool::OnPacket(this: TSingleton<CDropPool>::ms_pInstance, nType, iPacket); /*0x547065*/
              }
            }
            else
            {
              CEmployeePool::OnPacket(this: TSingleton<CEmployeePool>::ms_pInstance, nType, iPacket); /*0x54704b*/
            }
          }
          else
          {
            CNpcPool::OnPacket(this: TSingleton<CNpcPool>::ms_pInstance, nType, iPacket); /*0x547031*/
          }
        }
        else
        {
          CMobPool::OnPacket(this: TSingleton<CMobPool>::ms_pInstance, nType, iPacket); /*0x547017*/
        }
      }
      else
      {
        CSummonedPool::OnPacket(this: TSingleton<CSummonedPool>::ms_pInstance, nType, iPacket); /*0x546ffd*/
      }
    }
    else
    {
      CUserPool::OnPacket(this: (CUserPool *)TSingleton<CUserPool>::ms_pInstance.m_pInterface, nType, iPacket); /*0x546fe3*/
    }
  }
  else if ( nType == 432 ) /*0x546d5f*/
  {
    CWvsContext::OnLogoutGift(this: (CWvsContext *)TSingleton<CWvsContext>::ms_pInstance._m_pStr, iPacket); /*0x546fc6*/
  }
  else
  {
    switch ( nType ) /*0x546d7e*/
    {
      case 147: /*0x546d7e*/
        CField::OnTransferFieldReqIgnored(iPacket); /*0x546d8d*/
        break; /*0x546d92*/
      case 148: /*0x546d7e*/
        CField::OnTransferChannelReqIgnored(iPacket); /*0x546d9d*/
        break; /*0x546da2*/
      case 149: /*0x546d7e*/
        CField::OnFieldSpecificData(this: (CField *)((char *)this - 8), iPacket); /*0x546dad*/
        break; /*0x546db2*/
      case 150: /*0x546d7e*/
        CField::OnGroupMessage(this: (CField *)((char *)this - 8), iPacket); /*0x546dbd*/
        break; /*0x546dc2*/
      case 151: /*0x546d7e*/
        CField::OnWhisper(this: (CField *)((char *)this - 8), iPacket); /*0x546ddd*/
        break; /*0x546de2*/
      case 152: /*0x546d7e*/
        CField::OnCoupleMessage(this: (CField *)((char *)this - 8), iPacket); /*0x546dcd*/
        break; /*0x546dd2*/
      case 153: /*0x546d7e*/
        CField::OnSummonItemInavailable(this: (CField *)((char *)this - 8), iPacket); /*0x546ded*/
        break; /*0x546df2*/
      case 154: /*0x546d7e*/
        CField::OnFieldEffect(this: (CField *)((char *)this - 8), iPacket); /*0x546e2d*/
        break; /*0x546e32*/
      case 155: /*0x546d7e*/
        CField::OnFieldObstacleOnOff(this: (CField *)((char *)this - 8), iPacket); /*0x546dfd*/
        break; /*0x546e02*/
      case 156: /*0x546d7e*/
        CField::OnFieldObstacleOnOffStatus(this: (CField *)((char *)this - 8), iPacket); /*0x546e0d*/
        break; /*0x546e12*/
      case 157: /*0x546d7e*/
        CField::OnFieldObstacleAllReset(this: (CField *)((char *)this - 8), iPacket); /*0x546e1d*/
        break; /*0x546e22*/
      case 158: /*0x546d7e*/
        CField::OnBlowWeather(this: (CField *)((char *)this - 8), iPacket); /*0x546e3d*/
        break; /*0x546e42*/
      case 159: /*0x546d7e*/
        CField::OnPlayJukeBox(this: (CField *)((char *)this - 8), iPacket); /*0x546e4d*/
        break; /*0x546e52*/
      case 160: /*0x546d7e*/
        CField::OnAdminResult(iPacket); /*0x546e5d*/
        break; /*0x546e62*/
      case 161: /*0x546d7e*/
        CField::OnQuiz(this: (CField *)((char *)this - 8), iPacket); /*0x546e6d*/
        break; /*0x546e72*/
      case 162: /*0x546d7e*/
        CField::OnDesc(this: (CField *)((char *)this - 8), iPacket); /*0x546e7d*/
        break; /*0x546e82*/
      case 163: /*0x546d7e*/
        (*(void (__thiscall **)(char *, CInPacket *))(*((_DWORD *)this - 2) + 44))(a1: (char *)this - 8, a2: iPacket); /*0x546e93*/
        break; /*0x546e95*/
      case 166: /*0x546d7e*/
        CField::OnSetQuestClear(this: (CField *)((char *)this - 8), iPacket); /*0x546ea0*/
        break; /*0x546ea5*/
      case 167: /*0x546d7e*/
        CField::OnSetQuestTime(this: (CField *)((char *)this - 8), iPacket); /*0x546eb0*/
        break; /*0x546eb5*/
      case 168: /*0x546d7e*/
        CField::OnWarnMessage(this: (CField *)((char *)this - 8), iPacket); /*0x546f80*/
        break; /*0x546f85*/
      case 169: /*0x546d7e*/
        CField::OnSetObjectState(this: (CField *)((char *)this - 8), iPacket); /*0x546ec0*/
        break; /*0x546ec5*/
      case 170: /*0x546d7e*/
        CField::OnDestroyClock(this: (CField *)((char *)this - 8), iPacket); /*0x546ed0*/
        break; /*0x546ed5*/
      case 172: /*0x546d7e*/
        CField::OnStalkResult(iPacket); /*0x546f70*/
        break; /*0x546f75*/
      case 175: /*0x546d7e*/
        CQuickslotKeyMappedMan::OnInit(this: TSingleton<CQuickslotKeyMappedMan>::ms_pInstance, iPacket); /*0x546f93*/
        break; /*0x546f98*/
      case 176: /*0x546d7e*/
        CField::OnFootHoldInfo(this: (CField *)((char *)this - 8), iPacket); /*0x546fa3*/
        break; /*0x546fa8*/
      case 177: /*0x546d7e*/
        CField::OnRequestFootHoldInfo(this: (CField *)((char *)this - 8), iPacket); /*0x546fb3*/
        break; /*0x546fb8*/
      case 196: /*0x546d7e*/
        return;
      case 359: /*0x546d7e*/
        CField::OnHontaleTimer(iPacket); /*0x546f00*/
        break; /*0x546f05*/
      case 360: /*0x546d7e*/
        CField::OnChaosZakumTimer(iPacket); /*0x546ef0*/
        break; /*0x546ef5*/
      case 361: /*0x546d7e*/
        CField::OnHontailTimer(iPacket); /*0x546f10*/
        break; /*0x546f15*/
      case 362: /*0x546d7e*/
        CField::OnZakumTimer(iPacket); /*0x546ee0*/
        break; /*0x546ee5*/
      case 368: /*0x546d7e*/
        CTrunkDlg::OnPacket(iPacket); /*0x546f2d*/
        break; /*0x546f35*/
      case 371: /*0x546d7e*/
        CRPSGameDlg::OnPacket(iPacket); /*0x546f3d*/
        break; /*0x546f45*/
      case 372: /*0x546d7e*/
        CUIMessenger::OnPacket(iPacket); /*0x546f4d*/
        break; /*0x546f55*/
      case 373: /*0x546d7e*/
        CMiniRoomBaseDlg::OnPacketBase(iPacket); /*0x546f5d*/
        break; /*0x546f65*/
      case 381: /*0x546d7e*/
        CParcelDlg::OnPacket(iPacket); /*0x546f1d*/
        break; /*0x546f25*/
      default:
        goto LABEL_40;
    }
  }
}