# Quest Check.img field probe notes (BeiDou / v83)

Source packs typically expose start requirements under `Quest/Check.img/{questId}/0`.

## Fields read by `QuestData`

| Field | Meaning | Notes |
|---|---|---|
| `lvmin` | Minimum level | Optional; 0 = no min |
| `lvmax` | Maximum level | Optional; 0 = no max |
| `npc` | Related start NPC | Display / future map marker |
| `job` | Allowed jobs | Integer, string, or directory of ints |
| `job0` | Alternate job list | Used if `job` empty |
| `quest` | Prerequisites | Flat ints, or children with `id` |

## Fallback roots

- `Quest/Check.img/{id}` root also scanned if `/0` has no filters
- Names from `String/Quest.img/{id}` (`name`)
- Summary from `Check`/`QuestInfo`/`Act` `info`/`summary`

## TAB0 available list

`QuestData::list_available(level, job, QuestLog)` iterates `Check.img` children whose names are numeric, skips started/completed, then applies `meets_requirements`.

If Check.img is missing or filters never match, TAB0 is empty without crashing.
