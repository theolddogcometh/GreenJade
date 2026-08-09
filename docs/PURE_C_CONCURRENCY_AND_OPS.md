# Pure-C concurrency and microarchitecture ops (house rules)

**Policy only.** Soft≠product. Dual **MIT OR Apache-2.0**. Not a product bar close.  
Greppable: **`PURE_C_CONCURRENCY`** · **`HALL_OF_SHAME`** · **`Soft!=product`**

Codifies lessons from:

| Source | What we take |
|--------|----------------|
| **CppCon / Fedor Pikus** lock-free talks | Measure; high contention → short spinlocks beat CAS storms; acquire/release; false sharing; atomic ≠ lock-free; data race = UB |
| **LaurieWired Tailslayer** | Tail-latency *mental model* only — **do not** port C++ libraries. Channel striping → [MEM_PLACE_CHANNEL.md](MEM_PLACE_CHANNEL.md) (Option A soft residual landed; product L2 OPEN) |
| **xoreaxeaxeax Rosenbridge** | Platform is **G752-class Intel**, not VIA C3. Lesson = **no exploratory MSR**; document every MSR touch; keep attack surface small |
| **asm hall of shame** | Hot-path AVOID list for cache, fence, MMIO, and entropy spam |

Related: [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) (fail-closed revoke / S1–S2) · [spinlock.h](../kernel/include/gj/spinlock.h) · DMA sync in [dma_buf.h](../kernel/include/gj/dma_buf.h).

---

## 1. Pikus-aligned concurrency rules

1. **Measure before clever.** Contended micro-opts without a counter/profile are cargo cult. Soft residual counters OK; product claims need DUT evidence.
2. **High contention: prefer short spinlocks over CAS storms.** A ticket-free TAS with **read-spin then exchange** (see `gj_spin_lock`) reduces store traffic on the lock line vs blind CAS loops.
3. **Acquire / release, not full barrier on every access.** Lock word: `__ATOMIC_ACQUIRE` on take, `__ATOMIC_RELEASE` on drop. Do not `mfence` after every store “for safety.”
4. **False sharing is real.** Hot shared counters next to locks, or locks next to hot data, thrash remote CPUs. Use **`SPIN_ALIGNED`** / `struct gj_spinlock_aligned` (cacheline pad) when the lock is shared across cores on a hot path.
5. **`atomic` ≠ lock-free algorithm.** `__atomic_*` gives single-word RMW and ordering. Multi-word protocols still need locks, epochs, or careful publish/subscribe — or they are **data races** (UB in C11).
6. **Data race = UB.** Two threads, same non-atomic object, concurrent write or write+read without synchronization → undefined. Not “probably fine on x86.”
7. **Short critical sections only.** Never block, sleep, or schedule while holding a spinlock ([SECURITY_CORE_DESIGN](SECURITY_CORE_DESIGN.md) §4 / spinlock header rules).
8. **IRQ sharing → irqsave.** If the same lock is taken from IRQ context, use `gj_spin_lock_irqsave` / `gj_spin_unlock_irqrestore` (or try variant). Nested save/restore is LIFO and caller-managed.

### Map to knano

| Concept | knano surface |
|---------|----------------|
| Short TAS + read-spin | `gj_spin_lock` in `kernel/include/gj/spinlock.h` |
| Acquire/release | `__atomic_compare_exchange_n(..., __ATOMIC_ACQUIRE)` / `__atomic_store_n(..., __ATOMIC_RELEASE)` |
| False-sharing pad | `SPIN_ALIGNED` · `struct gj_spinlock_aligned` · `GJ_SPINLOCK_ALIGNED_INIT` |
| Soft probe (not ownership) | `gj_spin_is_locked` (relaxed) — debug only |
| IRQ-safe acquire | `SPIN_IRQSAVE` · `gj_spin_lock_irqsave` · `SPIN_TRY_IRQSAVE` |
| Relax under contention | `gj_cpu_relax()` (`pause` / `yield`) · `GJ_SPIN_RELAX_ITERS` |
| Compiler fence around CS | `gj_compiler_barrier()` — not a substitute for acquire/release on the lock word |

**Do not** change the lock algorithm without a measured bug and a pure-C fix. Soft≠product.

---

## 2. Asm hall of shame (hot paths) — `HALL_OF_SHAME`

**AVOID on hot paths** (serial init / one-shot bring-up may differ; still prefer narrow tools):

