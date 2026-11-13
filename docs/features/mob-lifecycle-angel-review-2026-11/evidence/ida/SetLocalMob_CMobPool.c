// Angel 083 `SetLocalMob_CMobPool` @ 0x678308
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMobPool::SetLocalMob(CMobPool *this, int a2, _DWORD *a3, int a4, struct CInPacket *a5)
{
  int Mob; // eax
  struct CInPacket *v6; // ebx
  CMob *v7; // esi
  char v8; // al
  int MobTemplate; // eax
  int v10; // ecx
  int result; // eax
  _BYTE v12[4]; // [esp+Ch] [ebp-18h] BYREF
  int v13; // [esp+10h] [ebp-14h]
  int v14; // [esp+14h] [ebp-10h]
  int v15; // [esp+20h] [ebp-4h]

  v14 = 0;
  Mob = CMobPool::GetMob(this, a2: a3);
  v6 = a5;
  v7 = Mob;
  v8 = CInPacket::Decode4(this: a5);
  if ( v7 != 0 )
  {
    CMob::SetTemporaryStat(this: v7, a2: v6, a3: a4);
  }
  else
  {
    MobTemplate = CMobTemplate::GetMobTemplate(a1: v8);
    v7 = CreateMob(a1: MobTemplate);
    v10 = v7 != 0 ? v7 + 8 : 0;
    v13 = v10 != 0 ? v10 - 8 : 0;
    sub_6726BC(a1: v12);
    v15 = 0;
    a5 = sub_679950(a1: v12);
    ZMap<unsigned long,__POSITION *,unsigned long>::Insert(a1: &a3, a2: &a5);
    v15 = -1;
    sub_445A87(a1: v12);
    CMob::SetTemporaryStat(this: v7, a2: v6, a3: a4);
    CMob::Init(this: v7, a2: a3, a3: v6);
  }
  result = CMob::SetActive(this: v7, a2: 1);
  if ( a2 > 1 )
    return sub_66B562(
             a1: 1,
             a2: TSingleton<CUserLocal>::ms_pInstance != 0 ? TSingleton<CUserLocal>::ms_pInstance + 4 : 0,
             a3: 0);
  return result;
}