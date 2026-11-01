void __thiscall MobStat::SetFrom(MobStat *this, CMobTemplate *pTemplate)
{
  int v3; // eax

  memset(a1: this, a2: 0, a3: sizeof(MobStat)); /*0x7249fe*/
  this->nLevel = _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nLevel, uCS: pTemplate->_ZtlSecureTear_nLevel_CS); /*0x724a14*/
  qmemcpy(this->aDamagedElemAttr, pTemplate->aDamagedElemAttr, sizeof(this->aDamagedElemAttr)); /*0x724a25*/
  this->nPAD = _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nPAD, uCS: pTemplate->_ZtlSecureTear_nPAD_CS); /*0x724a3a*/
  this->nPDR = _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nPDR, uCS: pTemplate->_ZtlSecureTear_nPDR_CS); /*0x724a50*/
  this->nMAD = _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nMAD, uCS: pTemplate->_ZtlSecureTear_nMAD_CS); /*0x724a66*/
  this->nMDR = _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nMDR, uCS: pTemplate->_ZtlSecureTear_nMDR_CS); /*0x724a7c*/
  this->nACC = _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nACC, uCS: pTemplate->_ZtlSecureTear_nACC_CS); /*0x724a92*/
  this->nEVA = _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nEVA, uCS: pTemplate->_ZtlSecureTear_nEVA_CS); /*0x724aa8*/
  if ( _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nMoveAbility, uCS: pTemplate->_ZtlSecureTear_nMoveAbility_CS) == 4 ) /*0x724ac1*/
    v3 = _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nFlySpeed, uCS: pTemplate->_ZtlSecureTear_nFlySpeed_CS); /*0x724ad1*/
  else
    v3 = _ZtlSecureFuse<long>(at: pTemplate->_ZtlSecureTear_nSpeed, uCS: pTemplate->_ZtlSecureTear_nSpeed_CS); /*0x724ae1*/
  this->nSpeed = v3; /*0x724ae6*/
  this->nFs = pTemplate->nFs; /*0x724af2*/
  this->bInvincible = _ZtlSecureFuse<int>( /*0x724b0e*/
                        at: pTemplate->_ZtlSecureTear_bInvincible,
                        uCS: pTemplate->_ZtlSecureTear_bInvincible_CS);
  this->bDisable = _ZtlSecureFuse<int>( /*0x724b27*/
                     at: pTemplate->_ZtlSecureTear_bDisable,
                     uCS: pTemplate->_ZtlSecureTear_bDisable_CS);
  this->bCannotEvade = _ZtlSecureFuse<int>( /*0x724b45*/
                         at: pTemplate->_ZtlSecureTear_bCannotEvade,
                         uCS: pTemplate->_ZtlSecureTear_bCannotEvade_CS);
}