# Angel v83 登录链路 — 收包分支 / 错误码 / 状态转移 逐行摘录

抽取自 `angel-login-decompile.json`。保留：case/switch、Decode/Encode、
Notice 错误码、goto/return、状态字段赋值、Send*Packet、Goto*、
CWndMan::SetFocus、CFadeWnd::Close、StringPool 文案常量。

## CLogin::OnCheckPasswordResult @ 0x5f83ee

```c
*(this + 92) = 0; /*0x5f840d*/
v99 = CInPacket::Decode1(this: v4); /*0x5f841d*/
*(this + 456) = CInPacket::Decode1(this: v2); /*0x5f8427*/
CInPacket::Decode4(this: v2); /*0x5f842d*/
if ( *dword_BEDA60 != 0 ) /*0x5f843a*/
if ( v99 == 2 )
v6 = CInPacket::Decode1(this: v2); /*0x5f8454*/
CInPacket::DecodeBuffer(a1: v87, a2: 8u); /*0x5f845f*/
if ( v6 == 0 ) /*0x5f8466*/
goto LABEL_51; /*0x5f8466*/
if ( v6 > 20 && v6 < 41 ) /*0x5f8474*/
if ( v89 - LOWORD(v78[0]) >= 3 ) /*0x5f84d9*/
nIdx: SP_2875_THE_ID_HAS_BEEN_PERMANENTLY_BLOCKED_R_NSO_YOU_WONT_BE_ABLE_TO_USE_THIS_ACCOUNT);
else
if ( v92 <= 0xCu ) /*0x5f84e0*/
else
v9 = *StringPool::GetString(this: v8, result: &a2, nIdx: SP_2837_YOU_CAN_LOGIN_AFTER_R_NS_2D_4D_2D02D_S); /*0x5f8509*/
if ( v6 == 99 ) /*0x5f858d*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f85aa*/
return; /*0x5f8a0a*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f85f8*/
StringPool::GetBSTR(this: v14, a2: &v71, a3: SP_2836_ETC_BLOCKREASONIMG); /*0x5f8619*/
if ( a2 != 0 ) /*0x5f8654*/
if ( v17 < 0 && v17 != -2147467262 ) /*0x5f8668*/
if ( v18 < 0 ) /*0x5f8683*/
if ( v19 < 0 ) /*0x5f869b*/
if ( v20 < 0 ) /*0x5f86b3*/
if ( v97 != 0 )
v66 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f86cf*/
if ( Unknown != 0 ) /*0x5f8710*/
if ( v23 < 0 && v23 != -2147467262 ) /*0x5f8724*/
if ( v24 < 0 ) /*0x5f873f*/
if ( result != 0 )
v66 = SP_1587_MSG; /*0x5f8759*/
v27 = *Item == 8; /*0x5f8785*/
if ( v29 < 0 ) /*0x5f87b7*/
v66 = SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID; /*0x5f87c0*/
if ( result != 0 ) /*0x5f8814*/
if ( v97 != 0 ) /*0x5f8824*/
goto LABEL_16; /*0x5f8830*/
if ( v99 == 27 ) /*0x5f8847*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f884d*/
nIdx: SP_4836_PLEASE_DOWNLOAD_THE_FULL_CLIENT_TO_EXPERIENCE__R_NTHE_WORLD_OF_MAPLESTORY__R_NWO);
if ( CUtilDlg::YesNo(a1: v71, a2: &v72->vt, a3: p_pvargDest, a4: p_result, a5: v75) == 6 ) /*0x5f8874*/
v75 = SP_3_HTTP__MAPLESTORYNEXONNET_WZASPXPART_DOWNLOADS_GAMEPATCHES; /*0x5f8879*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f88c6*/
nIdx: SP_4822_IF_YOU_WISH_TO_DOWNLOAD_THE_FULL_CLIENT_LATER__R_NYOU_CAN_ALWAYS_VISIT_MAPLESTOR);
return; /*0x5f88ea*/
if ( v99 > 11 ) /*0x5f88f2*/
switch ( v99 ) /*0x5f894f*/
case 13: /*0x5f894f*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8a0f*/
goto LABEL_76; /*0x5f8a12*/
case 14: /*0x5f894f*/
if ( CLoginUtilDlg::YesNo2(a1: 27) == 0 ) /*0x5f89d1*/
return; /*0x5f89d1*/
v75 = SP_2871_HTTP__WWWNEXONNET; /*0x5f89da*/
goto LABEL_73; /*0x5f8a02*/
case 15: /*0x5f894f*/
if ( CLoginUtilDlg::YesNo2(a1: 26) == 0 ) /*0x5f8991*/
return; /*0x5f8991*/
v75 = SP_2871_HTTP__WWWNEXONNET; /*0x5f899a*/
goto LABEL_73; /*0x5f89c5*/
default:
break; /*0x5f895c*/
if ( v99 != 16 ) /*0x5f895c*/
if ( v99 == 17 ) /*0x5f895f*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8977*/
goto LABEL_107; /*0x5f897a*/
if ( v99 != 21 ) /*0x5f8964*/
if ( v99 != 25 ) /*0x5f8969*/
goto LABEL_76; /*0x5f8969*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f896f*/
return; /*0x5f8d78*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f897f*/
goto LABEL_107; /*0x5f8982*/
if ( v99 == 11 ) /*0x5f88f4*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8944*/
goto LABEL_107; /*0x5f8947*/
switch ( v99 ) /*0x5f8903*/
case 6: /*0x5f8903*/
case 8: /*0x5f8903*/
case 9: /*0x5f8903*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f890a*/
goto LABEL_75; /*0x5f890d*/
case 2: /*0x5f8903*/
case 3: /*0x5f8903*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8912*/
goto LABEL_75; /*0x5f8915*/
case 4: /*0x5f8903*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8922*/
goto LABEL_75; /*0x5f8925*/
case 5: /*0x5f8903*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f891a*/
goto LABEL_75; /*0x5f891d*/
case 7: /*0x5f8903*/
CLogin::GotoTitle(this: v100, a2: 0, a3: 0); /*0x5f892f*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8934*/
goto LABEL_75; /*0x5f8937*/
case 10: /*0x5f8903*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f893c*/
goto LABEL_75; /*0x5f893f*/
default:
break;
if ( v99 != 0 && v99 != 12 && v99 != 23 ) /*0x5f8a28*/
return; /*0x5f8a28*/
if ( v42 <= 1u ) /*0x5f8a39*/
if ( *dword_BEDA60 != 0 ) /*0x5f8a99*/
if ( v40 == 23 ) /*0x5f8aa4*/
if ( result != 0 ) /*0x5f8ac1*/
return; /*0x5f8acf*/
a2 = CInPacket::Decode4(this: result); /*0x5f8ae0*/
v46 = CInPacket::Decode1(this: v45); /*0x5f8ae8*/
LOBYTE(Unknown) = CInPacket::Decode1(this: v45); /*0x5f8af7*/
LOBYTE(v99) = CInPacket::Decode1(this: v45); /*0x5f8b01*/
LOBYTE(v95) = CInPacket::Decode1(this: v45); /*0x5f8b09*/
CInPacket::DecodeStr(a1: v94); /*0x5f8b12*/
v101[0] = CInPacket::Decode1(this: v45); /*0x5f8b27*/
LOBYTE(v97) = CInPacket::Decode1(this: v45); /*0x5f8b2f*/
CInPacket::DecodeBuffer(a1: v83, a2: 8u); /*0x5f8b3a*/
CInPacket::DecodeBuffer(a1: v84, a2: 8u); /*0x5f8b58*/
v47 = CInPacket::Decode4(this: v45); /*0x5f8b78*/
if ( v46 == 10 ) /*0x5f8b7a*/
if ( *dword_BEDA60 != 0 ) /*0x5f8b88*/
if ( *dword_BEDA70 == 0 ) /*0x5f8b97*/
if ( v48 != 0 ) /*0x5f8bb1*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8bce*/
goto LABEL_97; /*0x5f8c26*/
if ( *dword_BEDA70 != 0 ) /*0x5f8c3f*/
CWnd::Destroy(a1: *dword_BEDA70); /*0x5f8c41*/
if ( *dword_BEDA70 != 0 ) /*0x5f8c4d*/
v52 = CInPacket::Decode1(this: result); /*0x5f8cce*/
v53 = CInPacket::Decode1(this: result); /*0x5f8cd1*/
if ( v52 != 0 ) /*0x5f8ce1*/
COutPacket::COutPacket(a1: 11); /*0x5f8d4d*/
CClientSocket::SendPacket(this: dword_BE7914, a2: &pvargDest); /*0x5f8d60*/
else
if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) == 1 ) /*0x5f8cec*/
goto LABEL_73; /*0x5f8c32*/
COutPacket::COutPacket(a1: 9); /*0x5f8cf7*/
COutPacket::Encode1(this: &v89, a2: 1); /*0x5f8d05*/
COutPacket::Encode1(this: &v89, a2: 1); /*0x5f8d0f*/
COutPacket::EncodeStr(a1: p_result); /*0x5f8d2b*/
CClientSocket::SendPacket(this: dword_BE7914, a2: &v89); /*0x5f8d3a*/
ZArray<unsigned char>::RemoveAll(a1: p_Hi32); /*0x5f8d6c*/
goto LABEL_97; /*0x5f8d71*/
v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8a3d*/
if ( v42 > 3u ) /*0x5f8a3e*/
goto LABEL_107; /*0x5f8d76*/
if ( CLoginUtilDlg::YesNo(a1: 31, a2: v75) != 0 && *(v41 + 90) == 0 ) /*0x5f8a5b*/
v75 = SP_7_HTTP__PASSPORTNEXONNET_PART_MYMAPLE_VERIFYCODE; /*0x5f8a64*/
goto LABEL_73; /*0x5f8a8c*/
```

