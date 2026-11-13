// Angel 083 `OnDoomed_CMob` @ 0x66d6d4
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
void __thiscall CMob::OnDoomed(CMob *this, int a2)
{
  const struct CMobTemplate *MobTemplate; // eax
  int v4; // edi
  int v5; // eax
  int *v6; // edi
  CVecCtrl *v7; // ebx
  int v8; // eax
  int v9; // eax
  int v10; // esi
  int v11; // eax

  if ( a2 == 0 )
  {
    if ( *(this + 99) == 0 )
      return;
    *(this + 99) = 0;
    goto LABEL_12;
  }
  if ( *(this + 330) != 0 )
  {
    CMovePath::SetStatChangedPoint(this: (*(this + 71) != 0 ? *(this + 71) - 12 + 428 : 428), a2: *(this + 1324));
    *(this + 330) = 0;
  }
  if ( *(this + 99) == 0 )
  {
    MobTemplate = CMobTemplate::GetMobTemplate(a1: 5);
    MobStat::SetFromWhenDoom(this: (this + 416), a2: MobTemplate);
    *(this + 99) = CMobTemplate::GetMobTemplate(a1: 5);
    if ( *(this + 304) == 0 )
      _com_issue_error(a1: -2147467261);
    v4 = *(this + 304);
    v5 = (*(*v4 + 224))(a1: v4, a2: 0xFFFFFF);
    if ( v5 < 0 )
      _com_issue_errorex(a1: v5, a2: v4, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf);
    *(this + 85) = get_update_time() + 1200;
LABEL_12:
    v6 = *(this + 70) != 0 ? (*(this + 70) - 12) : 0;
    v7 = *(this + 71) != 0 ? (*(this + 71) - 12) : 0;
    if ( *(this + 169) == 0 || (v8 = *(this + 99)) == 0 )
      v8 = *(this + 98);
    v6[146] = _ZtlSecureFuse<long>(a1: v8 + 64, a2: *(v8 + 72));
    if ( *(this + 169) == 0 || (v9 = *(this + 99)) == 0 )
      v9 = *(this + 98);
    *(v7 + 146) = _ZtlSecureFuse<long>(a1: v9 + 64, a2: *(v9 + 72));
    CMob::SetShoeAttr(this);
    CMob::ClearActionLayer(this);
    *(this + 242) = _ZtlSecureTear<long>(a1: -1, a2: this + 960);
    CMob::PrepareActionLayer(this);
    CVecCtrl::ResolveMoveAction(this: v6);
    CVecCtrl::ResolveMoveAction(this: v7);
    CMob::SetMoveAction(this, a2: v6[82], a3: 1);
    if ( *(this + 169) != 0 && *(this + 99) != 0 )
      v10 = *(this + 99);
    else
      v10 = *(this + 98);
    v11 = _ZtlSecureFuse<long>(a1: v10 + 64, a2: *(v10 + 72));
    CMovePath::DiscardByInterrupt(this: (v6 + 107), a2: v11, a3: v6, a4: 0);
  }
}