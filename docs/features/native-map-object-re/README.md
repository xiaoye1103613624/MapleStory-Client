# BeiDou.exe V17.7 — Native map-object RE dossier

Target: `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe` (imagebase `0x400000`, v83-lineage, MSVC C++,
COM/`VARIANT`-based resource layer via `NameSpace.dll`/`PCOM.dll`/`ResMan.dll`).
Analysis was **read-only** — no renames, comments or type edits were written to the IDB.

---

## 0. FOUNDATION FINDING — the exe's string pool is obfuscated (read this first)

Everything in §1–§7 depends on this, so it is stated up front.

### 0.1 The problem

A plain string scan of `BeiDou.exe` finds **no** animation names (`stand`, `attack`, `die1`,
`hit1`, `walk`, `ladder`, `swim`, `revive`, `respawn`, `mist`, `door`, `summon`, `reactor`)
anywhere in `.text`/`.rdata`/`.data` — verified for both ASCII and UTF‑16, case-insensitively,
and also as 4-byte ASCII immediates inside code. `attack` appears 0 times; `stand` appears only
4 times, all inside `.rsrc` font metadata ("standard 07_55"). So a naive "grep the exe for
`attack`" approach yields nothing, and the naive conclusion "the client has no action names"
would be **wrong**.

### 0.2 The mechanism (OBSERVED)

Strings are not literals. They are fetched **by numeric ID at runtime**:

```c
// sub_79E805: returns the per-thread resource context (InterlockedExchange spinlock + singleton)
v0 = *(void **)sub_406276(a1: (int)&v41, a2: 5640);   // "string id 5640"
sub_402BE8(Src: v0);                                   // register into vector unk_BEC4C8
```

`sub_406276(int *out, int id)` → `sub_79EA63(ctx, out, id, 0)` →
`sub_79E993(&slot, id, 0)`, and `sub_79E993` is the resolver:

```c
// sub_79E993 (excerpt, 0x79E993)
v12 = *(char *)*(&off_BDC9D4 + a3);      // SIGNED byte at table[id] == key rotation
v7  = sub_403065(dword_BF0B00, 4u);      // alloc 4-byte cache cell
*v7 = 0;
sub_414617((const char *)(v8 + 1), -1);  // copy bytes AFTER the length byte
sub_79ECDE(v13, &unk_B001EC, 16, v12);   // decrypt with 16-byte key @0xB001EC, rotated by v12
*(_DWORD *)(v6 + *this) = v13;           // memoise into slot array
```

So the layout is:

| Item | Address | Meaning |
| --- | --- | --- |
| Pointer table | `0xBDC9D4` … `0xBE2240` | **5659 entries**, 4 bytes each, **descending** pointers |
| String blob | `0xB22030` … `0xB372FC` (~85 KB) | `[len byte][XOR-obfuscated payload][00]` |
| Key | `0xB001EC` (16 bytes) | `d6 de 75 86 46 64 a3 71 e8 e6 7b d3 33 30 e7 2e` |

Cipher (OBSERVED, from `sub_79ECDE` → `sub_79E780` → `sub_79EBF3`):
a 16-byte key is *rotated* by `(len >> 3) % 16` and *bit-shifted left* by `len & 7`,
then the payload is `^`-ed with `key[i mod 16]`. `len` is the entry's own length byte,
interpreted as a **signed char**.

### 0.3 The decoder — **fully validated**

Reimplemented standalone at
[`tools/beidou_stringpool.py`](../../beidou_stringpool.py)
(mirrors `sub_79EBF3` exactly, including the signed-char rotation).
It recovers all 5659 strings; the complete plaintext table is archived at
[`evidence/beidou_v177_decrypted_strings.tsv`](evidence/beidou_v177_decrypted_strings.tsv).

Validation samples (ID → recovered text):

```text
1035 stand       1036 stand0      1053 hit1       1057 attack2    1062 attack
1065 die         1066 die1        1068 dieF       2735 skill1     2751 skillF
1495 Map/Obj/%s.img/%s/%s/%s      2972 force      3073 Sound/Reactor.img/%s/%s
3076 Reactor/%s.img               3292 UI/UIWindow.img/MobGage/Mob/%d
5112 reactorState                 5357 Sound/Radio.img/%s
109  MapleStory Error : Loading mobskill failed.
101  MapleStory Error : Loading tamingmob failed.
```

A cross-check proves the mechanism rather than assuming it: `sub_9F7159` holds a *visible*
15-entry literal array (`Character/Mob/Skill/Reactor/Npc/UI/Quest/Item/Effect/String/Etc/
Morph/TamingMob/Sound/Map`) and formats `"%s.wz"` per entry — the plain-literal half of the
same resource layer that the pool feeds.

### 0.4 Immediate consequence for the reconstruction

The native client is **name-driven, not number-driven**: every WZ node lookup, animation
lookup, sound lookup and property read goes through a string that happens to be stored
obfuscated. Nothing about the *logic* depends on the obfuscation. A C++ reconstruction can
keep ordinary string literals; it does **not** need to reproduce the pool. What matters is the
name sets and the paths enumerated below.

---

## 1. MOB activity / stance state machine (non-controlled mobs)

### 1.1 The activity-name table — OBSERVED

`sub_4A6C05` (0x4A6C05, size 0x905) builds a **40-entry vector at `unk_BEC4C8`** by pushing
pool strings in this exact order:

```text
idx  name      idx  name       idx  name        idx  name
  0  move        10 die2         20 attackF      30 skill10
  1  stand       11 dieF         21 skill1       31 skill11
  2  jump        12 att          22 skill2       32 skill12
  3  fly         13 attack2      23 skill3       33 skill13
  4  regena      14 attack3      24 skill4       34 skill14
  5  bomb        15 attack4      25 skill5       35 skill15
  6  hit1        16 attack5      26 skill6       36 skill16
  7  hit2        17 attack6      27 skill7       37 skillF
  8  hitF        18 attack       28 skill8       38 chase
  9  die1        19 attack8      29 skill9       39 miss
```

Note the deliberate irregularity: `att`(12) is the *base* attack name and `attack`(18) sits
between `attack6` and `attack8` — there is **no** `attack7` slot and **no** `attack1` slot.
Ditto `skill1…skill16` + `skillF`, and a bare `die`/`die1`/`die2`/`dieF`.

Pseudocode excerpt (head of the builder):

```c
// sub_4A6C05 @0x4A6C05
mov eax, offset loc_A824C4        ; SEH
call sub_79E805
push 1608h                        ; pool id 5640 = "move"
call sub_406276
mov ecx, offset unk_BEC4C8
call sub_402BE8                   ; vector push
; ... repeated 40 times: 1035 stand, 5635 jump, 1040 fly, 5649 regena,
;     1069 bomb, 1053 hit1, 1054 hit2, 1055 hitF, 1066 die1, 1067 die2,
;     1068 dieF, 1056 att, 1057 attack2 ... 2735 skill1 ... 2751 skillF,
;     3412 chase, 5348 miss
```

### 1.2 The two other name vectors — OBSERVED