## CLogin::OnWorldInformation @ 0x5f95b7

```c
a2 = CInPacket::Decode1(this: a2); /*0x5f95d8*/
if ( a2 >= 0 ) /*0x5f95db*/
v4 = sub_5FDE1F(a1: -1); /*0x5f962f*/
v5 = CInPacket::DecodeStr(a1: &a2); /*0x5f963c*/
if ( a2 != 0 ) /*0x5f965a*/
v4[2] = CInPacket::Decode1(this: v2); /*0x5f9670*/
v6 = CInPacket::DecodeStr(a1: &a2); /*0x5f9679*/
if ( a2 != 0 ) /*0x5f9697*/
v4[4] = CInPacket::Decode2(this: v2); /*0x5f96af*/
v4[5] = CInPacket::Decode2(this: v2); /*0x5f96bc*/
v4[6] = CInPacket::Decode1(this: v2); /*0x5f96c9*/
v7 = CInPacket::Decode1(this: v2); /*0x5f96cc*/
if ( v7 != 0 ) /*0x5f96d6*/
v8 = sub_5FDC66(a1: -1); /*0x5f96eb*/
v9 = CInPacket::DecodeStr(a1: &v17); /*0x5f96f3*/
if ( v17 != 0 ) /*0x5f9710*/
v8[1] = CInPacket::Decode4(this: v2); /*0x5f9725*/
v8[2] = CInPacket::Decode1(this: v2); /*0x5f9732*/
v8[3] = CInPacket::Decode1(this: v2); /*0x5f973f*/
v10 = CInPacket::Decode1(this: v2); /*0x5f9742*/
v11 = a2-- == 1; /*0x5f9747*/
*(this + 128) = CInPacket::Decode2(this: v2); /*0x5f9762*/
sub_5FDF26(a1: this + 516); /*0x5f9768*/
if ( v12 != 0 ) /*0x5f9775*/
if ( v12 > 0 ) /*0x5f977b*/
v15[0] = CInPacket::Decode2(this: v2); /*0x5f9794*/
v15[1] = CInPacket::Decode2(this: v2); /*0x5f979f*/
v14 = CInPacket::DecodeStr(a1: &v18); /*0x5f97a8*/
sub_5FD8B5(a1: v15, a2: -1); /*0x5f97d2*/
++v13; /*0x5f97e3*/
else
if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f95e5*/
else
if ( a2 != 0 ) /*0x5f95ff*/
CLogin::ChangeStep(this, a2: 1); /*0x5f9618*/
```

## CLogin::OnSelectWorldResult @ 0x5f9891

```c
*(this + 92) = 0; /*0x5f98a5*/
if ( dword_BEDA78 != 0 ) /*0x5f98b3*/
CWnd::Destroy(a1: dword_BEDA78); /*0x5f98b5*/
if ( dword_BEDA78 != 0 ) /*0x5f98c1*/
if ( dword_BEDA74 != 0 ) /*0x5f98d2*/
v3 = CInPacket::Decode1(this: a2); /*0x5f98e9*/
if ( v3 > 0xBu ) /*0x5f98f5*/
if ( v3 != 13 ) /*0x5f994f*/
switch ( v3 ) /*0x5f9956*/
case 0xEu: /*0x5f9956*/
if ( CLoginUtilDlg::YesNo2(a1: 27) == 0 ) /*0x5f99d2*/
return; /*0x5f99d2*/
v9 = *StringPool::GetString(this: Instance, result: &v24, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5f99ed*/
break;
case 0xFu: /*0x5f9956*/
if ( CLoginUtilDlg::YesNo2(a1: 26) == 0 ) /*0x5f9992*/
return; /*0x5f9992*/
v6 = *StringPool::GetString(this: v5, result: &v23, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5f99ad*/
break;
case 0x10u: /*0x5f9956*/
return; /*0x5f997c*/
case 0x11u: /*0x5f9956*/
return; /*0x5f9977*/
case 0x15u: /*0x5f9956*/
break;
case 0x19u: /*0x5f9956*/
return; /*0x5f9972*/
default:
goto LABEL_33; /*0x5f9969*/
return; /*0x5f9a07*/
else
if ( v3 == 11 ) /*0x5f98f7*/
return; /*0x5f9948*/
switch ( v3 ) /*0x5f9905*/
case 0xFF: /*0x5f9905*/
case 6u: /*0x5f9905*/
case 8u: /*0x5f9905*/
case 9u: /*0x5f9905*/
break; /*0x5f990f*/
case 2u: /*0x5f9905*/
case 3u: /*0x5f9905*/
break; /*0x5f9917*/
case 4u: /*0x5f9905*/
break; /*0x5f9927*/
case 5u: /*0x5f9905*/
break; /*0x5f991f*/
case 7u: /*0x5f9905*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5f9930*/
break; /*0x5f9938*/
case 0xAu: /*0x5f9905*/
break; /*0x5f9940*/
default:
break;
if ( v4 == 0 || v4 == 12 || v4 == 23 ) /*0x5f9a22*/
*(this + 130) = *(*dword_BE7918 + 8276); /*0x5f9a39*/
v12 = CInPacket::Decode1(this: a2); /*0x5f9a61*/
if ( v25 >= v12 ) /*0x5f9a7a*/
else
*v14 = CInPacket::Decode1(this: a2); /*0x5f9abd*/
if ( CInPacket::Decode1(this: a2) != 0 ) /*0x5f9acb*/
CInPacket::DecodeBuffer(a1: (v23 + *(this + 102)), a2: 0x10u); /*0x5f9ada*/
else
if ( *(v24 + *(this + 101)) == v22 ) /*0x5f9b04*/
*(this + 100) = v25; /*0x5f9b09*/
++v25; /*0x5f9b11*/
*(this + 372) = CInPacket::Decode1(this: a2); /*0x5f9b34*/
v15 = CInPacket::Decode4(this: a2); /*0x5f9b3a*/
*(this + 95) = v15; /*0x5f9b3f*/
if ( v16 != 0 ) /*0x5f9b6e*/
if ( v18 != 0 ) /*0x5f9b94*/
CLogin::ChangeStep(this, a2: 0xFFFFFFFF); /*0x5f9bad*/
if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) == 1 && *(this + 372) == 0 ) /*0x5f9bc4*/
COutPacket::COutPacket(a1: 9); /*0x5f9bcb*/
COutPacket::Encode1(this: &v20, a2: 1); /*0x5f9bdc*/
COutPacket::Encode1(this: &v20, a2: 1); /*0x5f9be6*/
COutPacket::EncodeStr(a1: 0); /*0x5f9c01*/
CClientSocket::SendPacket(this: dword_BE7914, a2: &v20); /*0x5f9c10*/
ZArray<unsigned char>::RemoveAll(a1: v21); /*0x5f9c1b*/
```

## CLogin::OnCheckDuplicatedIDResult @ 0x5f9c72

```c
*(this + 92) = 0; /*0x5f9c8c*/
CInPacket::DecodeStr(a1: v5); /*0x5f9c92*/
v3 = CInPacket::Decode1(this: a2); /*0x5f9c9d*/
if ( v3 == 1 ) /*0x5f9cb1*/
else if ( v3 == 2 ) /*0x5f9cbb*/
else if ( v3 != 0 ) /*0x5f9cc4*/
else
*(this + 142) = 1; /*0x5f9cd5*/
*(this + 143) = 1; /*0x5f9cdb*/
```

