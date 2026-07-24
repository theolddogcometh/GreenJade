# GreenJade — Design Spec Completion (Accepted Freeze)

| Field | Value |
|-------|--------|
| **Document** | Design specification complete **v1.0** |
| **Status** | **Accepted** — closes remaining design gaps under project law; 17 Wave 49 honesty only |
| **Does not** | Implement every feature in code; freezes **what** to build |
| **Law** | Security → Performance → Portability → Readability · pure C in-tree · **no GPL/copyleft source** · clean-room OK · dual MIT OR Apache-2.0 |
| **Adoption** | **Deck Top 50** ([GREENJADE_KERNEL_SPEC.md](GREENJADE_KERNEL_SPEC.md) 0.5.2) |
| **ABI** | Clean-room Linux-compatible via Option C ([LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md)) |
| **Precedence** | Law → Security core → Cap addressing → **Proton** → Apple → Solaris → this freeze (fills holes only) |
| **Honesty (Wave 49)** | Soft freezes (Accepted design) ≠ product multi-server / bar3; continuum **toward 19100** soft only; product lamps **0**; **no product claim** (17) |

This document is the **authoritative freeze** for topics previously listed as open, underspecified, or scaffold-only. Clean-room: reimplement behavior from public docs/manuals; never paste Linux/Wine/GPL trees into GreenJade.

---

## 0. Document index (normative)

| Doc | Authority |
|------|-----------|
| [GREENJADE_KERNEL_SPEC.md](GREENJADE_KERNEL_SPEC.md) | Project law, product bars, milestones |
| [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) | Caps, revoke, IPC, SMP, quotas, time |
| [CAP_ADDRESSING.md](CAP_ADDRESSING.md) | Scheme A handles, root meta, fault cookie |
| [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) | Deck Top 50, libprotonrt, game task model |
| [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) | Option C hot/cold, SYSCALL, ring-3 entry |
| [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) | VM objects/views, task ports, QoS, session |
| [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) | Untyped, CDT, cookie map rules, HCL tiers |
| [X86_64_INTEL_PLATFORM.md](X86_64_INTEL_PLATFORM.md) | UEFI, VT-d, x2APIC, TSC, large pages |
| [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) | Userspace driver surface |
| [IMPLEMENTATION.md](IMPLEMENTATION.md) / [TODO.md](TODO.md) | Build phases (subordinate to design) |
| **This document** | Remaining freezes (isolation, traps, doors v1, AC, matrix, locks, …); 17 Wave 49 honesty only |

Conflict: **higher authority wins**; this file only decides topics those docs left open.

---

## 1. Hard gates (security-first)

### 1.1 IDT before untrusted ELF (**P0**)