`sub_4A61AD` (0x4A61AD) builds a **46-entry vector at `byte_BEC568`** — this is the
*player/other* animation-name set:

```text
walk, stand, jump, alert, alert2, alert3, alert4, fly, prone, ladder, rope,
dead, sit, swingO1, swingO2, swingO3, swingT1, swingT3, shoot1, shootF,
proneStab, stabO1, stabO2, stabTF, timeleap, eburster, edrain,
ladder2, rope2, stormbreak, doublefire, somersault, straight, screw,
doubleupper, back, eorb, fist, dragonstrike, recovery, wave, shockwave,
demolition, snatch, windspear, windshot
```

`sub_4A9630` (0x4A9630) builds a **15-entry vector at `unk_BEC3BC`** — the *summon/pet* set:

```text
stand, move, fly, summoned, att, attack2, skill1, skill2, skill3,
skill4, skill5, skill6, hit, die, say
```

A separate 4-entry vector is built by `sub_4A7510` (0x4A7510) at `unk_BEC4B8..unk_BEC4C4`:

```c
sub_406292(a1: &unk_BEC4B8, a2: 1152);   // "beast"
sub_406292(a1: &unk_BEC4BC, a2: 1153);   // "dragon"
sub_406292(a1: &unk_BEC4C0, a2: 5657);   // "unde"
sub_406292(a1: &unk_BEC4C4, a2: 1154);   // "etc"
```

These are the four mount/TamingMob animation families (`beast`/`dragon`/`etc`/`unde` =
undead/other) used for the mount `skill`/`stand` families.

### 1.3 Name → index lookup exists, but it targets the **4-entry** array — OBSERVED (corrected)

`sub_4A8D5D` (0x4A8D5D) is a **linear string search** over a name vector:

```c
int __cdecl sub_4A8D5D(int a1)              // a1 = pointer to the incoming name string
{
  v1 = 0;
  v2 = (char *)&unk_BEC4B8;                 // start = the 4-entry mount-family array (§1.2)
  while ( (unsigned __int8)sub_402CCC(&a1) == 0 )   // sub_402CCC == string equality test
  {
    v2 += 4;
    ++v1;
    if ( (int)v2 >= (int)byte_BEC4C8 )      // bound = end of the 4-entry array
      return 3;                             // not-found sentinel
  }
  return v1;                                // matched index
}
```

**Scope note — read carefully.** The loop bound is `byte_BEC4C8`, i.e. this searches **only
`unk_BEC4B8..unk_BEC4C4`** (`beast`/`dragon`/`unde`/`etc`), the 4-entry *mount family* array
of §1.2 — **not** the 40-entry mob activity vector (which *starts* at `BEC4C8`). So the
verified OBSERVED rule is: **mount/TamingMob animation family is selected by comparing a name
against `{beast, dragon, unde, etc}` and returning that index (3 = default/not-found).**

`sub_402CCC` (0x402CCC) confirms it is a compare:

```c
bool __thiscall sub_402CCC(int *this, _DWORD *a2)
{
  v2 = *this;
  if (v2 == *a2) v3 = 0;
  else if (v2)   v3 = (*a2 ? sub_402ED6(*a2) : 1);   // else strcmp against candidate
  else           v3 = -1;
  return v3 == 0;
}
```

**INFERRED (not proven):** that the 40-entry activity vector is indexed the same way
(name → index) for mobs. The two direct consumers I could attribute are
`sub_4A74F2` (the 40-vector's `eh vector destructor iterator`, 40 entries × 4 bytes at
`unk_BEC4C8`) and `sub_4A6BE7` (same, 46 entries × 4 bytes at `byte_BEC568`) — those are
*destructors*, not lookups. See §1.4 for the one real runtime reader.

### 1.4 Where the activity names are consumed at runtime — OBSERVED (structure), INFERRED (purpose)

`sub_67CF06` (0x67CF06, size 0x288F) is the **map-object attribute reader** — it is the function
that holds the literal properties `hpTagColor`, `hpTagBgcolor`, `fixedDamage`,
`onlyNormalAttack`, `cantPassByTeleport`, `mbookID`, `weapon`, `speak`, `chatBalloon`, `con` —
and it is the only function with a *non-destructor* xref into both name vectors. It walks the
40-entry mob activity vector by pointer arithmetic:

```c
// sub_67CF06 @ ~0x67F4C9 … 0x67F5D1 (verbatim structure)
v343 = nullptr;
v288 = (char *)&byte_BEC4C8;                 // start of the 40-name mob activity vector
do {
    sub_405210(v288);                        // take the name string at this slot
    sub_403935(&v327, v297);                 // resource lookup keyed by that name
    v337 = sub_4032B2(v298, v299);           // resolved node
    sub_433363(&v337);
    if ( (unsigned __int8)sub_5DB9AC(&v344) != 0 )      // node present?
    {
        sub_403382(&v297, &off_AF83B0);      // concatenate literal "speak"
        sub_403935(&v320, v297);             // second lookup: <activityName> + "speak"
        if ( (unsigned __int8)sub_5DB9AC(&a1) != 0 )
            sub_67F85D(v298, (void *)v343);  // register result under index v343
    }
    v343 = (volatile LONG *)((char *)v343 + 1);
    v288 += 4;
} while ( (int)v288 < (int)byte_BEC568 );    // 40 iterations
```

The concatenated literals were resolved directly out of `.rdata` (MSVC string-pool suffix
pointers): `off_AF83B0` = **`"speak"`**, `off_AF8390` = **`"con"`**; neighbours are `0xAF8398`
`chatBalloon`, `0xAF83BC` `weapon`, `0xAF8418` `fixedDamage`, `0xAF8430` `hpTagBgcolor`,
`0xAF844C` `hpTagColor`, `0xAF8470` `mbookID`.

**Rule (OBSERVED):** for each of the 40 activity names the client performs a resource lookup and,
where it succeeds, re-looks-up **`<activityName>` + `speak`** and registers it with an
incrementing index. Per-action speech nodes are therefore addressed **by name**, keyed by the
activity-vector index.

**Marked INFERRED:** that this loop is mob/chat-balloon plumbing rather than an effect table —
the evidence is the literal `"speak"` in the concatenation plus the surrounding
`chatBalloon`/`weapon`/`hpTagColor` reads; no downstream consumer was traced.

### 1.5 The `(byte-24)/2+1` / `(byte-42)/2+1` hypothesis — NOT FOUND / NEGATIVE

Explicit checks:

* Machine-code sweep of all `.text` for `sub reg,24` (`83 /digit 18`) with a
  `shr reg,1` within 6 bytes: 16 hits; **zero** of them also contain `cmp reg,42` within
  300 bytes.
* The literal strings `"attack"` and `"skill"` are **absent** from the exe (§0.1), so the
  client cannot be doing `"attack" + n` string construction in the v83 sense; the names are
  pre-enumerated in the pool (ids 1056–1064, 2735–2751).
* The only range dispatch on a small integer that resembles action segmentation is
  `sub_4853B0` (0x4853B0, size 0x695) which chains `cmp 37 / 42 / 44 / 45` — inspecting the
  disassembly shows this is inside a **165-iteration item/option table loop**
  (`v81 = 165`, `v3 = 16 * v72`, `v78 = 248`), i.e. a fixed-stride record walk, not mob
  stance decoding.