## CLogin::OnCreateNewCharacterResult @ 0x5fa26c

```c
*(this + 92) = 0; /*0x5fa281*/
v3 = CInPacket::Decode1(this: a2); /*0x5fa287*/
switch ( v3 ) /*0x5fa292*/
case 10: /*0x5fa292*/
goto LABEL_32; /*0x5fa297*/
case 26: /*0x5fa292*/
nIdx: SP_4016_YOU_CANNOT_CREATE_A_NEW_CHARACTER__R_NUNDER_THE_ACCOUNT_THAT__R_NHAS_REQUESTED_F);
return; /*0x5fa2c5*/
case 30: /*0x5fa292*/
return; /*0x5fa2d9*/
default:
break; /*0x5fa2de*/
while ( *v7 != 0 ) /*0x5fa2ec*/
++v8; /*0x5fa2ee*/
if ( v8 >= 15 ) /*0x5fa2f7*/
goto LABEL_12; /*0x5fa2f7*/
++*(*dword_BE7918 + 8344); /*0x5fa2fe*/
if ( v5 != 0 || v25 < 0 ) /*0x5fa31e*/
if ( TSingleton<CWndMan>::ms_pInstance != 0 ) /*0x5fa459*/
CWndMan::SetFocus(a1: *(this + 98)); /*0x5fa461*/
return; /*0x5fa461*/
if ( v12 != 0 && *v12 != 0 ) /*0x5fa384*/
if ( v14 == 0 ) /*0x5fa398*/
if ( *v13 == 0 ) /*0x5fa3a0*/
if ( _strcmpi(a1: v15, a2: v14) == 0 ) /*0x5fa3af*/
if ( dword_BEDA4C == 0 ) /*0x5fa3c4*/
if ( v16 != 0 ) /*0x5fa3db*/
if ( *dword_BE7918 != 0 ) /*0x5fa412*/
if ( v18 != 0 ) /*0x5fa42e*/
CLogin::ChangeStep(this, a2: 2); /*0x5fa440*/
```

## CLogin::OnDeleteCharacterResult @ 0x5f9d15

```c
*(this + 92) = 0; /*0x5f9d2a*/
v20 = CInPacket::Decode4(this: v3); /*0x5f9d38*/
v4 = CInPacket::Decode1(this: a2); /*0x5f9d3b*/
switch ( v4 ) /*0x5f9d46*/
case 10: /*0x5f9d46*/
return; /*0x5f9dff*/
case 22: /*0x5f9d46*/
goto LABEL_20; /*0x5f9d5e*/
case 29: /*0x5f9d46*/
goto LABEL_20; /*0x5f9d71*/
case 18: /*0x5f9d46*/
goto LABEL_20; /*0x5f9d7e*/
case 20: /*0x5f9d46*/
goto LABEL_20; /*0x5f9d8e*/
case 24: /*0x5f9d46*/
goto LABEL_20; /*0x5f9d9e*/
case 9: /*0x5f9d46*/
goto LABEL_20; /*0x5f9dae*/
case 26: /*0x5f9d46*/
nIdx: SP_4011_YOU_CANNOT_DELETE_A_CHARACTER_THAT_R_N_IS_CURRENTLY_GOING_THROUGH_THE_TRANSFER);
return; /*0x5f9dd1*/
case 6: /*0x5f9d46*/
goto LABEL_20; /*0x5f9df6*/
default:
break; /*0x5f9e01*/
++v10; /*0x5f9e16*/
if ( v10 >= 15 ) /*0x5f9e1f*/
goto LABEL_26; /*0x5f9e1f*/
--*(*dword_BE7918 + 8344); /*0x5f9e28*/
if ( v6 != 0 || v9 < 0 ) /*0x5f9e45*/
if ( v9 < 14 ) /*0x5f9e4e*/
if ( dword_BEDA4C != 0 ) /*0x5f9ee5*/
if ( *(684 * v9 + *(this + 101)) == 0 ) /*0x5f9efd*/
if ( v9 <= 0 ) /*0x5f9f01*/
goto LABEL_38; /*0x5f9f07*/
--v9; /*0x5f9f03*/
if ( *(this + 100) == -1 ) /*0x5f9f1b*/
if ( dword_BEDA50 != 0 ) /*0x5f9f3d*/
CWnd::Destroy(a1: dword_BEDA50); /*0x5f9f3f*/
if ( dword_BEDA50 != 0 ) /*0x5f9f4b*/
if ( TSingleton<CWndMan>::ms_pInstance != 0 ) /*0x5f9f5e*/
CWndMan::SetFocus(a1: *(this + 98)); /*0x5f9f6a*/
```

## CLogin::OnViewAllCharResult @ 0x5facca

```c
if ( *(this + 71) != 0 ) /*0x5face4*/
switch ( CInPacket::Decode1(this: a2) ) /*0x5fad00*/
case 0: /*0x5fad00*/
v28 = CInPacket::Decode1(this: v3); /*0x5faf1e*/
v16 = CInPacket::Decode1(this: v3); /*0x5faf2e*/
if ( v16 > 0 ) /*0x5faf41*/
for ( i = v16; i != 0; --i ) /*0x5faf47*/
*(this + 76) = v17 + 1; /*0x5faf68*/
if ( CInPacket::Decode1(this: a2) != 0 ) /*0x5faf9c*/
CInPacket::DecodeBuffer(a1: (*(this + 79) + 16 * v17), a2: v26); /*0x5fafab*/
else
v6 = (*(this + 84))-- == 1; /*0x5fafd5*/
if ( v6 ) /*0x5fafe1*/
--*(this + 74); /*0x5faff3*/
if ( *(this + 74) <= 0 ) /*0x5fb008*/
v6 = (*(a2 + 1))-- == 1; /*0x5fb01a*/
if ( v6 ) /*0x5fb01d*/
CLogin::ChangeStep(this, a2: 5); /*0x5fb02c*/
if ( dword_BEDA38 == 0 ) /*0x5fb037*/
goto LABEL_38; /*0x5fb059*/
goto LABEL_44; /*0x5fb037*/
v6 = (*(a2 + 1))-- == 1; /*0x5fb00a*/
if ( v6 ) /*0x5fb00d*/
return; /*0x5fb015*/
case 1: /*0x5fad00*/
v4 = CInPacket::Decode4(this: a2); /*0x5fad1d*/
*(this + 74) = v4; /*0x5fad25*/
*(this + 75) = CInPacket::Decode4(this: v5); /*0x5fad34*/
v6 = (*(this + 86))-- == 1; /*0x5fad3a*/
if ( v6 ) /*0x5fad3d*/
*(this + 85) = 0; /*0x5fad3f*/
if ( *(this + 74) == 0 || *(this + 75) == 0 ) /*0x5fad4f*/
goto LABEL_18; /*0x5fad4f*/
sub_5FDD2D(a1: 60, a2: &v32); /*0x5fad5d*/
sub_5FDDA6(a1: 60, a2: &v31); /*0x5fad6e*/
sub_5FDBEE(a1: 60, a2: &v30); /*0x5fad7f*/
return; /*0x5fad84*/
case 2: /*0x5fad00*/
v21 = SP_3989_YOU_ARE_ALREADY_CONNECTED_TO_SERVER; /*0x5fadbe*/
goto LABEL_13; /*0x5fadc3*/
case 3: /*0x5fad00*/
case 6: /*0x5fad00*/
case 7: /*0x5fad00*/
if ( CInPacket::Decode1(this: v3) != 0 ) /*0x5faddc*/
CInPacket::DecodeStr(a1: &a2); /*0x5fade4*/
else
v21 = SP_3990_UNKNOWN_ERROR__VIEW_ALL_CHARACTERS; /*0x5fae25*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb0a7*/
break; /*0x5fb0ab*/
case 4: /*0x5fad00*/
case 5: /*0x5fad00*/
if ( --*(this + 74) > 0 ) /*0x5fae5b*/
goto LABEL_40; /*0x5fae5b*/
v6 = (*(this + 86))-- == 1; /*0x5fae61*/
if ( v6 ) /*0x5fae64*/
if ( *(this + 76) != 0 ) /*0x5fae6e*/
CLogin::ChangeStep(this, a2: 5); /*0x5faeb4*/
if ( dword_BEDA38 != 0 ) /*0x5faebf*/
if ( *(this + 70) != 0 ) /*0x5faf00*/
else
if ( v14 != 0 ) /*0x5fb062*/
v6 = (*(v11 + 1))-- == 1; /*0x5fb072*/
if ( v6 ) /*0x5fb075*/
else
StringPool::GetString(this: v13, result: &v22, nIdx: SP_3988_CANNOT_FIND_ANY_CHARACTERS); /*0x5fae95*/
CLogin::GotoWorldSelect(this, a2: 1); /*0x5faea6*/
break; /*0x5faeab*/
default:
goto LABEL_43; /*0x5fb09a*/
```

