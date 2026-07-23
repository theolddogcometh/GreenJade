# Steam bar3 status (honest product ceiling)

| Field | Value |
|-------|--------|
| **Date** | 2026-07-23 |
| **Companion** | [STEAM_HWTEST.md](STEAM_HWTEST.md) · [HCL.md](HCL.md) · [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) |

**Bar3** = real-DUT path where Steam **client** launches and Deck Top 50 titles can leave `NOT-TRIED`.  
Media prep, kernel smokes, and continuum soft gates are **not** bar3 completion.

---

## READY vs NOT-TRIED (do not conflate)

| Term | Scope | Current meaning |
|------|--------|-----------------|
| **READY** | Host / media bootstrap | `build/steam-stage/steam/STATUS` or `GJ-PERSIST/steam/STATUS` = **READY** when a prebuilt Steam tree is staged or packed |
| **SKELETON** | Media | Placeholder tree without full bootstrap extract |
| **MISSING** | Host soft check | No stage tree yet (`scripts/steam-bar3-check.sh`) |
| **NOT-TRIED** | **Matrix title rows only** | No Deck Top 50 title has been launched on GreenJade |

| Claim | Allowed? |
|-------|----------|
| “Steam tree is READY on media” | Yes, when stage/fetch succeeded |
| “Bar3 done because STATUS=READY” | **No** |
| “Top-50 PASS because kernel smokes green” | **No** |
| “Titles tried” without a client launch | **No** — rows stay **NOT-TRIED** |

**Hard rule:** never mark Top-50 rows `PASS` (or off `NOT-TRIED`) from host stage/`STATUS=READY`, continuum soft gates, or kernel ship gates alone.

---

## Kernel / media ship gates (current — not title claims)

| Gate | Status | Honesty bound |
|------|--------|---------------|
| Steam option 2+3 media path | **wired** | fetch / stage / host-prep / hwtest pack |
| Host soft check | **done** | `steam-bar3-check.sh` → READY \| SKELETON \| MISSING |
| Product **sshd** live | **done** | boot spawn TCP :22 |
| **scsi_mid** live embed | **done** | virtio-scsi path |
| **HDA multi-stream** | **kernel PASS** | not Steam audio / not game PCM |
| **io_uring min rings** | **shipped PASS** | `io_uring: min rings ready`; setup/enter/register soft surface; **not** full SQE for games |
| **768GiB hierarchical soak** | **PASS** | `GJ_MEM=768G` / `soak_tib`; product bar still **≥ 1 TiB** when host allows; **not** Steam |
| **aarch64 M0 scaffold** | **smoke PASS** | `make aarch64` / `aarch64-smoke` (exceptions→virtio-mmio); separate arch tree; **not** Deck/x86 product path |
| Continuum soft graph | **soft** | parallel waves → **makefile_max=14900**; CREATE-ONLY soft gates only; **not** client run |

---

## Open (bar3 client run — do not claim done)

| Item | Status |
|------|--------|
| Real-DUT UEFI install + boot to shell/sshd | **open** |
| Launch Steam client on GreenJade | **open** |
| Steam Runtime first-run / net + ABI | **open** |
| GPU / audio / input for client & games | **open** (kernel surfaces partial only) |
| Full 1 TiB host soak | **open** (separate from bar3) |
| **Deck Top 50** title rows | **NOT-TRIED** × 50 — claim level **targeting only** |

No bar3 titles have been tried. Matrix remains **NOT-TRIED: 50**.

---

## Operator check (media only)

```sh
./scripts/steam-bar3-check.sh          # READY | SKELETON | MISSING
# on stick:
# cat /mnt/gj-persist/steam/STATUS     # READY or SKELETON
```

When client path unblocks: fill [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) from real runs only.

## Related

- [STEAM_HWTEST.md](STEAM_HWTEST.md) — options 2+3 layout and commands  
- [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) — Deck Top 50 product surface  
- [IMPLEMENTATION.md](IMPLEMENTATION.md) — coding baseline + progress notes  