**Verdict: NOT FOUND.** The v83 "activity segment / 2 + 1 → `attack{n}`" formula does **not**
exist as arithmetic in this client. What exists is the 40-name vector of §1.1 plus a name→index
linear search (§1.3). Any `attack7`-style numbering the reconstruction emits must therefore
come from *the server/packet side*, and the client merely matches the resulting text.

### 1.6 Related OBSERVED anchors

| Purpose | Function | Notes |
| --- | --- | --- |
| Mob model/anim load | `sub_4A38E7` (0x4A38E7, 10237 B) | References `walk1, walk2, stand1, stand2, alert, swingO1..OF, swingT1..TF, swingP1..PF, stabO1..TF, shoot1, shoot2, shoot6, shootF, proneStab, prone, fly` — the *user* weapon-anim sets |
| Mob info effect | `sub_67FD81` | Uses id 1649 `Mob/%07d.img/%s/info/hit` |
| Mob per-skill effect/hit | pool 2752/2753/2754 | `Mob/%07d.img/info/skill/%03d/effect`, `…/%03d/hit`, `…/info/skill/%` |
| Mob sounds | `sub_989BA1` (0x989BA1) | `Sound/Mob.img/%07d/%s` |
| Class/creature tags | `sub_4A76E5` (0x4A76E5) | `Creature, User, Npc, Mob, Pet, Item, Portals` |

---

## 2. MOB death / kill / respawn / HP bar

### 2.1 Death animation names — OBSERVED

Death entries are in the same 40-name vector: `die`(1065), `die1`(1066), `die2`(1067),
`dieF`(1068) — i.e. the native client has **four** death animations selectable by name, not a
numeric 0/1/2 kill-type. There is **no** `kill`/`fade`/`smoke` string in the mob anim vector
(`smokeshell` id 3844 is an *item*, not a death effect).

OBSERVED death-related consumers:

| Function | Names used |
| --- | --- |
| `sub_49D0B6` (0x49D0B6) | `dieF` |
| `sub_6BCE5F` (0x6BCE5F, 6845 B) | `attackF`, `dieF`, `magic1`, `hungry`, `boss`, `noRevive`, `readyOff` |
| `sub_832586`, `sub_8351CA` | `dieF` |
| `sub_97931C` (0x97931C) | `die1` |
| `sub_6A4A05` (47 B) | `dieF` (trivial accessor) |

`sub_6BCE5F` is the richest: it mixes `dieF` + `noRevive` + `boss`, so this is where the
"boss / cannot-revive / death" branch lives. Its literal use of the *player* revive text
("You have revived on the current map through the…", id 3013-region) confirms revive handling
sits here.

### 2.2 `noRevive` — OBSERVED

Pool id **1880 = `noRevive`**, consumed at `0x6bdb9c` inside `sub_6BCE5F` and in
`sub_5D4AE1` (0x5D4AE1, 182 B). `sub_5D4AE1` is the small predicate:

```c
// sub_5D4AE1 — reads map/obj "noRevive" flag (a WZ bool property)
```

**Rule (OBSERVED):** revive-suppression is a **boolean WZ property named `noRevive`**, read
per object; it is not a numeric kill-type.

### 2.3 Mob HP tag / level display — OBSERVED

`sub_67CF06` reads literal `.rdata` properties `hpTagColor` (`0xAF844C`) and
`hpTagBgcolor` (`0xAF8430`) plus `mbookID`, i.e. per-mob HP-bar colour comes from
`Mob/%07d.img/info/hpTagColor`. Property readers:

| Property | Reader |
| --- | --- |
| `hp` / `mp` | `sub_67CF06` @0x67F890 / @0x67F8ED, and `sub_7FF1FE`/`sub_7FF726` |
| `HPgaugeHide` (id 1628) | `sub_884B7D` (0x884B7D, 429 B) — single-purpose: read `HPgaugeHide` |
| `AngerGauge` (1629) / `AngerGaugeAnimation` (3074) / `AngerGaugeEffect` (3075) | consumed near `sub_67CF06`/boss UI |

### 2.4 Respawn / regeneration — OBSERVED (weak)

* Pool id **5649 = `regena`** — the regeneration animation name (slot 4 of the 40-name vector,
  right after `fly`), and id **2545 = `Regen`**.
* Pool id **5634 = `interval`** (8 sites: `0x4363A9, 0x436724, 0x5C6F6E, 0x71EAD7, 0x7623EC,
  0x762902, 0x7BBD9B, 0x93BA2D`) — the respawn/period property. In the mob-skill reader
  `sub_7626BD` it is read alongside `time`, `prop`, `count`, `level`.
* There is **no** `respawn`/`revive` literal in the pool, so per-map mob respawn cadence is
  not stored under those names; it is driven by `interval` + `Regen`.

**Marked INFERRED:** mob count/`respawn` bookkeeping for the *map's* spawn list is server-side
in this lineage; the client only animates `regena` on the `interval` it was told.

---

## 3. REACTOR

### 3.1 Reactor WZ paths — OBSERVED

| Pool id | String | Consumer |
| --- | --- | --- |
| 2970 | `Reactor` | `sub_9F7159` (the 15-name WZ root array) |
| 2980 | `reactor` | `sub_639B3D` (0x639B3D), `sub_63AA7E` (0x63AA7E) |
| 3076 | `Reactor/%s.img` | `sub_739A92` (0x739A92), `sub_73A865` |
| 3078 | `Reactor/%0`(7d) | `sub_73A649` (0x73A649), `sub_73A865` |
| 5102 | `reactorName` | `sub_71165A` (0x71165A) |
| 5112 | `reactorState` | reactor state setter |
| 3073 | `Sound/Reactor.img/%s/%s` | `sub_989ADD` (§3.3) |
| 3079 | `%d/hit` | reactor hit frame path |
| 3080 | `event/%d/hit` | event-reactor hit path |

`sub_9F7159` is the loader that mounts the roots:

```c
// sub_9F7159 @0x9F745A (OBSERVED)
v53[0]="Character"; v53[1]="Mob";   v53[2]="Skill";  v53[3]="Reactor";
v53[4]="Npc";       v53[5]="UI";    v53[6]="Quest";  v53[7]="Item";
v53[8]="Effect";    v53[9]="String";v53[10]="Etc";   v53[11]="Morph";
v53[12]="TamingMob";v53[13]="Sound";v53[14]="Map";
for (i = 0; i < 15; ++i) {
    ...
    v17 = sub_416821(Format: L"%s.wz");     // "Reactor.wz" etc.
    sub_5D995B(&v51, v39);                  // PcCreateObject / NameSpace mount
}
```

### 3.2 Reactor properties — OBSERVED

`sub_71165A` (0x71165A, 3257 B) reads the full reactor `info` set. Its recovered string IDs:

```text
pn  pt  x  y  hRange  vRange  tm2  tnd  hi  delay  onlyOnce
verticalImpact  horizontalImpact  reactorName  /startnotice=  image  default
```