## CLogin::OnSelectCharacterResult @ 0x5fb541

```c
*(this + 92) = 0; /*0x5fb558*/
v4 = CInPacket::Decode1(this: v3); /*0x5fb566*/
v17 = CInPacket::Decode1(this: a2); /*0x5fb574*/
if ( v4 <= 11 ) /*0x5fb577*/
if ( v4 == 11 ) /*0x5fb579*/
return; /*0x5fb5ca*/
switch ( v4 ) /*0x5fb587*/
case 2: /*0x5fb587*/
case 3: /*0x5fb587*/
break; /*0x5fb599*/
case 4: /*0x5fb587*/
break; /*0x5fb5a9*/
case 5: /*0x5fb587*/
break; /*0x5fb5a1*/
case 6: /*0x5fb587*/
case 8: /*0x5fb587*/
case 9: /*0x5fb587*/
break; /*0x5fb591*/
case 7: /*0x5fb587*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb5b2*/
break; /*0x5fb5ba*/
case 10: /*0x5fb587*/
break; /*0x5fb5c2*/
default:
goto LABEL_27;
goto LABEL_27;
switch ( v4 ) /*0x5fb5d1*/
case 13: /*0x5fb5d1*/
break; /*0x5fb691*/
case 14: /*0x5fb5d1*/
if ( CLoginUtilDlg::YesNo2(a1: 27) == 0 ) /*0x5fb654*/
return; /*0x5fb654*/
v13 = *StringPool::GetString(this: Instance, result: &v18, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5fb672*/
goto LABEL_25; /*0x5fb681*/
case 15: /*0x5fb5d1*/
if ( CLoginUtilDlg::YesNo2(a1: 26) == 0 ) /*0x5fb614*/
return; /*0x5fb614*/
v12 = *StringPool::GetString(this: v5, result: &v19, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5fb632*/
return; /*0x5fb689*/
case 16: /*0x5fb5d1*/
goto LABEL_20; /*0x5fb5de*/
case 17: /*0x5fb5d1*/
return; /*0x5fb5f9*/
case 21: /*0x5fb5d1*/
return; /*0x5fb605*/
case 25: /*0x5fb5d1*/
return; /*0x5fb5f4*/
default:
break; /*0x5fb69a*/
if ( v4 != 0 ) /*0x5fb69a*/
if ( v4 == 12 ) /*0x5fb69f*/
if ( v17 > 13 ) /*0x5fb6b9*/
switch ( v17 ) /*0x5fb707*/
case 19: /*0x5fb707*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb740*/
return; /*0x5fb748*/
case 25: /*0x5fb707*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb72f*/
return; /*0x5fb737*/
case 27: /*0x5fb707*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb71e*/
return; /*0x5fb726*/
default:
break; /*0x5fb707*/
else
switch ( v17 ) /*0x5fb6bb*/
case 13: /*0x5fb6bb*/
goto LABEL_48; /*0x5fb6bb*/
case 1: /*0x5fb6bb*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb6f7*/
return; /*0x5fb6ff*/
case 2: /*0x5fb6bb*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb6e6*/
return; /*0x5fb6ee*/
case 3: /*0x5fb6bb*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb6d5*/
return; /*0x5fb6dd*/
case 11: /*0x5fb6bb*/
goto LABEL_48; /*0x5fb6cd*/
default:
break; /*0x5fb6bb*/
return; /*0x5fb715*/
if ( v4 != 23 ) /*0x5fb6a4*/
return; /*0x5fb6a4*/
v18 = CInPacket::Decode4(this: a2); /*0x5fb75f*/
LOWORD(v10) = CInPacket::Decode2(this: v8); /*0x5fb762*/
v19 = CInPacket::Decode4(this: v8); /*0x5fb773*/
LOBYTE(v8) = CInPacket::Decode1(this: v8); /*0x5fb77e*/
a2 = CInPacket::Decode4(this: a2); /*0x5fb785*/
v9[2071] = (v8 & 2) != 0; /*0x5fb7c0*/
v14 = *StringPool::GetStringW(this: v11, &a2, a3: SP_1243_GAMEIN); /*0x5fb7e4*/
```

## CLogin::OnConfirmEULAResult @ 0x5fc838

```c
if ( dword_BEDA78 != 0 ) /*0x5fc843*/
CWnd::Destroy(a1: dword_BEDA78); /*0x5fc845*/
if ( dword_BEDA78 != 0 ) /*0x5fc851*/
if ( CInPacket::Decode1(this: a2) != 0 ) /*0x5fc866*/
return (*(**dword_BEDA60 + 52))(a1: *dword_BEDA60, a2: 1); /*0x5fc896*/
return CUtilDlg::Notice(sMsg: v4, sSndName: v5, ppDialog: v6, bAutoSeparated: v7, bIsTightLine: v8); /*0x5fc899*/
```

## CLogin::OnCheckPinCodeResult @ 0x5fc89d

```c
v3 = CInPacket::Decode1(this: a2); /*0x5fc8b2*/
if ( v3 != 0 ) /*0x5fc8be*/
if ( v4 != 0 ) /*0x5fc8c5*/
if ( v5 != 0 ) /*0x5fc8cc*/
if ( v6 == 0 ) /*0x5fc8d3*/
return; /*0x5fc9e8*/
if ( v7 != 0 ) /*0x5fc8da*/
if ( v7 == 3 ) /*0x5fc8df*/
CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fc8e9*/
return; /*0x5fc8fe*/
COutPacket::COutPacket(a1: 9); /*0x5fc93a*/
COutPacket::Encode1(this: &v30, a2: v10); /*0x5fc947*/
if ( v10 != 0 ) /*0x5fc94e*/
COutPacket::Encode1(this: &v30, a2: 0); /*0x5fc963*/
String = StringPool::GetString(this: Instance, result: &v35, nIdx: SP_5514_04D); /*0x5fc978*/
COutPacket::EncodeStr(a1: v27[0]); /*0x5fc9a8*/
else
CClientSocket::SendPacket(this: dword_BE7914, a2: &v30); /*0x5fc9c3*/
ZArray<unsigned char>::RemoveAll(a1: v31); /*0x5fc9cf*/
else
COutPacket::COutPacket(a1: 9); /*0x5fca18*/
COutPacket::Encode1(this: &v28, a2: v17); /*0x5fca25*/
if ( v17 != 0 ) /*0x5fca2c*/
COutPacket::Encode1(this: &v28, a2: 0); /*0x5fca41*/
v19 = StringPool::GetString(this: v18, result: &v33, nIdx: SP_5514_04D); /*0x5fca56*/
COutPacket::EncodeStr(a1: v27[0]); /*0x5fca86*/
else
CClientSocket::SendPacket(this: dword_BE7914, a2: &v28); /*0x5fcaa1*/
ZArray<unsigned char>::RemoveAll(a1: v29); /*0x5fcaad*/
else
COutPacket::COutPacket(a1: 10); /*0x5fcadb*/
if ( PinCode >= 0 ) /*0x5fcae6*/
COutPacket::Encode1(this: &v28, a2: 1); /*0x5fcb03*/
v24 = StringPool::GetString(this: v23, result: &v32, nIdx: SP_5514_04D); /*0x5fcb18*/
COutPacket::EncodeStr(a1: v27[0]); /*0x5fcb46*/
else
COutPacket::Encode1(this: &v28, a2: 0); /*0x5fcaec*/
CClientSocket::SendPacket(this: dword_BE7914, a2: &v28); /*0x5fcb61*/
ZArray<unsigned char>::RemoveAll(a1: v29); /*0x5fcb6c*/
return; /*0x5fc9e0*/
if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) != 1 ) /*0x5fcb86*/
COutPacket::COutPacket(a1: 11); /*0x5fcb8d*/
CClientSocket::SendPacket(this: dword_BE7914, a2: &v28); /*0x5fcb9f*/
ZArray<unsigned char>::RemoveAll(a1: v29); /*0x5fcbab*/
```

## CLogin::SendViewAllCharPacket @ 0x5f6d6a