| Pattern | Why it hurts |
|---------|----------------|
| **`wbinvd`** | Whole-cache invalidate; multi-ms stalls; multi-CPU nightmare. Never for DMA publish. |
| **`clflush` spam** | Flush only dirty lines you own at the publish edge; not every dword write. |
| **`mfence` after every store** | Kills store buffering; one fence (or release) at the **visibility edge**. |
| **Wide / unaligned vector MMIO** (`vmovdqu` / `vmovaps` into BARs) | Device may fault, split, or snoop badly; prefer dword/qword MMIO. |
| **Split-lock atomics** | Locked RMW across cache-line or page boundary → huge bus lock / #AC on modern Intel. Align lock words and atomics. |
| **`rdrand` / `rdseed` spam** | Serializing, rate-limited; not a PRNG fill loop. Seed once, expand in software if needed. |
| **Hot `cpuid` / `rdmsr`** | Serialize and trap cost; cache feature bits / MSR values at boot or first use. |
| **Multi-CPU BAR hammer** | Concurrent uncoordinated MMIO to the same BAR → lost updates, device wedging. **One MMIO owner CPU** (or strict doorbell protocol). |
| **`fxrstor` from MMIO** | FPU restore must be from RAM, not device memory. |

### Prefer

- **Dword (or natural-width) MMIO** for device registers.
- **Narrow `clflush`** of dirty lines at DMA publish / OWN handoff (see `dma_clflush_range` + `dma_buf_sync_*`).
- **One fence at the visibility edge** (`mfence` or release store), not per-store.
- **One MMIO owner CPU** per device (or documented striping with ownership transfer).
- **Documented MSR touches only** — no exploratory read/write loops (Rosenbridge lesson).

Audit helper: **`./scripts/audit-ops-shame.sh`** (informational; not a hard CI fail by default).

---

## 3. Tailslayer (policy only)

- **Mental model:** average latency can look fine while **tail** (p99/p999) kills interactive / network DoD. Think about queueing, lock hold time, and cross-core invalidations.
- **Hedging / multi-path libraries** are userspace/HPC patterns — **do not port** Tailslayer or other C++ latency libs into the freestanding kernel.
- **Channel striping / mem place** is owned by **[MEM_PLACE_CHANNEL.md](MEM_PLACE_CHANNEL.md)** (Option A: `CREATE_PLACED` 104 / `MAP_REPLICA` 105; no PA leak).
- Soft residual L0 striping ≠ product DRAM channel map (L2 OPEN).

---

## 4. Rosenbridge (policy only)

- Target laptop class is **G752VT-class Intel**, **not** VIA C3 (Rosenbridge’s deep debug-port story).
- **Lesson still applies:** undocumented or exploratory **MSR** poking expands attack surface and brick risk.
- **Rules:** no exploratory MSR; every product/eng MSR touch is **documented** (why, which, when); prefer feature bits cached after one `cpuid`/`rdmsr` at bring-up.
- Small attack surface aligns with [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) fail-closed revoke (DEAD/gen first; use paths fail closed). Soft≠product.

---

## 5. Pure-C coding checklist (PR review)

Use on concurrency, DMA, MMIO, and irqsave patches:

- [ ] Pure C11; dual **MIT OR Apache-2.0**; Hungarian locals; no OOP / no GPL.
- [ ] Shared data: lock, atomics with stated memory order, or single-owner — **no** plain races.
- [ ] Spinlocks: short CS; no sleep/block under lock; irqsave if IRQ shares the lock.
- [ ] Contended shared locks: consider **`SPIN_ALIGNED`**; measure if claiming a win.
- [ ] Acquire on lock / release on unlock; no “mfence everywhere.”
- [ ] DMA publish: narrow dirty-line flush + **one** visibility fence — **no `wbinvd`**, no clflush spam.
- [ ] MMIO: dword-width preferred; one owner CPU; no vector stores to BARs on hot path.
- [ ] Atomics: natural alignment; no split locks.
- [ ] No hot `cpuid`/`rdmsr`/`rdrand` loops; no exploratory MSR.
- [ ] Soft lamps / soft residual ≠ product DoD close (**Soft!=product**).
- [ ] Fail-closed on security paths where applicable ([SECURITY_CORE_DESIGN](SECURITY_CORE_DESIGN.md)).
- [ ] Run `./scripts/audit-ops-shame.sh` if touching asm fences, clflush, or MMIO.

---

## 6. Audit script usage

```sh
# From repo root — informational report (default exit 0 unless -f)
./scripts/audit-ops-shame.sh

# Optional: treat selected severe patterns as hard fail
./scripts/audit-ops-shame.sh --fail-on-wbinvd

# Help
./scripts/audit-ops-shame.sh --help
```

Patterns scanned (see script header): `wbinvd`, `rdrand`/`rdseed`, `vmovdqu`/`vmovaps` near mmio-ish names, `fxrstor`, `clflush` (count/report), `lock`/`__atomic` alignment note.

---

## 7. Honesty

- **Soft!=product.** Codified lessons ≠ product bars closed (Bar3, Dual DoD A/B, Steam Top-50).
- Do **not** claim product latency or lock-free completeness from this document alone.
- Do **not** implement Tailslayer mem-place syscalls here; other agents own that track.

**greppable:** `PURE_C_CONCURRENCY` `HALL_OF_SHAME` `Soft!=product` `SPIN_ALIGNED` `SPIN_TAS` `SPIN_IRQSAVE`