and `sub_739A92` (the reactor `Reactor/%s.img` reader) reads:

```text
info  move  backTile  activateByTouch  link  repeat  event   (+ "Reactor/%s.img")
```

So the native reactor schema is:

| Property | Meaning / effect |
| --- | --- |
| `pn` / `pt` | portal name / portal target on reactor completion |
| `x`,`y` | position |
| `hRange` / `vRange` | horizontal / vertical activation range |
| `hi` | hit interval / hit-count limit |
| `tm2` / `tnd` | time-based reactor fields |
| `delay` | state-change delay |
| `onlyOnce` | fire once |
| `verticalImpact` / `horizontalImpact` | impulse applied to movers |
| `activateByTouch` | trigger on touch |
| `repeat` / `event` | looping / event reactor |
| `link` | linked reactor chain |
| `move` + `backTile` | reactor that moves |
| `default` (id 960) | the floating/default animation node |

`default` consumers (`0x40816B, 0x4090D5, 0x66A068, 0x712213, 0x719163, 0x7196A2, 0x80076B,
0x80186A, 0x866E7A, 0x93C04F`) — including `sub_71165A` itself and `sub_800214`/`sub_801474`
— is the Horntail-style *floating animation* node. **Rule (OBSERVED):** when a reactor's
state node is missing, the client falls back to the `default` node rather than drawing nothing.

Reactor visual frame source is the generic map-object path:
`sub_63AD16` (0x63AD16, 5372 B) formats **`Map/Obj/%s.img/%s/%s/%s`**
(site `0x63AE3C`), i.e. `Map/Obj/<set>.img/<obj>/<state>/<frame>`.

### 3.3 Reactor hit/destroy SOUND — OBSERVED, exact function

```c
// sub_989ADD @0x989ADD (196 bytes) — reactor sound dispatcher
void __cdecl sub_989ADD(int a1, void *Src, int a3)
{
  if (Src == (void *)51)                     // *** literal event code 51 ***
  {
    sub_79E805();
    v3 = sub_406276(a1: &v6, a2: 5543);      // id 5543 = "Hit"
    sub_41CF93(v3);
    Src = nullptr;
    sub_79E805();
    v4 = *(wchar_t **)sub_406276(a1: &v5, a2: 3073);   // "Sound/Reactor.img/%s/%s"
    sub_416821(a1: &Src, Format: v4);        // -> Sound/Reactor.img/<reactorName>/Hit
    sub_43FB25(Src, a2: a3, a3: 0);          // play
  }
}
```

Pool id 5543 is the literal `Hit` (title-case) and 5631 is lowercase `hit`.

**Rule (OBSERVED):** the native client plays a reactor sound **only for event code 51**, and
resolves it as `Sound/Reactor.img/<reactorName>/Hit`. It does *not* read `Reactor.img` event
`0`, and there is no `destroy` sound path — `destroy` does not appear anywhere in the decrypted
pool. This is a concrete gap to expect in a reconstruction that "plays Reactor event 0".

`sub_43FB25` is the sound-play helper; the final backend is `sub_730277`, which drives mss32:

```c
// sub_730277 imports: _AIL_quick_load_mem@8 (0xAF03BC), _AIL_quick_play@8 (0xAF039C),
//                    _AIL_quick_unload@4, _AIL_release_all_handle
// sole caller: sub_72FD72  (single sound cache/loader)
```

Sound path family (OBSERVED, ids 2195–2205, 3073, 5357):

```text
Sound/Item.img/%08d/%s     Sound/Skill.img/%07d/%s    Sound/Skill.img/%07d/Hit
Sound/Mob.img/%07d/%s      Sound/Summon.img/%d        Sound/Quest.img/%d
Sound/Pet.img/Basic/%s     Sound/Pet.img/%07d/%s      Sound/Reactor.img/%s/%s
Sound/Object.img/          Sound/BgmUI.img/…          Sound/Radio.img/%s[/track]
```

`sub_989BA1`, `sub_989A79`, `sub_98A2D4`, `sub_98A4B2`, `sub_989729` are the thin per-family
wrappers (mob / object / summon / pet / skill).

### 3.4 Movable / quest reactors (snowball)

* `snowBall` = pool id **1466**, consumed in `sub_574906` (0x574906, 1259 B) together with
  `portal` (1473). Snowball/pulley property literals: `pulley` (`0xAF4A58`) and `healer`
  (`0xAF4A68`) + `yMin`, read by **`sub_5587B3` (0x5587B3)** — these are Guild-Bossk/
  pulley/healer map objects, literal `.rdata` (not pool) so they are easy anchors.
* `massacre` family (`mobMassacre` id 0xAF4B54) and ids 5341–5346
  (`massacre_cool/hit/laststage/miss/party/skill`) cover the party-quest reactors.
* `countFalling`, `countBombing`, `countStopped`, `coconut` (ids 2776–2779) are read by
  `sub_548B77` (0x548B77, 2896 B) — the **Kerning/Snowball-type objective counters**, plus
  `Map/Obj/etc.img/killing/backgrnd` + `…/killing/fontTime` (literal, `sub_56C65A` /
  `sub_56D21D`).

**Marked INFERRED for the "movable reactor" (quest-reactor drag) case specifically:** the pool
contains `move` as a reactor property in `sub_739A92`, which is the enabling flag; the actual
drag mechanics were not traced to a dedicated function.

---

## 4. PORTAL

### 4.1 Portal paths / property reads — OBSERVED

| String | id | Consumer |
| --- | --- | --- |
| `Map/MapHelper.img/portal/game/pg` | 1807 | portal graphics (`sub_739A92` family, `sub_854B8C`) |
| `Map/MapHelper.img/mark/` | 1732 | `sub_855D02` (0x855D02, 9737 B) — minimap marks |
| `Map/MapHelper.img/worldMap/npcPos` | literal `0xB3F288` | `sub_9EA93B` |
| `String/ToolTipHelp.img/PortalTooltip/%d` | 3771 | `sub_85ADE2` (0x85ADE2, 3318 B) — **portal tooltip** |
| `mapName` | near 3771 | same function |
| `Effect/BasicEff.img/Teleport` | 2180 | `sub_957B74`, `sub_97FDF8` — warp effect |
| `Effect/PetEff.img/Basic/Teleport` | 1688 | `sub_70537D` |
| `targetPortal` | 5117 | portal-target resolution |
| `town` | 2699 | `sub_529EF2` (0x529EF2, 900 B) — town/return portal |
| `force` | 2972 | `sub_A43E7B` (0xA43E7B, 4997 B) — forced-movement / `forbidFallDown` |

Lifecycle strings (OBSERVED, ids 4388–4390) are **not** numbers but names:

```c
// sub_712D35 @0x712D35 (1527 B)  — portal script/lifecycle emitter
... sub_406276(&v, 4390);   // "portalExit"     @0x712E7F
... sub_406276(&v, 4388);   // "portalStart"    @0x713147
// sub_71332C (703 B)
... sub_406276(&v, 4389);   // "portalContinue"
```

plus `sub_712313`, `sub_71259D`, `sub_529BB4` all using `portal` (1473) and `sub_854B8C`
(429 B, `portal=`) for the `game`/`pv` variants.