```c
signed int __thiscall CLogin::SendViewAllCharPacket(CLogin *this, int a2, int a3)
v4 = *(this + 92) == 0; /*0x5f6d7f*/
if ( v4 ) /*0x5f6d88*/
if ( v7 == 0 || v5 >= *(v7 - 4) ) /*0x5f6da3*/
return 0; /*0x5f6da3*/
if ( *(i + v7) == a2 ) /*0x5f6daf*/
break; /*0x5f6daf*/
++v5; /*0x5f6db1*/
if ( v5 < 0 ) /*0x5f6db9*/
return 0; /*0x5f6db9*/
if ( v8 == 0 ) /*0x5f6dc7*/
return 0; /*0x5f6dc7*/
if ( v5 >= *(v8 - 4) ) /*0x5f6dd0*/
return 0; /*0x5f6dd0*/
if ( a3 < 0 ) /*0x5f6ddb*/
return 0; /*0x5f6ddb*/
if ( v10 == 0 || a3 >= *(v10 - 4) ) /*0x5f6dfb*/
return 0; /*0x5f6dfb*/
ZArray<ZXString<char>>::_Alloc(a1: v11, a2: &a2 + 3); /*0x5f6e17*/
if ( v12 != 0 ) /*0x5f6e24*/
else
sub_5FDC2A(a1: v13, a2: &v39); /*0x5f6e38*/
if ( v15 == 0 || v3 >= *(v15 - 4) ) /*0x5f6e4d*/
break; /*0x5f6e4d*/
*(v41 + 4 * v3++) = *(j + *(v9 + 7) + 16); /*0x5f6e6a*/
if ( v16 != 1 ) /*0x5f6e99*/
COutPacket::COutPacket(a1: 5); /*0x5f6ee5*/
COutPacket::Encode1(this: &v35, a2: v16); /*0x5f6ef7*/
if ( v16 == 1 ) /*0x5f6efe*/
COutPacket::EncodeStr(a1: v30[0]); /*0x5f6f2e*/
COutPacket::EncodeBuffer(a1: MachineId, a2: v30[0]); /*0x5f6f41*/
COutPacket::Encode4(this: &v35, a2: GameRoomClient); /*0x5f6f52*/
COutPacket::Encode1(this: &v35, a2: *(TSingleton<CWvsApp>::ms_pInstance + 36)); /*0x5f6f62*/
COutPacket::Encode1(this: &v35, a2: *v9); /*0x5f6f7b*/
COutPacket::Encode1(this: &v35, a2: a3); /*0x5f6f86*/
if ( getsockname(s: *(dword_BE7914 + 8), &name, &namelen) == 0 ) /*0x5f6fad*/
COutPacket::Encode4(this: &v35, a2: v21); /*0x5f6fb6*/
CClientSocket::SendPacket(this: dword_BE7914, a2: &v35); /*0x5f6fc5*/
ZArray<unsigned char>::RemoveAll(a1: v36); /*0x5f6ff6*/
ZArray<int>::RemoveAll(a1: &v41); /*0x5f7002*/
ZArray<ZXString<char>>::RemoveAll(a1: &v42); /*0x5f700e*/
return 1; /*0x5f7015*/
if ( v18 == 0 ) /*0x5f6ec1*/
goto LABEL_23; /*0x5f6ee0*/
if ( v18 <= 20025 ) /*0x5f7021*/
if ( v18 != 20025 ) /*0x5f7023*/
switch ( v18 ) /*0x5f7036*/
case 20000: /*0x5f7036*/
case 20021: /*0x5f7036*/
goto LABEL_50; /*0x5f703f*/
case 20002: /*0x5f7036*/
goto LABEL_43;
case 20003: /*0x5f7036*/
goto LABEL_50; /*0x5f7043*/
case 20006: /*0x5f7036*/
goto LABEL_50; /*0x5f7047*/
case 20007: /*0x5f7036*/
goto LABEL_50; /*0x5f704b*/
case 20008: /*0x5f7036*/
case 20009: /*0x5f7036*/
case 20010: /*0x5f7036*/
goto LABEL_42;
case 20015: /*0x5f7036*/
goto LABEL_44;
default:
goto LABEL_47;
goto LABEL_50; /*0x5f704f*/
if ( v18 > 20033 ) /*0x5f7056*/
if ( v18 != 20036 ) /*0x5f7090*/
if ( v18 == 20037 ) /*0x5f7097*/
goto LABEL_50; /*0x5f709f*/
goto LABEL_47; /*0x5f7097*/
else
if ( v18 >= 20032 ) /*0x5f705d*/
goto LABEL_50; /*0x5f709b*/
if ( v18 == 20026 ) /*0x5f7066*/
goto LABEL_50; /*0x5f7089*/
if ( v18 > 20028 ) /*0x5f706f*/
if ( v18 != 20030 ) /*0x5f7076*/
if ( v18 == 20031 ) /*0x5f707d*/
switch ( v23 ) /*0x5f70b7*/
case 2: /*0x5f70b7*/
break; /*0x5f70c9*/
case 4: /*0x5f70b7*/
break; /*0x5f70e1*/
case 7: /*0x5f70b7*/
CLogin::GotoTitle(this: v38, a2: 0, a3: 0); /*0x5f70eb*/
break; /*0x5f70f4*/
case 8: /*0x5f70b7*/
case 9: /*0x5f70b7*/
case 12: /*0x5f70b7*/
break; /*0x5f70c1*/
case 14: /*0x5f70b7*/
if ( CLoginUtilDlg::YesNo2(a1: 27) != 0 ) /*0x5f7113*/
v25 = *StringPool::GetString(this: v24, result: &a3, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5f712e*/
goto LABEL_60; /*0x5f7145*/
break; /*0x5f7145*/
case 15: /*0x5f70b7*/
if ( CLoginUtilDlg::YesNo2(a1: 26) != 0 ) /*0x5f7151*/
v28 = *StringPool::GetString(this: v27, result: &v37, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5f7168*/
break; /*0x5f7184*/
case 16: /*0x5f70b7*/
break; /*0x5f7189*/
case 17: /*0x5f70b7*/
break; /*0x5f718e*/
default:
break;
ZArray<int>::RemoveAll(a1: &v41); /*0x5f71c0*/
ZArray<ZXString<char>>::RemoveAll(a1: &v42); /*0x5f71cc*/
return 0; /*0x5f71cc*/
goto LABEL_47; /*0x5f707d*/
goto LABEL_51; /*0x5f70a3*/
goto LABEL_50; /*0x5f70a1*/
return 0; /*0x5f71d3*/
```

## CLogin::SendSelectCharPacket @ 0x5f726d

```c
void __thiscall CLogin::SendSelectCharPacket(CLogin *this)
if ( *(this + 92) == 0 && *(this + 126) == 0 )
if ( v2 <= 0xE && *(this + 91) == 0 )
if ( v3 != 0 ) /*0x5f72d0*/
if ( v4 != 0 )
if ( v4 == 1 )
if ( v5 != 0 ) /*0x5f73db*/
else
v7 = v6 != 0 ? (v6 + 2) : 0;
v16 = v7 != 0 ? v7 - 8 : 0;
sub_5FDA24(a1: v15); /*0x5f7408*/
if ( CDialog::DoModal(a1: v16) != 1 ) /*0x5f742f*/
sub_5FDA41(a1: v15); /*0x5f743f*/
return; /*0x5f7444*/
COutPacket::COutPacket(a1: 30); /*0x5f745e*/
COutPacket::EncodeStr(a1: v8[0]); /*0x5f747b*/
COutPacket::Encode4(this: &v11, a2: v19); /*0x5f7486*/
COutPacket::EncodeStr(a1: v8[0]); /*0x5f74c1*/
COutPacket::EncodeStr(a1: v8[0]); /*0x5f74f7*/
CClientSocket::SendPacket(this: dword_BE7914, a2: &v11); /*0x5f7506*/
ZArray<unsigned char>::RemoveAll(a1: v12); /*0x5f7535*/
sub_5FDA41(a1: v15); /*0x5f754d*/
else if ( v4 <= 3u ) /*0x5f72f3*/
COutPacket::COutPacket(a1: 19); /*0x5f72fe*/
COutPacket::Encode4(this: &v9, a2: v19); /*0x5f7310*/
COutPacket::EncodeStr(a1: v8[0]); /*0x5f734b*/
COutPacket::EncodeStr(a1: v8[0]); /*0x5f7381*/
CClientSocket::SendPacket(this: dword_BE7914, a2: &v9); /*0x5f7390*/
ZArray<unsigned char>::RemoveAll(a1: v10); /*0x5f73b4*/
else
CLoginUtilDlg::Notice(a1: 95, a2: this + 500); /*0x5f7560*/
if ( CSoftKeyboardDlg::InitializeSecondaryPassword(a1: &v18, a2: this + 500) == 0 ) /*0x5f757a*/
return; /*0x5f7588*/
COutPacket::COutPacket(a1: 29); /*0x5f7592*/
COutPacket::Encode1(this: &v13, a2: 1); /*0x5f75a0*/
COutPacket::Encode4(this: &v13, a2: v19); /*0x5f75ab*/
COutPacket::EncodeStr(a1: v8[0]); /*0x5f75e6*/
COutPacket::EncodeStr(a1: v8[0]); /*0x5f761c*/
COutPacket::EncodeStr(a1: v8[0]); /*0x5f7635*/
CClientSocket::SendPacket(this: dword_BE7914, a2: &v13); /*0x5f7644*/
ZArray<unsigned char>::RemoveAll(a1: v14); /*0x5f7667*/
*(this + 92) = 1; /*0x5f7678*/
```

