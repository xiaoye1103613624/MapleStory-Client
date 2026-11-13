// Angel 083 `OnMobEnterField_CMobPool` @ 0x67945a
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMobPool::OnMobEnterField(CMobPool *this, struct CInPacket *a2)
{
  int Mob; // eax
  int *v4; // esi
  int MobTemplate; // eax
  int v6; // ecx
  int v7; // ebx
  unsigned int v8; // eax
  int result; // eax
  int v10; // [esp-4h] [ebp-34h]
  int v11; // [esp-4h] [ebp-34h]
  _BYTE v12[4]; // [esp+Ch] [ebp-24h] BYREF
  int v13; // [esp+10h] [ebp-20h]
  int v14; // [esp+14h] [ebp-1Ch] BYREF
  int v15; // [esp+18h] [ebp-18h]
  int v16; // [esp+1Ch] [ebp-14h]
  _DWORD *v17; // [esp+20h] [ebp-10h] BYREF
  int v18; // [esp+2Ch] [ebp-4h]

  v17 = CInPacket::Decode4(this: a2);
  v16 = CInPacket::Decode1(this: a2);
  v15 = CInPacket::Decode4(this: a2);
  Mob = CMobPool::GetMob(this, a2: v17);
  v4 = Mob;
  if ( Mob != 0 )
  {
    v10 = v16;
    *(Mob + 300) = _ZtlSecureTear<int>(t: 1, at: (Mob + 292));
    CMob::SetTemporaryStat(this: v4, a2, a3: v10);
  }
  else
  {
    MobTemplate = CMobTemplate::GetMobTemplate(a1: v15);
    v4 = CreateMob(a1: MobTemplate);
    v6 = v4 != 0 ? (v4 + 2) : 0;
    v13 = v6 != 0 ? v6 - 8 : 0;
    sub_6726BC(a1: v12);
    v18 = 0;
    v7 = sub_6799C6(a1: this + 28);
    sub_6799A3(a1: v12);
    v14 = v7;
    ZMap<unsigned long,__POSITION *,unsigned long>::Insert(a1: &v17, a2: &v14);
    v18 = -1;
    sub_445A87(a1: v12);
    v8 = _ZtlSecureTear<int>(t: 1, at: v4 + 73);
    v11 = v16;
    v4[75] = v8;
    CMob::SetTemporaryStat(this: v4, a2, a3: v11);
    CMob::Init(this: v4, a2: v17, a3: a2);
  }
  result = v4[98];
  if ( *(result + 520) != 0 )
  {
    sub_672305(a1: v4);
    result = get_update_time();
    *(this + 24) = result;
  }
  return result;
}