### 4.2 Portal *type* enumeration — NOT FOUND

The v83-style numeric portal types 0..14 (0=normal, 4=scroll, 5=switch, 6=hidden,
7=script, 8=vertical, 9=horizontal, 10=terminal, 12=party-only…) leave **no trace** in this
binary: there is no `ptn`/`term`/`script`(as a portal key)/`hidden`/`interval` (as a portal key)
literal, and no 15-entry jump table or 15-way `cmp` chain anywhere in the portal functions
(verified: `.text` contains **zero** jump tables — the full jump-table sweep produced only
`switch`-style tables for *other* code, none in `0x7123xx–0x7134xx`, `0x85ABxx`, `0x529Exx`).

What the client *does* branch on is a mixture of **booleans/properties** (`activateByTouch`,
`onlyOnce`, `force`, `targetPortal`, `town`) and the named lifecycle events
(`portalStart`/`portalContinue`/`portalExit`). Spring portals: no `spring` token exists in the
5659-entry pool → **NOT FOUND**.

**Rule to hand to the reconstruction:** do not expect `switch(type)` — the native client
models portals as *property flags plus named lifecycle hooks*. Portals that the v83 docs call
"scroll"/"switch"/"script" are realised through `targetPortal`, `town`,
`Effect/BasicEff.img/Teleport`, and the `script` entry via
`Etc/ScriptInfo.img` (id 3196).

### 4.3 Portal warp packet send — NOT FOUND

The outbound side could not be attributed. The `ws2_32` import table has only 12 entries and
contains **no `send`/`recv` symbol** — those are reached through an indirect IAT:

```c
// sub_494ED1 @0x494FF6 (OBSERVED recv path)
v8 = dword_BF0674(a1: *(_DWORD *)(this + 8), a2: v6, a3: v7, a4: 0);   // recv wrapper
...
if (dword_BF0640() == 10035)      // WSAGetLastError wrapper == WSAEWOULDBLOCK
    dword_BF02F4(500);            // Sleep(500)
```

`sub_494ED1` (0x494ED1, 1533 B) is the **connection handshake / receive pump**, not a
dispatcher: it reads a 2-byte length, then an ASCII opcode string
(`sub_46F37B(&String, v5+1, len)` followed by `atoi(String)`), then two dwords, and enforces
`version == 83` (`(u16)v52 != 83 → throw AVCPatchException`; `sub_51E834(83)`),
`getpeername(...)`, and throws `AVCDisconnectException`/`AVCTerminateException`
(`570425345`/`570425351`) on failure. The exception classes `ZException`(38),
`CDisconnectException`, `CTerminateException`, `C_PATCH_FAILED` confirm the
`Net`/`Session` COM split. `sub_6EC85D` is the *decode-failure* reporter:

```c
// sub_6EC85D
sub_414617("Invalid Decoding\r\n");
sub_47C957(&v16, "PacketDump : ");   // + hex dump via sub_6EC7B1 ("%02x ")
```

**Marked NOT FOUND:** the packet-handler dispatch table. It is not a contiguous function-pointer
array in `.rdata`/`.data` (a full sweep for runs ≥25 consecutive in-`.text` pointers found only
`0xAF044C` (43 CRT initialisers), `0xAF0860` (57 CRT initialisers) and the
`0xBD7000` **C++ static-constructor table** — 1166 entries, terminated by
`__xt_z`/`__xi_z`, *not* packet handlers). Dispatch is therefore COM `QueryInterface`-based
(the `dword_BE7B38`/`dword_BE7918` service singletons), which IDA cannot resolve statically.

---

## 5. BOSS HP BAR (`MobGage`)

### 5.1 The boss HP bar is `UI/UIWindow.img/MobGage` — OBSERVED

`sub_5336CA` (0x5336CA, **4971 bytes**) is the boss-HP widget constructor/draw. Its recovered
property IDs are exactly the four backgrounds plus the mob/gauge sprites:

```text
3288  UI/UIWindow.img/MobGage/backgrnd
3289  UI/UIWindow.img/MobGage/backgrnd2
3290  UI/UIWindow.img/MobGage/backgrnd3
3291  UI/UIWindow.img/MobGage/backgrnd4
3292  UI/UIWindow.img/MobGage/Mob/%d       @0x533C86   <- per-mob portrait
3293  UI/UIWindow.img/MobGage/Gage/%d/0    @0x533E3F   <- gauge frame 0
3293/3294  MobGage/Gage/%d/0 and /1        @0x5340D2   <- gauge fill states
```

Callers:

```text
sub_5336CA  <- sub_5330F7 (0x5330F7, 869 B)   [the boss-HP manager / packet consumer]
            <- sub_6675A8 (x2 @0x66889B, 0x6689C2)
```

`sub_5330F7` (0x5330F7, 869 B) is therefore the boss-HP **entry point**; `sub_534ACA`
(1224 B) also participates (it uses `repeat`, id 2499).

**Rule (OBSERVED):** the native bar is keyed by **mob id** (`MobGage/Mob/%d`), and each slot
draws a `Gage/<n>/0` + `Gage/<n>/1` pair — i.e. the HP gauge is a **two-frame sprite per mob**
(background `0` + fill `1`), with four selectable overall background skins
(`backgrnd`..`backgrnd4`).

### 5.2 Mob count / reappear logic — NOT FOUND (bounded)

`sub_5336CA` contains **no** comparison against any small constant (sweep for `== N`, `< N`,
`>= N` over the pseudocode returned only `0`), and no loop bound. So the "how many mobs are
cached" number is **not** a literal in the widget; it is taken from the packet/manager argument.
`sub_5330F7` (the manager) likewise exposes no constant. The classic v83 `25`/`MobCount`
limit is therefore **not confirmable from the client side here** and must be taken as a
server-side constant. The `AngerGauge`/`AngerGaugeAnimation`/`AngerGaugeEffect` trio
(ids 1629/3074/3075) is the boss enrage meter that shares the same UI family.

### 5.3 Adjacent gauges (do not confuse)

```text
UI/UIWindow.img/DualMobGauge/{backgrnd,gauge,text,Mob/9700036}   (literal .rdata, sub_56C9B7)
UI/UIWindow.img/MonsterKilling/Gauge/{backgrd,backgrdD,danger,iconD,text,textD}  (sub_55F33F)
UI/UIWindow.img/MonsterKilling/Count/number{,2}                  (sub_55F33F)
UI/StatusBar.img/gauge/{bar,gray,hpFlash,mpFlash,graduation,tempExp}
```

`sub_56C9B7` = DualMobGauge (the 2-vs-2 "Dual Boss" gauge, hard-coded mob id 9700036);
`sub_55F33F` (3969 B) = MonsterKilling (mob-massacre) gauge. Neither is the ordinary boss bar.

---

## 6. SUMMON / PET auto-pickup / MIST / DOOR

### 6.1 SUMMON — OBSERVED