## CLogin::SendDeleteCharPacket @ 0x5f7c4a

```c
unsigned int __thiscall CLogin::SendDeleteCharPacket(CLogin *this)
if ( *(this + 92) == 0 )
if ( result <= 0xE )
if ( *(result + *(this + 101)) != 0 )
if ( *(this + 372) == 1 )
if ( v3 != 0 ) /*0x5f7cd4*/
else
v5 = v4 != 0 ? (v4 + 2) : 0;
v13 = v5 != 0 ? v5 - 8 : 0;
sub_5FDA24(a1: v12); /*0x5f7d03*/
if ( CDialog::DoModal(a1: v13) == 1 )
if ( v7 == 1 || v7 == 2 )
v8 = *(*(this + 103) + 4 * *(this + 100)) != 0 ? 58 : 55;
else
v8 = *(*(this + 103) + 4 * *(this + 100)) != 0 ? 57 : 13;
if ( CLoginUtilDlg::YesNo(a1: v8, a2: this + 500) != 0 ) /*0x5f7dbd*/
COutPacket::COutPacket(a1: 23); /*0x5f7dc8*/
COutPacket::EncodeStr(a1: v9); /*0x5f7de7*/
COutPacket::Encode4(this: &v10, a2: *(684 * *(this + 100) + *(this + 101))); /*0x5f7e04*/
if ( dword_BEDA4C != 0 ) /*0x5f7e1a*/
*(this + 100) = -1; /*0x5f7e1c*/
ZArray<unsigned char>::RemoveAll(a1: v11); /*0x5f7e4f*/
else
return sub_5FDA41(a1: v12); /*0x5f7e66*/
else
return CLoginUtilDlg::Notice(a1: 94, a2: this + 500); /*0x5f7cb1*/
return result; /*0x5f7e6b*/
```

## CLogin::SendCheckDuplicateIDPacket @ 0x5f7fe7

```c
void *__thiscall CLogin::SendCheckDuplicateIDPacket(CLogin *this, int *a2)
if ( *(this + 92) == 0 ) /*0x5f7ffe*/
if ( is_valid_character_name(sCharName: a2, bWordCheck: v4 == 0) ) /*0x5f8014*/
COutPacket::COutPacket(a1: 21); /*0x5f8030*/
COutPacket::EncodeStr(a1: v5); /*0x5f804d*/
return ZArray<unsigned char>::RemoveAll(a1: v7); /*0x5f8064*/
else
return CLoginUtilDlg::Error(a1: 10, a2: 0); /*0x5f8022*/
return result; /*0x5f8069*/
```

## CLogin::GotoTitle @ 0x5fc142

```c
void __thiscall CLogin::GotoTitle(CLogin *this, int a2, struct IUIMsgHandler *a3)
if ( *(this + 90) > *(this + 96) && *(this + 91) == 0 ) /*0x5fc16a*/
if ( a2 != 0 ) /*0x5fc173*/
else
if ( v4 != 0 ) /*0x5fc18c*/
if ( dword_BF14EC == 0 ) /*0x5fc198*/
if ( *dword_BEDA60 != 0 || *(this + 96) != 0 ) /*0x5fc1c8*/
if ( *(this + 96) == 1 || (v7 = *(this + 99)) != 0 && *(v7 - 4) != 0 ) /*0x5fc207*/
sub_5FDDE3(a1: this + 396); /*0x5fc20f*/
else
if ( v6 != 0 ) /*0x5fc1e3*/
*(this + 372) = -1; /*0x5fc21a*/
CLogin::ChangeStep(this, a2: v8); /*0x5fc223*/
else if ( a3 != 0 && TSingleton<CWndMan>::ms_pInstance != 0 ) /*0x5fc237*/
CWndMan::SetFocus(a1: a3); /*0x5fc23c*/
```

## CLogin::GotoWorldSelect @ 0x5fc36c

```c
void __thiscall CLogin::GotoWorldSelect(CLogin *this, int a2)
if ( *(this + 90) > 1 ) /*0x5fc385*/
if ( dword_BF14EC == 0 ) /*0x5fc390*/
sub_5FDDE3(a1: this + 396); /*0x5fc3ad*/
if ( a2 == 0 ) /*0x5fc3b5*/
COutPacket::COutPacket(a1: 12); /*0x5fc3bc*/
CClientSocket::SendPacket(this: dword_BE7914, a2: v3); /*0x5fc3ce*/
ZArray<unsigned char>::RemoveAll(a1: v4); /*0x5fc3da*/
*(this + 96) = 1; /*0x5fc3e9*/
CLogin::ChangeStep(this, a2: 1); /*0x5fc3ef*/
*(this + 96) = 0; /*0x5fc3f4*/
```

## CLogin::OnNewCharStep @ 0x5f6425

```c
if ( *(32 * *(*dword_BE7918 + 8276) + *(this + 99) + 24) != 0 ) /*0x5f6440*/
CLoginUtilDlg::Notice(a1: 36, a2: 0); /*0x5f6445*/
else if ( *(684 * *(this + 95) + *(this + 101) - 684) != 0 ) /*0x5f6465*/
else
CLogin::ChangeStep(this, a2: 0xFFFFFFFF); /*0x5f647b*/
```

## CLogin::ViewRecommendWorld @ 0x5fd364

```c
if ( a2 != 0 )
if ( *(this + 131) != 0 ) /*0x5fd37f*/
if ( result != 0 && *(result - 1) != 0 ) /*0x5fd396*/
if ( v5 != 0 ) /*0x5fd3ba*/
*(this + 77) = 1; /*0x5fd3d2*/
else
if ( dword_BEDA68 != 0 ) /*0x5fd3e6*/
CWnd::Destroy(a1: dword_BEDA68); /*0x5fd3e8*/
if ( dword_BEDA68 != 0 ) /*0x5fd3f4*/
if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0
|| (v6 = TSingleton<CUIWorldSelect>::ms_pInstance, TSingleton<CUIWorldSelect>::ms_pInstance != 0) )
CWndMan::SetFocus(a1: v6 != 0 ? v6 + 4 : 0);
else
CWndMan::SetFocus(a1: this != 0 ? this + 4 : 0);
*(this + 77) = 0; /*0x5fd443*/
return result; /*0x5fd449*/
```

## CUIWorldSelect::OnButtonClicked @ 0x624ae8

```c
if ( CUIWorldSelect::IsRequestValid(this) != 0 )
if ( (a2 - 1000) >= 0 && v3 < *(this + 33) )
*(this + 34) = v3; /*0x624b0e*/
*(this + 99) = v3; /*0x624b29*/
CLogin::SendCheckUserLimitPacket(this: v4, a2: v3); /*0x624b2f*/
if ( TSingleton<CWndMan>::ms_pInstance != 0 )
if ( dword_BEDA68 != 0
|| (v5 = TSingleton<CUIChannelSelect>::ms_pInstance, TSingleton<CUIChannelSelect>::ms_pInstance != 0) )
CWndMan::SetFocus(a1: v5 != 0 ? v5 + 4 : 0);
```

## CUIWorldSelect::EnableButtons @ 0x62536c

```c
if ( *(this + 33) > 0 ) /*0x625378*/
++v3; /*0x62538d*/
if ( a2 >= 0 ) /*0x6253a0*/
return (*(*(*(this + 2 * a2 + 36) + 4) + 28))(a1: *(this + 2 * a2 + 36) + 4, a2: 0); /*0x6253b1*/
return result; /*0x6253b4*/
```

