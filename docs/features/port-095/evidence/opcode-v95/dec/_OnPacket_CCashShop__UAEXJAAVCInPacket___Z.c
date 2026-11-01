void __thiscall CCashShop::OnPacket(CCashShop *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x4997f2*/
  {
    case 382: /*0x4997f2*/
      CCashShop::OnChargeParamResult(this: (CCashShop *)((char *)this - 8), iPacket); /*0x499801*/
      break; /*0x499806*/
    case 383: /*0x4997f2*/
      CCashShop::OnQueryCashResult(this: (CCashShop *)((char *)this - 8), iPacket); /*0x499811*/
      break; /*0x499816*/
    case 384: /*0x4997f2*/
      CCashShop::OnCashItemResult(this: (CCashShop *)((char *)this - 8), iPacket); /*0x499831*/
      break; /*0x499836*/
    case 385: /*0x4997f2*/
      CCashShop::OnPurchaseExpChanged(this: (CCashShop *)((char *)this - 8), iPacket); /*0x499821*/
      break; /*0x499826*/
    case 386: /*0x4997f2*/
      CCashShop::OnGiftMateInfoResult(this: (CCashShop *)((char *)this - 8), iPacket); /*0x4998a1*/
      break; /*0x4998a6*/
    case 387: /*0x4997f2*/
      CCashShop::OnCheckDuplicatedIDResult(this: (CCashShop *)((char *)this - 8), iPacket); /*0x499841*/
      break; /*0x499846*/
    case 388: /*0x4997f2*/
      CCashShop::OnCheckNameChangePossibleResult(this: (CCashShop *)((char *)this - 8), iPacket); /*0x499851*/
      break; /*0x499856*/
    case 390: /*0x4997f2*/
      CCashShop::OnCheckTransferWorldPossibleResult(this: (CCashShop *)((char *)this - 8), iPacket); /*0x499861*/
      break; /*0x499866*/
    case 391: /*0x4997f2*/
      CCashShop::OnCashShopGachaponStampResult(this: (CCashShop *)((char *)this - 8), iPacket); /*0x499871*/
      break; /*0x499876*/
    case 392: /*0x4997f2*/
    case 393: /*0x4997f2*/
      CCashShop::OnCashItemGachaponResult(this: (CCashShop *)((char *)this - 8), iPacket); /*0x499881*/
      break; /*0x499886*/
    case 395: /*0x4997f2*/
      CCashShop::OnOneADay(this: (CCashShop *)((char *)this - 8), iPacket); /*0x4998b1*/
      break; /*0x4998b6*/
    case 396: /*0x4997f2*/
      CCashShop::OnNoticeFreeCashItem(this: (CCashShop *)((char *)this - 8), iPacket); /*0x4998c1*/
      break; /*0x4998c1*/
    default:
      return;
  }
}