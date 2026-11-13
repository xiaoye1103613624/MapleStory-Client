// Angel 083 `ShowDamage_CMob` @ 0x6691d3
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
void __userpurge CMob::ShowDamage(CMob *this@<ecx>, int nDamage, int a3, int *a4, int bHalfHeight, int a6, int a7)
{
  int v8; // ebx
  int v9; // eax

  if ( *(this + 72) == 0 )
    _com_issue_error(a1: -2147467261);
  v8 = IWzShape2D::Getx(a1: *(this + 72));
  if ( *(this + 72) == 0 )
    _com_issue_error(a1: -2147467261);
  v9 = -15 - a3 * (bHalfHeight != 0 ? 15 : 30) + IWzShape2D::Gety(a1: *(this + 72));
  if ( nDamage <= 0 )
  {
    if ( nDamage != 0 )
      CAnimationDisplayer::Effect_HP(this: dword_BEBF6C, lCenterLeft: v8, lCenterTop: v9, idx: -nDamage, a5: 1, lY: 0);
    else
      CAnimationDisplayer::Effect_Miss(this: dword_BEBF6C, a2: v8, a3: v9, a4: 0);
  }
  else
  {
    CAnimationDisplayer::Effect_HP(this: dword_BEBF6C, lCenterLeft: v8, lCenterTop: v9, idx: nDamage, a5: 0, lY: a4);
  }
}