## CUIWorldSelect::SetFocusWorld @ 0x6272d0

```c
void __thiscall CUIWorldSelect::SetFocusWorld(CUIWorldSelect *this, int nWorldID)
if ( v2 > 0 ) /*0x6272dc*/
if ( *v4 == nWorldID ) /*0x6272f0*/
break; /*0x6272f0*/
++v3; /*0x6272f2*/
if ( v3 != v2 ) /*0x6272fe*/
*(this + 34) = v3; /*0x627301*/
```

## CUIWorldSelect::MakeAdvice @ 0x6281b9

```c
if ( v26 != 0 ) /*0x6281f2*/
v4 = *v3 == 0; /*0x628205*/
if ( v4 ) /*0x62820a*/
if ( v5 < 0 ) /*0x628235*/
if ( v26 != 0 ) /*0x6282ad*/
if ( v9 < 0 ) /*0x6282c5*/
if ( v10 < 0 ) /*0x6282dd*/
if ( v11 < 0 ) /*0x6282ff*/
if ( v12 < 0 ) /*0x62831f*/
v4 = *v3 == 0; /*0x628327*/
if ( v4 ) /*0x62832e*/
if ( v14 < 0 ) /*0x628363*/
if ( v15 < 0 ) /*0x628381*/
if ( v16 < 0 ) /*0x628399*/
if ( v27 != 0 ) /*0x6283aa*/
return (*(*v27 + 8))(a1: v27); /*0x6283af*/
return result; /*0x6283b2*/
```

## CUIWorldSelect::UserLimitResult @ 0x6283c1

```c
if ( a2 == 0 ) /*0x6283d9*/
if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x6284b3*/
return sub_6297A0(a1: *(this + 99), a2: a3, a3: 1); /*0x6284b3*/
if ( result == 0 ) /*0x6284e6*/
return result; /*0x6284e6*/
return sub_628501(a1: v9, a2: v8, a3: v10); /*0x6284e8*/
if ( a2 == 1 ) /*0x6283e0*/
nIdx: SP_1369_SINCE_THERE_ARE_MANY_R_NCONCURRENT_USERS_IN_THIS_WORLD_R_NYOU_MAY_ENCOUNTER_SOME);
if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x628458*/
return sub_6297A0(a1: *(this + 99), a2: a3, a3: 1); /*0x62846a*/
if ( result == 0 ) /*0x6284a4*/
return result; /*0x6284a4*/
return sub_628501(a1: v9, a2: v8, a3: v10); /*0x6284ec*/
if ( a2 == 2 ) /*0x6283e3*/
nIdx: SP_1370_THE_CONCURRENT_USERS_IN_THIS_WORLD_R_NHAVE_REACHED_THE_MAX_R_NPLEASE_TRY_AGAIN_L);
return (*(*(v6 + 4) + 28))(a1: v6 + 4, a2: 1); /*0x628423*/
return result; /*0x6284f1*/
```

## CUIWorldSelect::DrawWorldItems @ 0x624e13

```c
*(this + 97) = v2; /*0x624e38*/
if ( v2 != 0 ) /*0x624e3e*/
if ( v2 > 0 ) /*0x624e48*/
sub_5FDF26(a1: &v60); /*0x624ec8*/
sub_5FDF26(a1: &v61); /*0x624ed4*/
sub_5FDF26(a1: &v62); /*0x624ee0*/
++v3; /*0x624ee9*/
if ( v8 != 0 ) /*0x624f05*/
else
*(this + 33) = v9; /*0x624f10*/
v42 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x624f45*/
StringPool::GetBSTR(this: Instance, a2: &v38, a3: SP_3184_UI_LOGINIMG_WORLDNOTICE); /*0x624f66*/
if ( Unknown != 0 ) /*0x624f9c*/
if ( v14 < 0 && v14 != -2147467262 ) /*0x624fb3*/
if ( v15 < 0 ) /*0x624fce*/
if ( v16 < 0 ) /*0x624fe6*/
if ( v17 < 0 ) /*0x624ffe*/
if ( v57 >= *(this + 33) ) /*0x625021*/
v42 = SP_1357_UI_LOGINIMG_WORLDSELECT_BTWORLD_E; /*0x6252bc*/
if ( v35 != 0 ) /*0x625303*/
else
else
v42 = SP_1355_UI_LOGINIMG_WORLDSELECT_BTWORLD_D; /*0x625039*/
if ( v22 != 0 ) /*0x625084*/
else
if ( *(v19 + 8) != 0 ) /*0x6250bf*/
v24 = *(this + 95) == 0; /*0x6250d4*/
if ( v24 ) /*0x6250df*/
if ( v25 < 0 ) /*0x625114*/
if ( v28 != 0 ) /*0x62516d*/
if ( v29 < 0 && v29 != -2147467262 ) /*0x625184*/
if ( v30 < 0 ) /*0x62519f*/
if ( v62 != 0 ) /*0x6251ab*/
if ( v61 == 0 ) /*0x6251c5*/
if ( v32 < 0 ) /*0x6251fa*/
if ( v33 < 0 ) /*0x62521a*/
if ( v62 != 0 ) /*0x62522e*/
if ( v61 != 0 ) /*0x62523f*/
++v57; /*0x62524d*/
if ( *dword_BE7918 != 0 && *(*dword_BE7918 + 8328) == 0 ) /*0x625276*/
if ( v56 != 0 ) /*0x625290*/
```

## CUITitle::OnButtonClicked @ 0x620ca5

```c
if ( a2 == 1000 ) /*0x620cbd*/
if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620e07*/
return; /*0x620e07*/
goto LABEL_22; /*0x620e0f*/
if ( a2 != 1001 ) /*0x620cc4*/
switch ( a2 ) /*0x620ccb*/
case 0x3EAu: /*0x620ccb*/
if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620db8*/
return; /*0x620db8*/
nIdx: SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID);
break;
case 0x3EBu: /*0x620ccb*/
if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620d7b*/
return; /*0x620d7b*/
nIdx: SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD);
break;
case 0x3ECu: /*0x620ccb*/
if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620d3e*/
return; /*0x620d3e*/
nIdx: SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK);
break;
case 0x3EDu: /*0x620ccb*/
if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620cfb*/
return; /*0x620cfb*/
v9 = *StringPool::GetString(this: v3, result: &v15, nIdx: SP_5633_HTTP__MAPLESTORYNEXONNET); /*0x620d1a*/
break;
case 0x3EEu: /*0x620ccb*/
return; /*0x620ced*/
default:
if ( a2 == 1 ) /*0x620e16*/
else if ( a2 == 2 ) /*0x620e22*/
return; /*0x620e1c*/
return; /*0x620de8*/
if ( CUITitle::IsRequestValid(this) != 0 ) /*0x620df3*/
```

## CUICharSelect::OnButtonClicked @ 0x601667

```c
if ( CUICharSelect::IsRequestValid(this) != 0 )
*(this + 29) = (a2 - 1000); /*0x60168b*/
switch ( a2 ) /*0x60168e*/
case 0x3E8u: /*0x60168e*/
CLogin::SendSelectCharPacket(this: *(this + 28)); /*0x6016ad*/
break;
case 0x3E9u: /*0x60168e*/
break;
case 0x3EAu: /*0x60168e*/
CLogin::SendDeleteCharPacket(this: *(this + 28)); /*0x601699*/
break;
default:
break; /*0x60168e*/
if ( TSingleton<CWndMan>::ms_pInstance != 0 )
CWndMan::SetFocus(a1: this != 0 ? this + 1 : 0);
```

## CUIRecommendWorld::OnButtonClicked @ 0x62b708

```c
if ( CUIRecommendWorld::IsRequestValid(a1: this) != 0 ) /*0x62b712*/
switch ( nId ) /*0x62b71d*/
case 1000: /*0x62b71d*/
break;
case 1001: /*0x62b71d*/
break;
case 1002: /*0x62b71d*/
break;
default:
break; /*0x62b71d*/
```

## CUIRecommendWorld::SelectWorld @ 0x62b96b

```c
if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x62b976*/
CUIWorldSelect::SetFocusWorld( /*0x62b98d*/
return (*(*this + 52))(a1: this, a2: 1); /*0x62b99b*/
```

## CUIRecommendWorld::MoveWorld @ 0x62b99d

```c
if ( v2 != 0 ) /*0x62b9ae*/
else
*(this + 33) = (nMovement + v3 + *(this + 33)) % v3; /*0x62b9ce*/
```

