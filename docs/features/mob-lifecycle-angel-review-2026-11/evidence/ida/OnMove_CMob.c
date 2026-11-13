// Angel 083 `OnMove_CMob` @ 0x66be61
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::OnMove(CMob *this, struct CInPacket *a2)
{
  int v3; // edi
  int v4; // ebx
  char v5; // al
  int result; // eax
  int update_time; // eax
  int v8; // ebx
  int v9; // ebx
  int v10; // ecx
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int v14; // eax
  signed int v15; // edi
  int v16; // eax
  int v17; // eax
  int v18; // eax
  int v19; // eax
  int v20; // eax
  int v21; // edx
  int ActionDelay; // eax
  int *v23; // eax
  int v24; // eax
  int v25; // edx
  int v26; // eax
  int v27; // edi
  int v28; // eax
  int v29; // ecx
  int v30; // ebx
  int v31; // eax
  unsigned int v32; // ecx
  int v33; // eax
  int v34; // ecx
  int v35; // [esp-4h] [ebp-38h]
  int v36; // [esp-4h] [ebp-38h]
  int v37; // [esp-4h] [ebp-38h]
  int v38; // [esp+Ch] [ebp-28h]
  char v39; // [esp+10h] [ebp-24h]
  int v40; // [esp+14h] [ebp-20h]
  int v41; // [esp+18h] [ebp-1Ch]
  signed int v42; // [esp+1Ch] [ebp-18h]
  int v43; // [esp+1Ch] [ebp-18h]
  unsigned int v44; // [esp+20h] [ebp-14h] BYREF
  unsigned __int8 v45; // [esp+27h] [ebp-Dh]
  int v46; // [esp+30h] [ebp-4h]

  v3 = CInPacket::Decode1(this: a2);
  v4 = CInPacket::Decode1(this: a2);
  v38 = v4;
  v5 = CInPacket::Decode1(this: a2);
  v35 = *(this + 78);
  v45 = v5;
  if ( _ZtlSecureFuse<long>(a1: this + 304, a2: v35) != 1 || v4 != 0 || (result = v45 >> 1) >= 0xC && v45 >> 1 <= 0x14u )
  {
    update_time = get_update_time();
    v8 = *(this + 70);
    v44 = update_time;
    v9 = v8 != 0 ? v8 - 12 : 0;
    v40 = v9;
    if ( CMob::IsActive(this) != 0 )
    {
      v10 = *(this + 71) != 0 ? *(this + 71) - 12 : 0;
      (*(*v10 + 4))(a1: v10, a2: 0, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0);
      if ( *(this + 169) == 0 || (v11 = *(this + 99)) == 0 )
        v11 = *(this + 98);
      v12 = _ZtlSecureFuse<long>(a1: v11 + 64, a2: *(v11 + 72));
      CMovePath::DiscardByInterrupt(this: (v9 + 428), a2: v12, a3: v9, a4: v3);
    }
    v42 = CInPacket::Decode4(this: a2);
    if ( (v45 & 0x80u) == 0 )
    {
      if ( *(this + 169) == 0 || (v13 = *(this + 99)) == 0 )
        v13 = *(this + 98);
      v14 = _ZtlSecureFuse<long>(a1: v13 + 64, a2: *(v13 + 72));
      CMovePath::DiscardByInterrupt(this: (v9 + 428), a2: v14, a3: v9, a4: v3);
      v41 = v45 & 1;
      v15 = v45 >> 1;
      v16 = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242));
      if ( (v16 < 12 || v16 > 20) && ((v17 = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242))) < 21 || v17 > 37)
        || v45 >> 1 < 6u
        || v45 >> 1 > 8u )
      {
        v18 = _ZtlSecureTear<long>(a1: v15, a2: this + 960);
        v36 = *(this + 239);
        *(this + 242) = v18;
        v19 = _ZtlSecureFuse<long>(a1: this + 948, a2: v36);
        *(this + 239) = _ZtlSecureTear<long>(a1: v19 ^ (v41 ^ v19) & 1, a2: this + 948);
        CMob::PrepareActionLayer(this);
      }
      if ( v15 < 12 || v15 > 20 )
      {
        if ( v15 < 6 || v15 > 8 )
        {
          if ( v15 >= 21 && v15 <= 37 )
          {
            v39 = _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20));
            v37 = *((*(*(this + 1) + 16))(a1: this + 4) + 4);
            v23 = (*(*(this + 1) + 16))(a1: this + 4);
            v24 = sub_9894F3(a1: *v23, a2: v37);
            sub_989BA1(a1: v39, a2: (v15 - 1), a3: v24);
            v44 = 0;
            v25 = v42;
            v26 = v42 >> 8;
            v43 = 0;
            v27 = v26;
            v28 = *(this + 98);
            while ( 1 )
            {
              v29 = *(v28 + 636);
              if ( v29 == 0 || v44 >= *(v29 - 4) )
                break;
              if ( v25 == *(v43 + v29 + 12) )
              {
                v30 = *(v28 + 636) + 28 * v44;
                goto LABEL_36;
              }
              ++v44;
              v43 += 28;
            }
            v30 = 0;
LABEL_36:
            v31 = sub_7632F2(a1: v25);
            if ( v30 != 0 && v31 != 0 && v27 > 0 )
            {
              v32 = *(v31 + 4);
              if ( v32 != 0 )
                v32 = *(v32 - 4);
              if ( v27 <= v32 )
              {
                MOBSKILLLEVELDATA::GetEffectUOL(a1: 92 * v27 + *(v31 + 4) - 92, a2: &v44);
                v46 = 0;
                if ( v44 != 0 && *v44 != 0 )
                  v33 = *(*v44 - 4) >> 1;
                else
                  v33 = 0;
                if ( v33 != 0 )
                {
                  sub_402C9B(a1: &v44);
                  *(this + 42) = v41;
                  v34 = get_update_time() + *(v30 + 24);
                  *(this + 41) = v34;
                  if ( v34 == 0 )
                    *(this + 41) = 1;
                }
                v46 = -1;
                if ( v44 != 0 )
                {
                  _bstr_t::Data_t::Release(a1: v44);
                  v44 = 0;
                }
              }
            }
            v9 = v40;
          }
        }
        else
        {
          ActionDelay = CMob::GetActionDelay(this, a2: v15);
          *(this + 245) = _ZtlSecureTear<long>(a1: v44 + ActionDelay, a2: this + 972);
        }
      }
      else
      {
        v20 = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242));
        CMob::DoAttack(a1: this, a2: v21, a3: v20, a4: v41, a5: v42);
      }
    }
    CMovePath::OnMovePacket(this: (v9 + 428), a2);
    result = _ZtlSecureTear<long>(a1: -(v38 != 0) - 2, a2: this + 304);
    *(this + 78) = result;
  }
  return result;
}