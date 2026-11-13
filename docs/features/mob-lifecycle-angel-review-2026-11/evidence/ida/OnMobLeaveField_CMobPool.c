// Angel 083 `OnMobLeaveField_CMobPool` @ 0x67961d
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMobPool::OnMobLeaveField(CMobPool *this, struct CInPacket *a2)
{
  int v3; // edi
  int result; // eax
  CMob *v5; // edi
  CMob *v6; // ebx
  CMob *v7; // ecx
  _BYTE v8[4]; // [esp+8h] [ebp-1Ch] BYREF
  CMob *v9; // [esp+Ch] [ebp-18h]
  int v10; // [esp+10h] [ebp-14h] BYREF
  int v11; // [esp+14h] [ebp-10h] BYREF
  int v12; // [esp+20h] [ebp-4h]

  v10 = CInPacket::Decode4(this: a2);
  v3 = CInPacket::Decode1(this: a2);
  result = ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(a1: &v10, a2: &v11);
  if ( result != 0 )
  {
    v9 = *(v11 + 4);
    sub_6726BC(a1: v8);
    v12 = 0;
    if ( v3 != 0 )
    {
      v6 = v9;
      *(v6 + 275) = _ZtlSecureTear<long>(a1: v3, a2: v9 + 1092);
      sub_679950(a1: v8);
    }
    else
    {
      if ( timeGetTime() != 0 )
      {
        v5 = v9;
        *(v5 + 75) = _ZtlSecureTear<int>(t: 0, at: v9 + 73);
      }
      if ( CMob::IsActive(this: v9) != 0 )
        goto LABEL_17;
    }
    CMobPool::CancelChaseTarget(this, a2: v9);
    v7 = v9;
    if ( *(*(v9 + 98) + 520) == 0 )
    {
      if ( sub_670C54(a1: v9) == 0 )
      {
LABEL_16:
        CWvsContext::OnMobDead(this: *dword_BE7918);
        sub_679A04(this: this + 7, a2: v11);
        ZMap<unsigned long,__POSITION *,unsigned long>::RemoveKey(a1: &v10);
LABEL_17:
        v12 = -1;
        return sub_445A87(a1: v8);
      }
      v7 = v9;
    }
    if ( *(*(v7 + 98) + 520) != 0 && *(this + 23) == v7 )
    {
      sub_445A87(a1: this + 88);
      v7 = v9;
    }
    if ( sub_670C54(a1: v7) != 0 )
      sub_445A87(a1: this + 100);
    goto LABEL_16;
  }
  return result;
}