## CPinCodeDlg::OnButtonClicked @ 0x61e2db

```c
if ( a2 == 1000 ) /*0x61e2f0*/
if ( *(this + 55) == 1003 ) /*0x61e357*/
return; /*0x61e385*/
goto LABEL_9; /*0x61e357*/
if ( a2 == 1001 ) /*0x61e2f3*/
if ( *(this + 55) != 1 || CPinCodeDlg::YesNo(a1: 7, a2: 0) != 0 ) /*0x61e31d*/
return; /*0x61e34b*/
return; /*0x61e335*/
if ( a2 == 1002 && *(this + 55) == 3 ) /*0x61e2ff*/
```

## CUIChannelSelect::OnKey @ 0x628a1c

```c
if ( lParam >= 0 ) /*0x628a2b*/
if ( wParam == 9 ) /*0x628a38*/
if ( dword_BEC33C == 0 /*0x628bb5*/
|| CInputSystem::IsKeyPressed(this: dword_BEC33C, a2: 16) == 0
&& CInputSystem::IsKeyPressed(this: dword_BEC33C, a2: 161) == 0 )
if ( v16 == 0 || v6 >= *(v16 - 4) ) /*0x628bda*/
goto LABEL_58; /*0x628bdc*/
if ( (v6 & 0x80000000) != 0 ) /*0x628bb8*/
goto LABEL_43; /*0x628bc5*/
return; /*0x628be2*/
if ( wParam != 13 ) /*0x628a43*/
if ( wParam == 27 ) /*0x628a4c*/
return; /*0x628b7b*/
if ( wParam != 37 ) /*0x628a55*/
switch ( wParam ) /*0x628a5c*/
if ( v4 < 0 ) /*0x628aed*/
if ( v4 >= 4 ) /*0x628af3*/
else
if ( v9 != 0 ) /*0x628b02*/
if ( v11 == 0 || v6 >= *(v11 - 4) ) /*0x628b17*/
if ( v12 != 0 ) /*0x628b22*/
break;
if ( v7 != 0 ) /*0x628abe*/
if ( v4 == v7 - 1 ) /*0x628ac9*/
else if ( v4 % 4 == 3 ) /*0x628adc*/
else
break;
if ( v4 < 0 ) /*0x628a7e*/
if ( v5 != 0 ) /*0x628a8f*/
if ( (v5 - v4) > 4 ) /*0x628a98*/
else
break;
default:
return; /*0x628a77*/
goto LABEL_58; /*0x628aa5*/
if ( v4 < 0 ) /*0x628b3c*/
if ( v4 % 4 != 0 ) /*0x628b47*/
else
if ( v14 == 0 || v6 >= *(v14 - 4) ) /*0x628b5f*/
if ( v15 != 0 ) /*0x628b66*/
goto LABEL_58; /*0x628b6e*/
goto LABEL_58; /*0x628b5f*/
if ( v4 < 0 ) /*0x628b7f*/
CUILoginStart::SendViewAllCharacterPacket(this: this - 1, a2: v4); /*0x628b87*/
```

## CUIAvatar::DrawNameTag @ 0x605b52

```c
v4 = *(this + a2 + 49) == 0; /*0x605b74*/
if ( v4 ) /*0x605b85*/
if ( v6 < 0 ) /*0x605bb0*/
if ( *v5 == 0 ) /*0x605bba*/
v4 = *v5 == 0; /*0x605bcd*/
if ( v4 ) /*0x605bd2*/
if ( v11 < 0 ) /*0x605c42*/
if ( v43 == 0 ) /*0x605c4f*/
if ( v14 < 0 ) /*0x605c78*/
if ( v13 - 10 > 0 ) /*0x605c90*/
v40 = (this + 12 * (a3 != 0) + 280); /*0x605ca8*/
if ( v43 == 0 ) /*0x605ccd*/
if ( v16 < 0 ) /*0x605cfe*/
if ( v17 < 0 ) /*0x605d1e*/
v45 = a3 != 0; /*0x605d54*/
if ( v43 == 0 ) /*0x605d66*/
if ( v19 < 0 ) /*0x605d96*/
if ( v20 < 0 ) /*0x605db6*/
if ( v43 == 0 ) /*0x605de4*/
if ( v22 < 0 ) /*0x605e19*/
if ( v23 < 0 ) /*0x605e39*/
if ( v27 < 0 ) /*0x605ec7*/
if ( v28 < 0 ) /*0x605edf*/
if ( v43 != 0 ) /*0x605ef0*/
return (*(*v43 + 8))(a1: v43); /*0x605ef5*/
return result; /*0x605ef8*/
```

## CUIAvatarVAC::OnCreate @ 0x608ca6

```c
v80 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x608cca*/
StringPool::GetBSTR(this: Instance, a2: &v96, a3: SP_3987_UI_LOGINIMG_VIEWALLCHAR_BACKGRND); /*0x608cdf*/
v80 = SP_3983_UI_LOGINIMG_VIEWALLCHAR_BTENTER; /*0x608d39*/
if ( v8 != 0 ) /*0x608d85*/
else
if ( v10 != 0 ) /*0x608dce*/
else
if ( v12 <= 9 ) /*0x608e26*/
else
StringPool::GetBSTR(this: v15, a2: &v73, a3: SP_1323_UI_LOGINIMG_CHARSELECT_NAMETAG); /*0x608e7c*/
if ( Unknown != 0 ) /*0x608eb2*/
if ( v18 < 0 && v18 != -2147467262 ) /*0x608ec9*/
if ( v19 < 0 ) /*0x608ee4*/
if ( v20 < 0 ) /*0x608efc*/
if ( v21 < 0 ) /*0x608f14*/
if ( v90 != 0 ) /*0x608f73*/
if ( v24 < 0 && v24 != -2147467262 ) /*0x608f8a*/
if ( v25 < 0 ) /*0x608fa2*/
for ( i = 0; i < 3; ++i ) /*0x608fad*/
v65 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x608fbd*/
if ( v29 < 0 && v29 != -2147467262 ) /*0x60900b*/
if ( v30 < 0 ) /*0x609023*/
if ( v95 != 0 ) /*0x609042*/
++*v94; /*0x60904a*/
v65 = SP_3986_UI_LOGINIMG_VIEWALLCHAR_WORLDICONS_D; /*0x609075*/
if ( v33 < 0 ) /*0x6090c5*/
if ( v34 < 0 ) /*0x6090e9*/
v65 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x6090f1*/
if ( v39 < 0 && v39 != -2147467262 ) /*0x609154*/
if ( v40 < 0 ) /*0x60916f*/
if ( v41 < 0 ) /*0x609187*/
if ( v42 < 0 ) /*0x60919f*/
++*v94; /*0x6091b3*/
v65 = SP_1410_CANVASFONT; /*0x6091c7*/
if ( v45 < 0 ) /*0x609214*/
v61 = SP_5527_ARIAL; /*0x60922d*/
if ( v48 < 0 ) /*0x609269*/
v65 = SP_1410_CANVASFONT; /*0x609274*/
if ( v51 < 0 ) /*0x6092c1*/
v61 = SP_5527_ARIAL; /*0x6092d7*/
if ( v54 < 0 ) /*0x609313*/
if ( *(*(this + 27) + 300) > 0 ) /*0x609326*/
++v55; /*0x609333*/
if ( *(*(this + 27) + 300) > 0 ) /*0x609353*/
++v57; /*0x60936c*/
++v58; /*0x60936d*/
if ( !v60 ) /*0x609384*/
if ( *a2 == 0 ) /*0x609391*/
break; /*0x609391*/
CUIAvatarVAC::MakeWorldIcon(this, a2: v96++); /*0x6093a3*/
if ( v91 != 0 ) /*0x6093d4*/
```

## CUILoginStart::EnableLoginStartCtrl @ 0x60eb04

```c
if ( v3 != 0 ) /*0x60eb14*/
if ( result != 0 ) /*0x60eb2c*/
if ( v5 != 0 ) /*0x60eb44*/
return (*(*(v5 + 4) + 28))(a1: v5 + 4, a2: nEnable & 0x10); /*0x60eb50*/
return result; /*0x60eb53*/
```

## CLoginUtilDlg::DoModal @ 0x60f70b

```c
return v2; /*0x60f74a*/
```

## CLogo::Init @ 0x62edda

```c
if ( v2 == 1 ) /*0x62ee0f*/
else
if ( v5 != 0 && *v5 != 0 && *(TSingleton<CWvsApp>::ms_pInstance + 40) != 0 ) /*0x62ee67*/
```
