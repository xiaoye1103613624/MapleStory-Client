# r398–r409 连续波 checkpoint

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`

| 轮次 | 要点 |
|------|------|
| r398 | ContiMove/State + HourChanged → SM |
| r399 | UIClock HUD；ObjectState/KoreanEvent SM |
| r400 | UIMTS + MapSummon + Snowball/Coconut |
| r401 | MapMist；Dojo/Catch/Pet/Mount/Pendant SM |
| r402 | FakeGmNotice/BackEffect/NameChange SM |
| r403 | Ranch/Pyramid/KnockBack + ShowItemGain effects |
| r404 | OpenUI 0x21/22/25；Portal/Rock/Gender/LockUI |
| r405 | Authkey/Magnet/Ariant；StoreBank on shop box |
| r406 | Weather ItemEff 校准；Guild/Account SM |
| r407 | Grenade spawn_summon；Obstacle/Merchant SM |
| r408 | MTS 0x15 逐条 itemId/price；FamilyJoin |
| r409 | SetITC→UIMTS；PyramidScore/Chair SM |

阻塞：登录叠图缺 `current_shots`；PersonalShop CREATE 待 FM 实机。
