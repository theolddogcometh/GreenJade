# Deck Top 50 — YYYY-MM-DD

| Field | Value |
|-------|--------|
| **HCL** | e.g. QEMU q35, virtio-gpu, virtio-input — or real DUT from [HCL.md](../docs/HCL.md) |
| **GreenJade** | git describe / version |
| **Runtime** | out-of-tree Proton-class build id (only when used for a title try) |
| **Spec** | [GREENJADE_KERNEL_SPEC.md](../docs/GREENJADE_KERNEL_SPEC.md) §0.5.2 · [DESIGN_SPEC_COMPLETE.md](../docs/DESIGN_SPEC_COMPLETE.md) §8 |
| **Bar3 companion** | [STEAM_BAR3_STATUS.md](../docs/STEAM_BAR3_STATUS.md) · [STEAM_HWTEST.md](../docs/STEAM_HWTEST.md) |

## Vocabulary (do not conflate)

| Term | Means | Does **not** mean |
|------|--------|-------------------|
| **NOT-TRIED** | Title row: game never launched on GreenJade | Kernel/media gates unfinished |
| **READY** | Host/media bootstrap: stage tree `STATUS=READY` | Steam client ran, or any title PASS |
| **shipped / done / PASS** (kernel gates) | In-tree smoke or soft surface green | Bar3 client or Top-50 title claim |
| **Bar3** | Real-DUT path: Steam **client** launches; titles can leave NOT-TRIED | Media prep or continuum soft gates alone |

**Hard rule:** never mark a row off **NOT-TRIED** from host stage/`STATUS=READY`, continuum soft gates, or kernel ship gates alone (HDA, io_uring min rings, 768G soak, aarch64 scaffold, sshd, …). Zero titles tried ⇒ **NOT-TRIED: 50**.

## When to fill rows

Fill **Result** only after an actual title launch attempt **on GreenJade** (real DUT preferred; QEMU only if that is the stated HCL for the run).

1. Install / boot GreenJade on the recorded HCL.
2. Steam client runs (or fails in a documented way) — client open is separate from title rows.
3. Launch title via Proton personality; record PASS / PASS-OFFLINE / FAIL / BLOCKED-SECURITY + Notes + Owner.
4. Copy this template to `deck-top50-YYYY-MM-DD.md` (or update the active dated matrix). Do not invent PASS.

**Not a title try:** `steam-bar3-check.sh` READY, fetch/stage, kernel smokes, ICD host smoke, continuum makefile soft gates.

## Results

| Rank | Title / AppID | Result | Notes | Owner |
|------|---------------|--------|-------|-------|
| 1 | | NOT-TRIED | | |
| 2 | | NOT-TRIED | | |
| … | | | | |
| 50 | | NOT-TRIED | | |

**Result values:** `PASS` · `PASS-OFFLINE` · `BLOCKED-SECURITY` · `FAIL` · `NOT-TRIED`

| Result | Use when |
|--------|----------|
| **NOT-TRIED** | Title never launched on GreenJade (default until real DUT + Steam client run + title attempt) |
| **PASS** | Title launched and played acceptably on GreenJade (online path OK if required) |
| **PASS-OFFLINE** | Title playable offline only (or online blocked by env, not by GreenJade) |
| **FAIL** | Title attempted; crash, hang, missing ABI, or unplayable on GreenJade |
| **BLOCKED-SECURITY** | Attempt stopped by intentional security policy (cap, seccomp, landlock, …) |

**Not result values (do not put these in the Result column):**

| Term | Meaning |
|------|---------|
| **READY** | Media/host bootstrap only (`steam/STATUS=READY`) — **not** a title result |
| Kernel **PASS** / gate **done** | Ship smoke — does **not** change title rows |

## Summary

| Result | Count |
|--------|------:|
| PASS | 0 |
| PASS-OFFLINE | 0 |
| BLOCKED-SECURITY | 0 |
| FAIL | 0 |
| NOT-TRIED | 50 |

**Claim level:** targeting only · majority (≥25 PASS-family) · met (≥40) · full (50)

| Claim | Meaning |
|-------|---------|
| **targeting only** | Zero or sparse tries; product aim stated; **default until real runs** |
| **majority** | ≥25 rows PASS or PASS-OFFLINE from real launches |
| **met** | ≥40 |
| **full** | 50 |

Do **not** promote claim level from media READY or kernel ship gates alone.

## Clean-room notes

List any new syscalls/ioctls implemented for FAILs (clean-room only; no GPL paste). Kernel-gate diaries belong in IMPLEMENTATION / bar3 docs — not as invented title PASS rows.