```text
2198  Sound/Summon.img/%d                 -> sub_98A2D4 (118 B)
5656  summon                              -> sub_75DEF
1080  summoned                            -> sub_471F37, sub_4A9630, sub_5E674A, sub_5F83EE
2800  summonEffect                        -> sub_7626BD @0x7630CD
2393  Skill/%03d.img/skill/%07d/summon    -> sub_75CD52 (8469 B)
2719  Skill/%03d.img/skill/%07d/summon/%s/info/effect  -> sub_7ACB5A
2720  Skill/%03d.img/skill/%07d/summon/%s/info/hit     -> sub_7ACB5A
2205  Sound/Summon.img/%d                  (as part of 2195-2205 family)
5563  Summon                              -> sub_7CCEA4, sub_89EC7C (2460 B each)
4419  Effect/MapEff.img/NpcSummon         -> sub_6D59B0 (746 B)
4420  Effect/MapEff.img/NpcReturn         -> sub_6D59B0
```

`sub_7ACB5A` (2837 B) is the summon-skill data reader; its full recovered ID set is:

```text
info, PADamage, MADamage, range, sp, r, lt, rb, mobCount, attackAfter,
effect, Skill/%03d.img/skill/%07d/summon/%s/info/effect, ball, bulletSpeed
```

**Rule (OBSERVED):** summon damage/geometry comes from `Skill/<job>.img/skill/<id>/summon/<key>/info`
— `PADamage`/`MADamage`/`range`/`mobCount`/`attackAfter`/`bulletSpeed`, plus a `ball` node
(`Mob/%07d.img/%s/info/ball`, id 1650). The 15-entry *summon* anim vector of §1.2
(`stand, move, fly, summoned, att, attack2, skill1..6, hit, die, say`) governs what a summon
can animate — note it has **only 6 skill slots**, so a reconstruction that lets summons play
`skill7+` is over-permissive versus native.

`sub_471F37` (1540 B) and `sub_5F83EE` (2472 B) use `summoned`; `sub_5E674A` is a small
accessor. `Etc/Category.img` + `summoned` in `sub_471F37` is the summon-box UI category list.

### 6.2 PET auto-pickup — OBSERVED

```text
1631  pickUp
3656  pickUpBlock
1245  PickUpItem
3083  pickupAll      3084  pickupItem    3085  pickupMeso    3086  pickupOther
3092  ignorePickup
3601  "The Auto-Pickup Skill for %"
1236  ScrollUp
Effect/PetEff.img/Basic/{LevelUp,Teleport,Evolution}   ids 1687/1688/1689
Effect/PetEff.img/%07d/warp                            id 1690
Sound/Pet.img/Basic/%s                                 id 2201
Sound/Pet.img/%07d/%s                                  id 2202
```

Consumers: `sub_70537D` (603 B) plays the pet effects; `sub_919B73` (7622 B) is the huge
pet/inventory context menu that resolves `Sound/Object.img/` + `Sound/Pet.img/Basic/%s`;
`sub_5CAC3D` (18802 B) reads `pickupMeso`. Pet auto-speak/auto-recall limits are literal
`.rdata`: `petAutoSpeakingLimit`, `petRecallLimit`, `autoSpekaing` (note the **typo**
`Spekaing` — reproduced verbatim from the binary, read by `sub_70AF01`).

**Rule (OBSERVED):** the pickup namespace is `pickUp` / `pickUpBlock` / `ignorePickup` plus the
four-way `pickup{All,Item,Meso,Other}` selector — auto-pickup is a *command/skill flag*, not an
animation. The only pickup *animations* are the pet `Effect/PetEff.img/Basic/*` set.

### 6.3 MIST — OBSERVED (identified via MobSkill tile, not a "mist" string)

`sub_7626BD` (0x7626BD, 3125 B) is the single mist/poison field reader. Full recovered ID list:

```text
hp, conMP, time, interval, prop, x, y, count, direction, elemAttr, lt, rb,
effect,   Skill/MobSkill.img/%03d/level/%d/effect
hit,      Skill/MobSkill.img/%03d/level/%d/hit
mobQ,     Skill/MobSkill.img/%03d/level/%d/mob
affected, tile, summonEffect, limit
```

plus literal `.rdata` templates: `Skill/MobSkill.img/%03d/level/%d/tile` at `0xAFE1F0`,
`Skill/MobSkill.img/%03d/level/%d/affected` at `0xAFE23C`, and
`Skill/ItemSkill.img/%03d/level/%d/mob` at `0xAFE1A2`.

**Rule (OBSERVED):** a mist is defined by `lt`/`rb` corner points plus `interval` (HP/MP
drain period), `prop` (probability), `hp`/`conMP` (cost/condition), `time`, `elemAttr`,
`mobQ`, `limit`, and its visuals are `Skill/MobSkill.img/<skill>/<level>/{tile,effect,hit}`.
The poison *message* is pool id 2911 (`You have been poisoned, so your HP will slowly decrease.`),
consumed by `sub_932DA6` (1853 B) which is the **status-change message table** (also holds
`seal/weaken/stun/curse/darkness/seduce/undead/covered-by-mist/fear` texts — note id 3503-region
"You will receive damage if you become covered …" is the *mist-overlap* message).

There is **no** `mist`/`Mist` token in the pool: natively it is `MobSkill` **tile**, i.e.
`Skill/MobSkill.img/.../tile`. A reconstruction naming it `Mist`/`FieldMist` is fine as an
internal name but must keep the `tile` node contract.

### 6.4 DOOR — OBSERVED

```text
2714  Skill/231.img/skill/2311002/mDoor   -> sub_7BD262 (818 B), sub_5365C8 (4129 B)
2716  Skill/231.img/skill/2311002/cDoor   -> sub_7BD6C6 (2362 B)
2715  Skill/231.img/skill/2311002/Fr…     -> sub_7BD262, sub_7BD6C6, sub_5365C8
3181  "You cannot use the Mystic Door Skill near the portal."
```

`sub_7BD6C6` fetches `mDoor` (the *master/own* door sprite), `…/F…` (the *far/other-end*
sprite) and `cDoor` (the *close*/client-side sprite) from `Skill/231.img/skill/2311002`.
`sub_5365C8` (4129 B) is the higher-level door handler that also touches `blaze` and `/answer`.

**Rule (OBSERVED):** Mystic Door is drawn from **three named nodes** under the Bishop skill id
`2311002` (`mDoor`, `cDoor`, and the `Fr*` node) inside `Skill/231.img` — the door is a *skill
resource*, not a `Map/Obj` resource. Its restriction ("not near a portal", id 3181) is a
message-level rule enforced where the door is opened.

---

## 7. DROP (meso stack/merge, item flip, owner protection)

### 7.1 Item flip — OBSERVED, exact

```text
4151  flip        -> sub_406ABD (2313 B), sub_717963 (1831 B)
2982  noFlip      -> sub_5C68E3 (4088 B), sub_67CF06
980   fall        -> sub_43612F (1196 B)
2181  Effect/Tomb.img/fall  -> sub_92FD5A (3257 B)
2777  countFalling / 2778 countBombing / 2779 countStopped -> sub_548B77
```

`sub_5C68E3` (4088 B) reads **`noFlip`** alongside the mob-anim vector, and `sub_67CF06`
(also `noFlip`) is the shared object-attribute reader. `sub_406ABD` (2313 B) and `sub_717963`
(1831 B) read `flip`.