| Rule | Detail |
|------|--------|
| **G-IDT-1** | No **untrusted** ring-3 ELF (beyond signed/owned trampoline tests) until IDT + exception stubs exist. |
| **G-IDT-2** | On exception: dump vector, error code, RIP, CS, CR2 (if #PF), current thread/process id; then **kill faulting thread** or halt if kernel fault. |
| **G-IDT-3** | User #PF → existing pager/cookie path when registered; else kill thread (fail closed). |
| **G-IDT-4** | Double-fault / NMI: halt with dump (no silent reboot loop). |

### 1.2 Kernel mappings supervisor-only (**P0**)

| Rule | Detail |
|------|--------|
| **G-MAP-1** | All **kernel** text/data/rodata/bss and kernel-only heap **must** be mapped with **U=0** (supervisor-only). |
| **G-MAP-2** | User mappings (U=1) only in `[GJ_USER_VA_BASE, GJ_USER_VA_END)` (or future documented user half). |
| **G-MAP-3** | Identity-map bring-up may temporarily share low PA; before product ring-3: **audit** and clear U on kernel pages; enable **SMEP** (and **SMAP** when copy_from_user uses STAC/CLAC). |
| **G-MAP-4** | Boot 2 MiB splits must not leave kernel pages user-executable. |

### 1.3 User pointer validation (**P0** — restated)

| Rule | Detail |
|------|--------|
| **G-PTR-1** | Every Linux/native syscall that touches user memory uses `copy_from_user` / `copy_to_user` (or equivalent) under the **current** AS. |
| **G-PTR-2** | Kernel-pointer bypass for smoke is **debug-only** (`#if GJ_DEBUG`) and **off** for product builds. |
| **G-PTR-3** | Failed access → `-EFAULT` / `GJ_ERR_FAULT`, never kernel panic for user addresses. |

---

## 2. Address spaces (true multi-process)

### 2.1 Model (**P0**)

```text
Each PROCESS has:
  u64Cr3 → PML4
  kernel half: same physical kernel pages, U=0, shared mappings
  user half: private (or shared via memory objects)
```

| Rule | Detail |
|------|--------|
| **G-AS-1** | `process.u64Cr3 == 0` means “not yet own AS”; first user map or `as_create` installs a **cloned** PML4. |
| **G-AS-2** | Clone: copy kernel PML4 entries only; empty user entries. |
| **G-AS-3** | On schedule: `cpu_load_cr3(pProc->u64Cr3)` if non-zero. |
| **G-AS-4** | TLB shootdown on multi-CPU when unmap/protect (IPI); UP: `invlpg` / full reload OK. |
| **G-AS-5** | Kernel never relies on user CR3 for kernel correctness (no user-mapped kernel data). |

### 2.2 Layering: vmm vs memory objects (**P0**)

| Layer | Role |
|-------|------|
| **PMM** | Physical frames |
| **vmm** | Arch PTE install/remove/protect (hardware) |
| **memory_object** | Ownership, share, pager, quota, CDT |
| **region** | VA range in a space → object + rights |

| Rule | Detail |
|------|--------|
| **G-MO-1** | Anon mmap creates/uses an **anon memory object** then `vmm_map_page` frames owned by that object. |
| **G-MO-2** | File mmap: file memory object + pager (vfs); hot path may defer file mmap to cold/personality until object wired. |
| **G-MO-3** | Shared wine maps = **one object**, multiple process maps; futex shared key = **PA of word** (Linux-true). |
| **G-MO-4** | Bring-up may map PMM frames without object bookkeeping only inside `GJ_DEBUG` early tests. |

---

## 3. Linux personality & Option C (production shape)

### 3.1 Personality TCB (**P0**)

| Phase | Who runs cold path | Allowed? |
|-------|--------------------|----------|
| **Bring-up** | Kernel thread `cold_personality_server` + doors | Yes, **temporary** |
| **Product** | **Userspace** `libprotonrt` / `protonrt-server` process | **Required** |
| **Forbidden** | Growing large FS/net policy in kernel “for speed” | Yes forbidden |

| Rule | Detail |
|------|--------|
| **G-PERS-1** | Kernel may only implement **hot** syscalls listed in [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md); all else doors to personality. |
| **G-PERS-2** | Production personality holds only caps: COLD door, vfs client, optional net client — **no** IOMMU/IRQ. |
| **G-PERS-3** | Personality death → in-flight cold calls get `PEER_DEAD` / `-EIO`; Linux tasks may be killed by policy. |

### 3.2 Single cold-path API (**P0**)

| Rule | Detail |
|------|--------|
| **G-COLD-1** | **Normative:** cold syscalls use **doors** (`door_call` / `door_recv` / `door_reply`). |
| **G-COLD-2** | Queue + `GJ_SYS_COLD_DEQUEUE/REPLY` are **deprecated scaffolds**; remove after userspace door server lands. |
| **G-COLD-3** | Client blocks (scheduler) until reply; no busy-spin in product builds. |

### 3.3 CapJit as a real right (**P0**)

| Rule | Detail |
|------|--------|
| **G-JIT-1** | JIT authority is `GJ_RIGHT_JIT` on a PROCESS (task) cap or a dedicated sealed grant from launcher. |
| **G-JIT-2** | `mprotect`/`mmap` W\|X allowed **only** if current process holds JIT. |
| **G-JIT-3** | Launcher grants JIT to Proton game tasks only; revoke on process death with 1.1. |
| **G-JIT-4** | Boolean `u32Jit` in PCB is a **cache** of the cap check, not the authority source (migrate). |

### 3.4 Credentials & uname (**P1**)

| Rule | Detail |
|------|--------|
| **G-CRED-1** | v1 desktop: **single-user** personality; fake uid/gid **1000** (or 0 only for sealed system tasks). |
| **G-CRED-2** | Authority remains **caps**, not UID. UID exists for Linux ABI compatibility only. |
| **G-UNAME-1** | Linux personality: `sysname=Linux`, `release`/`version` include `greenjade` for support. |
| **G-UNAME-2** | Native GJ tasks may report `GreenJade` if ever exposed. |

### 3.5 Futex timeouts (**P1**)

| Rule | Detail |
|------|--------|
| **G-FUT-1** | Shared key = PA of word; private = VA (Linux). |
| **G-FUT-2** | Absolute/relative timeout via mono clock; timer wheel or tick wakes waiter with `-ETIMEDOUT`. |
| **G-FUT-3** | No spin loops in product; only `thread_block` + timer. |

---

## 4. Doors / IPC production model

### 4.1 Scaffold → capability doors (**P0**)

| Rule | Detail |
|------|--------|
| **G-DOOR-1** | Production door = **ENDPOINT** (or DOOR type alias) in CNode with Call/Recv/Reply. |
| **G-DOOR-2** | `gj_door` bare struct is **v0**; must gain obj_hdr, DEAD/gen, CDT, quota. |
| **G-DOOR-3** | Cap transfer on doors follows SOLARIS remaining (receiver-allocated slots, small K). |
| **G-DOOR-4** | Server death: clients `PEER_DEAD`; revoke endpoint 1.1. |

### 4.2 Sync IPC + bulk rings

Unchanged: doors for control; SPSC rings for bulk ([SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) 3, 6).

---

## 5. Scheduler, time, SMP locks

### 5.1 Idle & preemption (**P0/P1**)

| Rule | Detail |
|------|--------|
| **G-SCHED-1** | Idle thread: when no RUNNABLE, **HLT** (with IRQs enabled), not forever-`pause`. |
| **G-SCHED-2** | Timer tick (APIC) drives preemption and futex/IPC timeouts. |
| **G-SCHED-3** | v1: cooperative + tick preemption; full CFS-like not required. |
| **G-SCHED-4** | QoS classes per Apple channel; PI on doors per security core (implement with turnstile after endpoints). |

### 5.2 Lock order (UP → SMP) (**P1**)

| Rank (low → high acquire) | Locks |
|---------------------------|--------|
| 1 | Per-CPU runqueue |
| 2 | Thread block object (futex bucket / door) |
| 3 | Process / CNode |
| 4 | Endpoint / door object |
| 5 | Space fault / Space map |
| 6 | PMM zone |

| Rule | Detail |
|------|--------|
| **G-LOCK-1** | Documented order; debug builds assert. |
| **G-LOCK-2** | Never hold runqueue lock across door Call into userspace. |
| **G-LOCK-3** | UP-only code paths must be marked; product SMP requires locks before AP online. |

---

## 6. Process lifecycle (spawn / wine-server)

| Rule | Detail |
|------|--------|
| **G-PROC-1** | `spawn`: create PROCESS, AS (CR3), CNode, root meta, first thread, cap bundle, quota slice. |
| **G-PROC-2** | Parent receives **PROCESS** task cap (not root meta). |
| **G-PROC-3** | Wine-server = second process; shared memory objects mapped into both. |
| **G-PROC-4** | No ambient fork of full CNode (security core). |
| **G-PROC-5** | Death: clear pager, revoke CNode, unmap AS, refund quota (1.1). |

---

## 7. Anti-cheat (Deck Top 50 honesty)

| Rule | Detail |
|------|--------|
| **G-AC-1** | **No** Linux `.ko` / kernel-module AC. |
| **G-AC-2** | Optional **userspace AC agent** process: limited PROCESS INSPECT on game task only; no seat/GPU MMIO. |
| **G-AC-3** | Titles requiring kernel AC with no userspace path → **BLOCKED-SECURITY** or **PASS-OFFLINE** if SP works. |
| **G-AC-4** | Never weaken 0.0 for multiplayer catalog size. |

---

## 8. Deck Top 50 matrix artifact (**P0** process)

| Rule | Detail |
|------|--------|
| **G-MAT-1** | Path: `matrix/deck-top50-YYYY-MM-DD.md` (or `.csv`) in-tree or release notes. |
| **G-MAT-2** | Columns: rank, appid/title, result (PASS/PASS-OFFLINE/BLOCKED-SECURITY/FAIL/NOT-TRIED), HCL, notes, owner. |
| **G-MAT-3** | Claims majority/met/full only against a **dated** snapshot per arch 0.5.2. |
| **G-MAT-4** | FAIL with “missing Linux syscall N” → clean-room implement in hot or personality; never GPL paste. |

Template: see 15.

---

## 9. Platform & drivers (clean-room)

| Topic | Decision |
|-------|----------|
| **Libc** | Prefer **original clean-room** + optional **MIT/Apache-only** imported pieces with provenance; no glibc GPL. |
| **Netstack** | **Original clean-room** userspace netstackd first; no Linux networking stack source. |
| **Clean-room wall** | Reader/implementer split when studying GPL for ideas; CI rejects GPL license strings; `third_party/PROVENANCE.md` required for any import. |
| **GPU** | virtio-gpu T0 → clean-room ICD + UDX host; DRM ioctl shims only if matrix requires (clean-room). |
| **IOMMU** | Production: no bus-master without window (Intel platform). |
| **UEFI** | Product boot; Multiboot interim deleted by UEFI milestone. |
| **PMM** | Replace 4 GiB bitmap before 1 TiB DoD. |

---

## 10. Quotas / ledger (**P1** restated)

| Rule | Detail |
|------|--------|
| **G-Q-1** | Every creatable kernel object charges process ledger. |
| **G-Q-2** | Spawn takes sub-slice; cannot exceed parent remaining. |
| **G-Q-3** | Threads, slots, pages, endpoints, timers, IRQ binds all charged. |
| **G-Q-4** | Soft jetsam optional; hard `GJ_ERR_QUOTA` is v1 default. |

---

## 11. Graphics / input / audio (A1 path)

| Rule | Detail |
|------|--------|
| **G-GFX-1** | Vulkan ICD → GPU host (UDX) → compositor present; export handles gen-checked. |
| **G-GFX-2** | Session filters input; game capture grant for focus. |
| **G-GFX-3** | Audio stream client; soft-RT QoS for callback thread. |
| **G-GFX-4** | T0 HCL = QEMU virtio-gpu + virtio-input + virtio-snd (or serial fallback). |

---

## 12. Implementation dependency graph (normative order)

```text
IDT + supervisor-only maps + SMEP
  → true AS clone (CR3)
  → spawn + PROCESS cap
  → userspace personality doors (replace kernel thread)
  → memory objects on all mmap
  → timer + idle HLT + futex timeouts
  → vfs + net cold paths for matrix
  → virtio-gpu present (A1)
  → Deck Top 50 matrix A2/A3
  → UEFI + 1TiB PMM + SMP locks/APs (parallel once UP stable)
```

---

## 13. Explicit non-goals (reaffirmed)

| Non-goal |
|----------|
| Import GPL/LGPL/AGPL **source** into GreenJade tree |
| Load Linux **`.ko`** |
| SteamOS **distro port** as GreenJade |
| Ambient root / full fork of CNode |
| OOP / non-C in-tree runtime |
| Reordering priorities for games |
| Kernel streaming stacks / in-kernel class drivers |

---

## 14. Resolved former “open questions”

| Question | Decision |
|----------|----------|
| License | **MIT OR Apache-2.0** dual |
| Linux ABI | **Allowed clean-room**; depth = Deck Top 50 |
| Libc | Clean-room + optional MIT/Apache pieces only |
| Netstack | Clean-room original userspace first |
| Clean-room wall | Process in 9 + CI + provenance |
| Personality location | Kernel thread bring-up → **userspace doors** product |
| Cold API | **Doors only** (production) |
| Futex share | **Physical address** of word |
| Major games | **Deck Top 50** |

---

## 15. Matrix template

```markdown
# Deck Top 50 — YYYY-MM-DD

HCL: QEMU q35 -smp N -m M + virtio-gpu | or: hardware XYZ
GreenJade: <git describe>
Runtime: <proton-class out-of-tree version>

| Rank | Title / AppID | Result | Notes | Owner |
|------|---------------|--------|-------|-------|
| 1 | … | NOT-TRIED | | |
…
## Summary
PASS: 0  PASS-OFFLINE: 0  BLOCKED-SECURITY: 0  FAIL: 0  NOT-TRIED: 50
Claim: targeting only
```

---

## 16. Completion statement

With this freeze, **design** for GreenJade’s product path is **complete enough to implement without further architectural ambiguity** on:

isolation (AS, maps, IDT), hybrid Linux ABI production shape, personality TCB, doors, CapJit, futex, cold path, anti-cheat honesty, Deck Top 50 process, quotas, graphics A1, clean-room IP, and SMP lock direction.

Remaining work is **coding and matrix execution**, not open-ended design choice—except measured perf refinements that do not violate 0.0.

---

## 17. Honesty bounds — soft freezes ≠ product multi-server / bar3 (Wave 49 · 2026-07-23)

**Additive only (Wave 49 exclusive for this file).** Freezes and decisions in 0–16 stay **Accepted**. This section is a Wave 49 honesty ledger: what **design freezes** and greppable **soft** surfaces mean vs what remains **open** for product multi-server confine and Steam **bar3**. It does **not** re-litigate architecture, weaken 1 hard gates, close any product bar, invent multi-server product completion, or claim **bar3**. Wave 47 17 ledger text is superseded here as the same honesty formula under Wave 49 exclusive ownership of this file.

### 17.1 Soft freezes vs product bars

| Term | Meaning on this document |
|------|--------------------------|
| **Soft freeze / Accepted freeze** | Normative **what to build** (isolation, doors, CapJit, futex, cold path, AC honesty, matrix process, …). Design complete enough to implement. |
| **Soft continuum / soft ship gates** | CREATE-ONLY graph high-water / greppable kernel smokes / media inventory — bring-up honesty only (companion docs). Parent-wire high-water **advancing toward 19100** soft only; honest `makefile_max` is a Makefile scan (may still report prior tip, e.g. **19000**, until next decade is wired). |
| **Product multi-server confine** | Full multi-server drop-ambient security product: servers + clients confined by caps/promises end-to-end; bootstrap seal — **open** |
| **Product lamps** | Soft score / bar3-ready continuum stubs — remain **0** by design |
| **bar3** | Steam **client** launch on DUT + Deck Top 50 leave `NOT-TRIED` — **OPEN** |

| Soft / freeze surface | What it is | What it is **not** |
|-----------------------|------------|--------------------|
| **This document Accepted** (0–16 freezes) | Design judgment closed; implement without further architectural ambiguity on listed topics | Product multi-server confine closed; bar3 closed; Deck Top 50 titles tried |
| **Production freezes text** (personality doors, cold doors-only, CapJit rights, AS/IDT gates, …) | Normative contract for implementers | Every freeze row product-hard on DUT end-to-end |
| **Live server embeds / soft confine / soft caps** (companion tree) | Bring-up / skeleton / greppable soft | Sealed multi-server product for every server+client |
| **Continuum toward 19100 soft** / media `STATUS=READY` | Soft graph parent wire / host media inventory | Runtime ABI product complete; Steam client; matrix fill; product lamps lit |
| **16 Completion statement** | Design path complete enough to code | Product DoD complete; multi-server done; bar3 done |

**Hard rule:** never promote Accepted freezes, soft continuum (toward **19100**), media READY, or greppable soft `PASS` lines to “product multi-server closed,” “bar3 closed,” Deck Top 50 `PASS`, product lamps > 0, or product DoD complete. **Soft freezes ≠ product multi-server ≠ bar3.** **Soft ≠ product complete.**

**Hard stamp (Wave 49):** soft freezes **≠** product multi-server / bar3. Soft continuum high-water **toward 19100** is CREATE-ONLY parent wire only. Product lamps remain **0**. **No product claim** from this document.

### 17.2 Product multi-server — **open**

| Item | State |
|------|--------|
| Design freezes for isolation / doors / quotas / spawn (this doc) | **Accepted** — design only |
| Soft confine / soft cap / doors greppable surface | Soft only — see [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) 13, [CAP_ADDRESSING.md](CAP_ADDRESSING.md) 9 |
| Live embeds (`vfsd`, `sessiond`, `netstackd`, `storaged`, `sshd`, …) | Skeleton / door bring-up ≠ full multi-server product |
| Bootstrap seal (drop ambient retype / broad authority after init) | **Open** |
| Hierarchical quotas product for every creatable object class | **Open** / incomplete product path |
| Cap transfer on IPC product (copy/move K, all-or-nothing) | **Open** / incomplete product path |
| **Full multi-server product** (caps + confine end-to-end) | **Open** — soft freezes do **not** close it |

Soft freezes and soft promise/cap gates are **not** a claim that every server and client run under sealed multi-server confinement. Product multi-server stays **open** until that path is product-complete.

### 17.3 bar3 / matrix — **open** (out of freeze scope)

| Item | State |
|------|--------|
| Steam **client** launch on GreenJade DUT | **OPEN** (bar3) |
| Deck Top 50 title rows | **NOT-TRIED × 50** — claim **targeting only** (7–8 process still binds) |
| Host media `STATUS=READY` / continuum soft high-water **toward 19100** | Soft only — **≠ bar3** |
| Product lamps (`gj_bar3_ready_*` / `gj_product_score_*` soft stubs) | Remain **0** by design |
| Product RAM **≥ 1 TiB** full path | **Open** when host allows (separate from bar3) |

**Bar3 remains OPEN.** Matrix honesty lives in [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) / [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md). This document does not promote those rows.

### 17.4 Explicit non-claims (Wave 49)

| Claim | Allowed? |
|-------|----------|
| “Design Spec Complete **Accepted** — freezes what to build (0–16)” | **Yes** — design only |
| “Soft freezes / soft continuum toward **19100** / soft greppable surface (with bound)” | **Yes** — honesty ledger only |
| “Accepted freezes = product multi-server confine closed” | **No** |
| “Accepted freezes / soft continuum / media READY = bar3 closed” | **No** |
| “Product lamps lit / product complete from soft freezes or continuum” | **No** — lamps remain **0** |
| “Deck Top 50 titles tried / PASS from this freeze alone” | **No** — matrix stays **NOT-TRIED** |
| “16 completion = product DoD / multi-server / bar3 done” | **No** |
| “Wave 49 honesty closes bar3 or any matrix row” | **No** — honesty refresh only |
| Any **bar3** closed claim from DESIGN_SPEC_COMPLETE alone | **No** |

### 17.5 Related honesty surfaces

- [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) 13 — soft confine ≠ product multi-server  
- [CAP_ADDRESSING.md](CAP_ADDRESSING.md) 9 / 9.5 — soft cap surface ≠ product multi-server; continuum toward 19100 soft  
- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) — soft surface ≠ bar3  
- [IMPLEMENTATION.md](IMPLEMENTATION.md) — soft stamps ≠ product score  
- [TODO.md](TODO.md) — multi-server confine / bar3 open boxes  
- [HCL.md](HCL.md) — soft probes ≠ full HCL product; continuum toward 19100  

---

*Design Spec Complete v1.0 — Accepted. Clean-room. Deck Top 50. Security first.*  
*17 Wave 49 honesty (2026-07-23): soft freezes ≠ product multi-server / bar3; continuum **toward 19100 soft only**; product lamps **0**; **soft ≠ product complete**; **no product claim**.*
