# Steam bar3 status (honest product ceiling)

| Field | Value |
|-------|--------|
| **Date** | 2026-07-23 |
| **Wave** | GreenJade Wave 24 (honesty refresh) |
| **Bar3** | **OPEN** |
| **Deck Top 50** | **NOT-TRIED × 50** (no title PASS) |
| **Soft inventory** | **READY** (host media only — `./scripts/steam-bar3-check.sh`) |
| **Soft continuum** | **≠ bar3** (CREATE-ONLY graph wire; high-water **advancing toward 16600** parent wires; honest scan may still report **makefile_max=16500** until parent wires; not client run) |
| **Product lamps** | **0** (soft ≠ product complete) |
| **Companion** | [STEAM_HWTEST.md](STEAM_HWTEST.md) · [HCL.md](HCL.md) · [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) |

**Bar3** = real-DUT path where Steam **client** launches and Deck Top 50 titles can leave `NOT-TRIED`.  
Media prep, kernel smokes, and continuum soft gates are **not** bar3 completion.

**Soft stamp (2026-07-23 / Wave 24):** host media **READY** (`build/steam-stage/steam/STATUS`, tree/stage/rootfs hits agree). Continuum soft high-water **advancing toward 16600** (parent wires) — **soft only**. Honest `makefile_max` is a Makefile scan (verify `./scripts/gj-continuum-makefile-snippet.sh --max`); **do not hardcode false 16600** if scan still reports prior tip (**16500**).  
**Prior tips (historical):** Wave 23 toward 16500 / soft high-water 16500 when wired; Wave 18 toward 16000 / soft high-water 16000 when wired; Wave 17 toward 15900; Wave 16 toward 15800; Wave 15 toward 15700; Wave 14 toward 15600; Wave 13 soft high-water 15500.  
**Hard stamp:** bar3 is **OPEN**. Matrix remains **NOT-TRIED: 50**. Product lamps **0**. No title PASS invented from host scripts.  
**Media READY ≠ client run ≠ Top-50.** Soft continuum (toward **16600**, prior tip **16500**, parallel waves) **≠ bar3**. **Soft ≠ product complete.**

---

## READY vs NOT-TRIED (do not conflate)

| Term | Scope | Current meaning |
|------|--------|-----------------|
| **READY** | Host / media bootstrap | `build/steam-stage/steam/STATUS` or `GJ-PERSIST/steam/STATUS` = **READY** when a prebuilt Steam tree is staged or packed |
| **SKELETON** | Media | Placeholder tree without full bootstrap extract |
| **MISSING** | Host soft check | No stage tree yet (`scripts/steam-bar3-check.sh`) |
| **NOT-TRIED** | **Matrix title rows only** | No Deck Top 50 title has been launched on GreenJade |
| **host-prep / fetch / stage PASS** | Lab-host scripts only | Extract, copy, or pack succeeded — **not** client run |
| **soft continuum** | libcgj graph parent wire only | CREATE-ONLY freestanding TUs / makefile lines — **not** bar3 |

| Claim | Allowed? |
|-------|----------|
| “Steam tree is READY on media” | Yes, when stage/fetch succeeded |
| “Bar3 done because STATUS=READY” | **No** |
| “Top-50 PASS because kernel smokes green” | **No** |
| “Titles tried” without a client launch | **No** — rows stay **NOT-TRIED** |
| “Bar3 closed by continuum / io_uring / 768G / aarch64” | **No** |
| “Soft continuum makefile_max closes bar3” | **No** — continuum ≠ bar3 |

**Hard rule:** never mark Top-50 rows `PASS` (or off `NOT-TRIED`) from host stage/`STATUS=READY`, continuum soft gates, or kernel ship gates alone. **No false PASS.**

---

## Media path scripts (soft surface — not bar3)

Strict host path for option 2+3 (no DUT client claim):

| Script | Role | What PASS / READY means | What it does **not** mean |
|--------|------|-------------------------|---------------------------|
| `scripts/fetch-steam-bootstrap.sh` | Host fetch + extract Valve bootstrap `.deb` | Tree under `build/steam-tree/` + `READY` stamp | Client ran; Runtime downloaded; titles tried |
| `scripts/stage-steam-tree.sh` | Stage into `build/steam-stage` (+ optional rootfs) | `STATUS=READY` or `SKELETON` written | Client ran; bar3 closed |
| `scripts/steam-host-prep.sh` | Option 2 pack / option 3 copy onto img/USB/mount | Host copy or image pack succeeded | Client ran; Top-50 PASS |
| `scripts/steam-bar3-check.sh` | Soft media inventory (always exit 0) | Prints READY \| SKELETON \| MISSING | smoke-all green; bar3 done |

Soft inventory (agent honesty) includes: file counts (capped), launcher kind, bootstrap blob presence, MANIFEST keys, STATUS multi-layout agreement, `STAGE_META.txt` / `HOST_PREP_META.txt` when present.

```sh
./scripts/steam-bar3-check.sh
# → steam-bar3-check: READY|SKELETON|MISSING
# → always ends with bar3: OPEN
```

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
| Continuum soft graph | **soft** | parallel waves → high-water **advancing toward 16600** (parent wires; honest scan may still be **16500**); CREATE-ONLY soft gates only; product lamps **0**; **soft continuum ≠ bar3**; **soft ≠ product complete**; **not** client run |

---

## Open (bar3 client run — do not claim done)

| Item | Status |
|------|--------|
| Real-DUT UEFI install + boot to shell/sshd | **open** |
| Launch Steam client on GreenJade | **open** |
| Steam Runtime first-run / net + ABI | **open** |
| GPU / audio / input for client & games | **open** (kernel surfaces partial only) |
| Full 1 TiB host soak | **open** (separate from bar3) |
| Soft continuum (graph wire only) | **soft** — **not** a bar3 closer |
| **Deck Top 50** title rows | **NOT-TRIED** × 50 — claim level **targeting only** |

No bar3 titles have been tried. Matrix remains **NOT-TRIED: 50**.  
**Bar3 is OPEN.** Soft continuum ≠ bar3. No false PASS.

---

## Operator check (media only)

```sh
./scripts/fetch-steam-bootstrap.sh     # host extract only
./scripts/stage-steam-tree.sh          # STATUS=READY|SKELETON
./scripts/steam-bar3-check.sh          # soft inventory; exit 0; bar3 OPEN
# on stick:
# cat /mnt/gj-persist/steam/STATUS     # READY or SKELETON
# cat /mnt/gj-persist/steam/STAGE_META.txt   # soft honesty stamp if staged
```

When client path unblocks: fill [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) from real runs only.

## Related

- [STEAM_HWTEST.md](STEAM_HWTEST.md) — options 2+3 layout and commands  
- [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) — Deck Top 50 product surface  
- [IMPLEMENTATION.md](IMPLEMENTATION.md) — coding baseline + progress notes  