**Rule (OBSERVED):** flip is a **boolean property pair** — an object may set `noFlip` to
suppress the tumbling spawn animation, while `flip` is read for the objects that do animate.
There is no numeric flip *duration* in the pool: the duration comes from the animation itself
(`Effect/Tomb.img/fall` is the canonical falling effect). The `countFalling/countBombing/
countStopped` trio (`sub_548B77`) is the **objective counter** for falling objects
(Kerning Square/Coconut-style minigames), not per-item flip state.

### 7.2 Meso drop / merge — OBSERVED, with an important negative

```text
5550  Meso     -> sub_47FF89 (3268 B)   [meso-market/auction row formatter]
5639  meso     -> sub_426CE6, sub_4271B6, sub_4B7B98, sub_4BBC6B,
                  sub_754E78, sub_75577A, sub_A0C03C (in sub_A0A63F)
5510  mesos    -> sub_4852C8, sub_5A8E54, sub_5AD04A, sub_7061A5, sub_8FCA75
5646  protectTime -> sub_A0C03C  (inside sub_A0A63F, 17557 B)
3085  pickupMeso
1848  UI/UIWindow.img/Item/BtCoin    (sub_5F726D, sub_5F76AE, sub_5F7C4A, sub_6A3FBB, sub_81C6C9)
1819  UI/UIWindow.img/TradingRoom/BtCoin
1631  pickUp
```

`protectTime` (literal `0xAF64B4`, pool 5646) is read by **`sub_5D2319`** (0x5D2319, 94 B —
the pure accessor) *and* used inside the big `sub_A0A63F` at `0xA0CBE5`:

```c
// sub_5D2319 @0x5D2319 — literal "protectTime" accessor
sub_406292(a1: &p_pvargDest, a2: 961);            // id 961 = "info"  -> object's info node
...
sub_425ADD("protectTime");                        // property read
v7 = sub_414D40(v6, v12);                         // VARIANT -> int
*(_DWORD *)(sub_5DBCEA(v15) + 4) = v7;            // store at struct+4
```

**Rule (OBSERVED):** owner-protection time is read from the **item's own `info` node**
property `protectTime` (a plain int), and the client caches it at `obj+4`. It is
*not* a hard-coded per-drop timer. `sub_5D2FC1` reads the sibling trio `recoveryInterval`,
`MPrecovery`, `HPrecovery`; `sub_5D2166` reads `maxDays`; `sub_5D2473` reads `recoveryRate` —
so this cluster (`0x5D21xx–0x5D30xx`) is the generic **item `info` property reader set**.

### 7.3 Meso stack/merge — NOT FOUND

The decrypted pool contains **no** `stack`, `merge`, `combine`, `overlap`, `mesoBag`(as a
key), `dropLimit`, or `MaxMeso` token, and `sub_47FF89`'s 17 IDs are all
*trade-listing* fields (`ItemId, Count, Price, Priority, Period, MaplePoint, Premium, reqLev,
Gender, OnSale, Class, Limit, PbCash, PbPoint, PbGift, SN=`). Meso-bag UI text exists
(id 797/798/799, plus the literal `"Users under level 15 users cannot use the meso bags."` at
`0xB3F748` → `sub_A0A63F`), but **no stack/merge algorithm was located in the client**.

**Conclusion (honest):** meso stacking/merging is not implemented as a visible client rule in
this binary. On v83-lineage clients the drop object count (and thus any "merge") is dictated by
the server's drop packets; the client just renders each object. A reconstruction should not
invent a client-side merge rule and claim it is native.

---

## 8. Consolidated function index (all OBSERVED addresses)

| Area | Function | Size | Role |
| --- | --- | --- | --- |
| Resource | `0x9F7159` | — | mount 15 `.wz` roots (`Reactor`, `Sound`, `Map`, …) |
| Resource | `0x406276`/`0x79EA63`/`0x79E993`/`0x79ECDE`/`0x79EBF3` | — | string-by-ID pool resolver + XOR key schedule |
| Packet | `0x494ED1` | 1533 | recv pump + version-83 handshake |
| Packet | `0x6EC85D` | 299 | `Invalid Decoding` + `PacketDump : ` |
| Packet | `0x46F37B` | — | TLV string extractor (`[u16 len][bytes]`) |
| Mob anim | `0x4A6C05` | 2309 | build 40-entry mob activity name vector `BEC4C8` |
| Mob anim | `0x4A61AD` | 2606 | build 46-entry user anim vector `BEC568` |
| Mob anim | `0x4A9630` | 863 | build 15-entry summon anim vector `BEC3BC` |
| Mob anim | `0x4A7510` | — | build 4-entry mount family `BEC4B8` |
| Mob anim | `0x4A8D5D` | — | **activity name → index** linear search |
| Mob anim | `0x402CCC` | — | string equality helper used by the search |
| Mob anim | `0x4A38E7` | 10237 | weapon/user animation set selection |
| Mob anim | `0x67CF06` | 10351 | mob attribute reader (`hpTagColor`, `fixedDamage`, per-action `/effect`) |
| Mob anim | `0x49D0B6`,`0x6BCE5F`,`0x832586`,`0x8351CA`,`0x97931C`,`0x6A4A05` | — | `die*`/`attackF`/`noRevive` users |
| Reactor | `0x739A92` | 1754 | `Reactor/%s.img` reader: `info move backTile activateByTouch link repeat event` |
| Reactor | `0x73A649`,`0x73A865` | 483,1046 | `Reactor/%07d.img` + `link`/`info` |
| Reactor | `0x639B3D`,`0x63AA7E` | 1475,664 | `reactor` object state (`foothold`,`ladderRope`,`life`,`type`,`id`) |
| Reactor | `0x71165A` | 3257 | reactor `info`: `pn pt x y hRange vRange tm2 tnd hi delay onlyOnce …` |
| Reactor | `0x63AD16` | 5372 | map-object frame resolver `Map/Obj/%s.img/%s/%s/%s` (+ `s%d`) |
| Reactor | `0x989ADD` | 196 | **reactor sound: only event code 51 → `Sound/Reactor.img/<name>/Hit`** |
| Sound | `0x989BA1`,`0x989A79`,`0x98A2D4`,`0x98A4B2`,`0x989729` | — | per-family sound wrappers |
| Sound | `0x72FD72`,`0x730277` | — | sound cache + mss32 backend |
| Portal | `0x712D35`,`0x71332C` | 1527,703 | `portalStart` / `portalExit` / `portalContinue` |
| Portal | `0x712313`,`0x71259D`,`0x529BB4`,`0x854B8C` | — | `portal` / `game` / `pv` variants |
| Portal | `0x529EF2` | 900 | `town` portal |
| Portal | `0x85ADE2` | 3318 | `PortalTooltip/%d`, `mapName` |
| Portal | `0xA43E7B` | 4997 | `force`, `forbidFallDown` |
| Portal | `0x957B74`,`0x97FDF8` | — | `Effect/BasicEff.img/Teleport` |
| BossHP | `0x5330F7` | 869 | boss-HP manager (entry) |
| BossHP | `0x5336CA` | 4971 | `MobGage` widget: `Mob/%d`, `Gage/%d/0`, `Gage/%d/1`, 4 skins |
| BossHP | `0x534ACA` | 1224 | boss-gauge `repeat` handler |
| BossHP | `0x884B7D` | 429 | `HPgaugeHide` reader |
| BossHP | `0x55F33F` | 3969 | MonsterKilling gauge (**not** boss bar) |
| BossHP | `0x56C9B7` | — | DualMobGauge (mob 9700036) |
| Summon | `0x75CD52` | 8469 | `Skill/%03d.img/skill/%07d/summon` |
| Summon | `0x7ACB5A` | 2837 | summon info: `PADamage MADamage range sp r lt rb mobCount attackAfter effect ball bulletSpeed` |
| Summon | `0x7CCEA4`,`0x89EC7C` | 2460 | `Summon` object |
| Summon | `0x6D59B0` | 746 | `NpcSummon`/`NpcReturn` effects |
| Summon | `0x471F37`,`0x5F83EE` | 1540,2472 | `summoned` state |
| Pet | `0x70537D` | 603 | `PetEff.img/Basic/{LevelUp,Teleport,Evolution}`, `%07d/warp` |
| Pet | `0x919B73` | 7622 | pet/object sound + context menu |
| Pet | `0x70AF01` | 10215 | `autoSpekaing` (sic) |
| Mist | `0x7626BD` | 3125 | MobSkill mist field (`lt rb interval prop hp conMP time count direction elemAttr tile mobQ affected limit summonEffect`) |
| Mist | `0x932DA6` | 1853 | status message table (poison/seal/stun/…/mist-cover) |
| Door | `0x7BD6C6` | 2362 | `Skill/231.img/skill/2311002/{mDoor,cDoor}` |
| Door | `0x7BD262` | 818 | `mDoor` + `Fr*` |
| Door | `0x5365C8` | 4129 | door handler (mystic-door restrictions) |
| Drop | `0x406ABD`,`0x717963` | 2313,1831 | `flip` |
| Drop | `0x5C68E3` | 4088 | `noFlip` |
| Drop | `0x43612F` | 1196 | `fall` |
| Drop | `0x92FD5A` | 3257 | `Effect/Tomb.img/fall` |
| Drop | `0x548B77` | 2896 | `countFalling/countBombing/countStopped` |
| Drop | `0x5D2319` | 94 | `protectTime` accessor |
| Drop | `0x5D2FC1`,`0x5D2473`,`0x5D2166` | — | `recoveryInterval/MPrecovery/HPrecovery`, `recoveryRate`, `maxDays` |
| Drop | `0x47FF89` | 3268 | `Meso` trade-listing formatter |
| Map | `0x52A5C7` | 3089 | equip-slot property array (`mobequip tamingmob saddle petwear …`) |
| Map | `0x52B1D8`,`0x52B2B5` | — | `noSkill`, `swimArea` |
| Map | `0xA43433` | 2316 | `Map/Physics.img` props (`walkForce walkSpeed walkDrag fallSpeed jumpSpeed maxFriction`) |
| Map | `0x97BD9A` | — | `Map/MapHelper.img/weather/squib/squib%d` |
| Map | `0x855D02` | 9737 | `MapHelper.img/mark/` (minimap) |

