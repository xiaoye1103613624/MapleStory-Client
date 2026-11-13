// Angel 083 `SetMoveAction_CMob` @ 0x6644f3
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::SetMoveAction(CMob *this, int a2, int a3)
{
  int FineMoveDirAction; // edi
  int result; // eax
  int v6; // eax
  int v7; // [esp-4h] [ebp-Ch]

  FineMoveDirAction = CMob::GetFineMoveDirAction(this, nMoveDirAction: a2);
  if ( a3 != 0 || (result = _ZtlSecureFuse<long>(a1: this + 948, a2: *(this + 239)), FineMoveDirAction != result) )
  {
    v6 = _ZtlSecureTear<long>(a1: FineMoveDirAction, a2: this + 948);
    v7 = *(this + 242);
    *(this + 239) = v6;
    result = _ZtlSecureFuse<long>(a1: this + 960, a2: v7);
    if ( result <= -1 )
      return CMob::PrepareActionLayer(this);
  }
  return result;
}