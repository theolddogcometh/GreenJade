# Deck Top 50 — YYYY-MM-DD

| Field | Value |
|-------|--------|
| **HCL** | e.g. QEMU q35, virtio-gpu, virtio-input |
| **GreenJade** | git describe / version |
| **Runtime** | out-of-tree Proton-class build id |
| **Spec** | [GREENJADE_KERNEL_SPEC.md](../docs/GREENJADE_KERNEL_SPEC.md) §0.5.2 · [DESIGN_SPEC_COMPLETE.md](../docs/DESIGN_SPEC_COMPLETE.md) §8 |

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
| **NOT-TRIED** | Title never launched on GreenJade (default until real DUT + Steam client run) |
| PASS / PASS-OFFLINE / FAIL / BLOCKED-SECURITY | Only after an actual title attempt on GreenJade |

**Not result values (do not put these in the Result column):**

| Term | Meaning |
|------|---------|
| **READY** | Media/host bootstrap only (`steam/STATUS=READY`) — **not** a title result |
| Kernel **PASS** / gate **done** | Ship smoke (io_uring min rings, 768G soak, aarch64 scaffold, HDA, …) — does **not** change title rows |

**Hard rule:** never mark a row off **NOT-TRIED** from host stage/`STATUS=READY`, continuum soft gates, or kernel ship gates alone. Zero titles tried ⇒ **NOT-TRIED: 50**.

## Summary

| Result | Count |
|--------|------:|
| PASS | 0 |
| PASS-OFFLINE | 0 |
| BLOCKED-SECURITY | 0 |
| FAIL | 0 |
| NOT-TRIED | 50 |

**Claim level:** targeting only · majority (≥25) · met (≥40) · full (50)

## Clean-room notes

List any new syscalls/ioctls implemented for FAILs (clean-room only; no GPL paste).
