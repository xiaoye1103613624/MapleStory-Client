# G11 资源取证摘要（2026-11-05）

## Catch

```
Effect/BasicEff.img/Catch
  Fail/0..4     bitmap ~33–37×95–106
  Success/0..7  bitmap ~68–119×64–113
```

V17 解密串：`Catch/Success` @ `0xb1400c`，`Catch/Fail` @ `0xb14000`。

## Chalkboard

```
Item/Cash/0537.img/05370000/info
  sample  bitmap 80×68
  icon    bitmap 29×29
```

## CharBox / 店招

```
Item/Cash/0503.img/05030000/skin/signboard  bitmap 166×166
UI/UIWindow.img/PersonalShop/select         bitmap 162×35（回退）
```

## ItemFX

`Effect/ItemEff.img/<itemid>/effect/0`（既有 `try_load_anim`）；本轮仅改 Kind 与去英文名牌。