---

## 9. Actionable deltas for `E:\project\MapleStory-Client`

1. **Area 1 — replace numeric stance decoding with name matching.** The reconstruction's
   `attack{n} = (byte-24)/2+1` / `skill{n} = (byte-42)/2+1` formula has **no native
   counterpart** (§1.5). Native uses a 40-entry name vector + linear name→index compare. Keep
   the wire format if the server needs it, but drive animation selection from the *name*.
   Note `att`(base) vs `attack`(18) vs the missing `attack1`/`attack7` slots.
2. **Area 3 — fix the reactor sound rule.** Play only for event **51**, and resolve as
   `Sound/Reactor.img/<reactorName>/Hit`. Remove any "event 0 = hit sound" assumption. There is
   no `destroy` sound node in the client.
3. **Area 3 — add `default` fallback for reactors** (Horntail floating), and honour
   `activateByTouch`, `onlyOnce`, `hRange`/`vRange`, `verticalImpact`/`horizontalImpact`,
   `link`, `repeat`, `event`, `move`+`backTile`, `pn`/`pt`.
4. **Area 4 — drop the `switch(portalType)` model** in favour of flags + `portalStart`/
   `portalContinue`/`portalExit` named hooks (§4.2). No spring portals in native.
5. **Area 5 — validate `UIBossHP` against `MobGage`.** Two-frame gauge per mob
   (`Gage/<n>/0` + `/1`), portrait `MobGage/Mob/<mobId>`, 4 background skins. The mob-count
   limit is **not** a client constant — do not hard-code one and claim parity.
6. **Area 6 — summon anims are capped at `skill1..skill6` + `say`** (§6.1); MIST is
   `Skill/MobSkill.img/…/tile`, not a `Mist` resource; DOOR lives under `Skill/231.img/
   skill/2311002/{mDoor,cDoor,Fr*}`.
7. **Area 7 — flip is `flip`/`noFlip` boolean pair; `protectTime` comes from the item's own
   `info` node** (§7.1–7.2). Do not add a client-side meso merge rule (§7.3).
8. **Tooling:** `tools/beidou_stringpool.py` now recovers any pool id → string. Use it instead
   of `grep`ing the exe; the prior "string not found ⇒ feature absent" reasoning was invalid.

---

## 10. Method / reproducibility

* IDA MCP over `http://127.0.0.1:13337/mcp`, DB `BeiDou.exe.i64`, `server_health` →
  `auto_analysis_ready: true`, `hexrays_ready: true`, `strings_cache_size: 3889`
  (note: that cache is **only** the pre-typed strings — the raw scan found 13 265 candidate
  byte runs, which is what exposed the obfuscation).
* Helpers added (analysis-side only, no IDB writes):
  `tools/_mcp2.py` (full-length MCP calls), `tools/_idap.py`, `tools/dec.py`
  (decompile → file, bypasses the 6 KB MCP truncation),
  `tools/fetch_full.py` (retrieve `http://127.0.0.1:13337/output/<id>.json` for >6 KB results),
  `tools/beidou_stringpool.py`, `tools/_out/*.py` scans.
* IDA `py_exec_file` has a **hard 60 s server-side timeout**; all heavy scans were moved to
  local file reads (`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`) with manual PE
  offset↔VA mapping (`.text` 0x401000↔0x1000, `.rdata` 0xAF0000↔0x6F0000, `.data` 0xBD7000↔0x7D7000).
* **No writes were made to the IDB** (no rename/comment/type/patch calls).
* Not executed / out of scope: the `.img` containers under `Data/` are separately encrypted
  (magic `73f86c77fc7983271958`, position-dependent keystream; identical headers across
  `Mob/`, `Reactor/`, `smap.img`, `zmap.img`) and were **not** decrypted — animation frame
  node names therefore come from the exe's own pool, not from the containers.
