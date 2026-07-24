# GreenJade — Security-First Core Design Decisions

| Field | Value |
|-------|--------|
| **Document** | Security core design v1.7 |
| **Status** | **Accepted** — implement against this + [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md); 13 Wave 13 historical · 14 Wave 52 honesty |
| **Priority** | Security → Performance → Portability → Readability |
| **Heritage default** | **Solaris / Sun** for concurrency, doors, DDI, turnstiles; **Apple** for VM objects/views, task ports, QoS, session ([APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md)) |
| **Heritage secondary** | IBM only when Solaris has no good analogue **and** it still matches GreenJade law |
| **Companion** | [Architecture](GREENJADE_KERNEL_SPEC.md) · [Implementation](IMPLEMENTATION.md) · [Apple channel](APPLE_CHANNEL_REMAINING.md) |
| **Honesty (Wave 52)** | Soft confine ≠ product multi-server; soft continuum toward **19400** ≠ product; lamps **0**; **no bar3 claim** (13 historical · 14) |

This document **picks one direction** for the gaps that matter most. Alternatives are noted only to record why they lost.

### Heritage preference (normative)

When choosing patterns for new subsystems:

| Rank | Source | Use when |
|------|--------|----------|
| **1** | **Solaris / Sun** | Default for kernel concurrency, IPC shape, drivers, timers/cyclic analogy, sleepable vs spin locks, PI, DDI-like UDX, zones-like isolation policy |
| **2** | GreenJade law | Always wins over any heritage (caps, no GPL, security-first, pure C, dual MIT/Apache, no ambient root) |
| **3** | L4/seL4 mechanics | Caps/untyped/CDT/revoke where Solaris has no capability model |
| **4** | **IBM** | Only if Solaris-style option is weaker or missing **and** choice still matches rank 2 |

**Do not** default to IBM hierarchical/microkernel-server framing when a Solaris-shaped design fits.

---

## 0. Design north star

| Principle | Rule |
|-----------|------|
| **Explicit authority** | No ambient root after bootstrap seal; every act needs a cap |
| **Fail closed** | Missing right / quota / timeout ⇒ error, not wait-forever |
| **Small kernel TCB** | Policy-heavy logic stays in userspace where possible; kernel enforces mechanisms |
| **Death is cleanup** | When a thread/space/host dies, kernel revokes its grants (MMIO, DMA, rings, IRQs) |
| **DoS is a security bug** | Unbounded kernel objects, IPC storms, and timer floods are in-scope threats |
| **Performance second** | Fast paths allowed only if security checks remain equivalent |

---

## 1. Authority model

### Decision: **Capability CNodes + untyped memory retype + sealed bootstrap**

| Choice | Detail |
|--------|--------|
| **Model** | **Untyped** caps for RAM (and device memory regions); **retype** into FRAME / PAGE_TABLE / CNODE / kernel objects. No dual “mystery factory” path. *(Cap mechanics: L4/seL4 — Solaris has no native cap kernel.)* |
| **Handle (x86_64 v1)** | **Scheme A:** `u64` slot + `u32` gen; slot 0 = root meta; **one CNode per process (shared by threads)** — [CAP_ADDRESSING.md](CAP_ADDRESSING.md) |
| **Derivation** | **Capability Derivation Tree (CDT)** / parent links: every minted/copied cap records parent for subtree revoke |
| **Rights** | Bitmask per cap: `Read`, `Write`, `Grant`, `Mint`, `Destroy`, `Map`, `Identify`, … type-specific |
| **Mint / copy** | Can only mint **weaker or equal** rights; never escalate |
| **Transfer** | Caps move/copy only via explicit syscalls or **IPC cap transfer slots** (see 3) — **Solaris doors**-like RPC carrying handles |
| **Revoke** | Subtree revoke — **1.1 mandatory SMP non-livelock algorithm** (**Solaris-style** fine-grained teardown, not stop-the-world) |
| **Bootstrap** | Single **root CNode** + **root untyped** pool to `init` only; then **zone-like** quota slices for subsystems (**Solaris zones** as policy analogy) |
| **Seal** | After bootstrap, `init` drops/seals root retype of privileged types so ambient superpower ends |

### Authority rules (normative)

1. **Kernel never “opens by name.”** No path→object in kernel. Naming is userspace (`vfsd`, name service) holding caps.
2. **IRQ / MMIO / IOMMU windows** are caps retyped/minted only from **device untyped / IOMMU authority** held by `devmgr` (from bootstrap), not by arbitrary apps.
3. **Endpoint badges** are set by the **receiver’s** authority when binding; clients cannot forge badges.
4. **No UID 0.** Admin is a bag of caps in a sealed admin CNode.
5. **Confused deputy mitigation:** servers that act for clients must use **client-provided reply caps / grant caps**, not ambient server rights, for client-owned resources.

### 1.1 Capability revoke on SMP — **must not livelock** (normative)

**Requirement:** `sys_cap_revoke` / object destroy / space death cleanup **progresses in bounded time per step** on multi-CPU systems. No CPU may spin forever waiting for another CPU that is itself waiting on revoke.

#### Strategy: **split invalidate from reclaim** (immediate logical revoke + deferred free)

Inspired by **Solaris-class** fine-grained SMP teardown and deferred reclaim (grace/epoch; not stop-the-world): **never hold the world while waiting for every core to drop every lock.**

#### Security property (normative — answers “when is revoke safe?”)

**YES: object is DEAD / generation-bumped first; every use fails closed.**

| Rule | Detail |
|------|--------|
| **S1** | Phase A **must** set object state **DEAD** and **bump object generation** before any other Phase A work that can block or defer. |
| **S2** | **Every** cap operation (Call, Map, Identify, Mint, …) that resolves a slot **must** load the object and check **LIVE + matching generation**. If DEAD or gen mismatch ⇒ return `GJ_ERR_DEAD` / `GJ_ERR_STALE_CAP` — **never success**. |
| **S3** | **Fail-closed does not wait on slot zeroing.** Uncleared slots may still *look* occupied; they **must not** grant successful use after S1. |
| **S4** | **Mandatory hygiene:** **every** derived CNode slot for the object **must** eventually be invalidated (structured zero — see below). Not optional. |
| **S5** | Identify on a slot pointing at a DEAD object returns `DEAD`/`STALE`, not success. |
| **S6** | **Quota / reuse:** a slot does not return to the free pool until invalidated; revoke is not “fully complete” for accounting until **all** derived slots are cleared (may finish after security is already done). |
| **S7** | Slot invalidate is **structured**, not a bare forget: `type=INVALID`, **bump slot generation**, clear obj ptr, unlink CDT edge. |

**Structured slot invalidate (normative fields):**

```
slot.type     = GJ_CAP_INVALID
slot.u32Gen++          /* invalidate user handle gen; skip 0 (null) */
slot.pObj     = NULL
slot.u16Rights = 0
unlink CDT child edge for this slot
refund cap-slot quota to owner account
```

```
Phase A — LOGICAL INVALIDATE (security complete after steps 1–2)
  1. CAS object LIVE → REVOKING (one revoker; R6).
  2. **FIRST:** set state DEAD + bump object generation.
        → From this moment, all use paths fail closed (S1–S2).
  3. Wake all waiters on this object (IPC, fault) with PEER_DEAD / REVOKED.
  4. Queue deferred work: CDT walk + **mandatory slot invalidate (S4–S7)**, then reclaim.
  5. Publish on deferred queue; do not undo DEAD.
  → Security goal met after step 2.

Phase A′ — SLOT HYGIENE (**must complete**; may span deferred runs)
  6. Walk CDT; for each derived cap slot: try-lock CNode, apply structured invalidate (S7);
     if lock busy → defer (R2). Repeat until **all** slots cleared (S4, S6).
  7. Object remains DEAD the whole time.

Phase B — DRAIN
  8. Abort in-flight kernel ops holding refs; they restart and see DEAD.
  9. Refcount and/or epoch; TLB IPI finite retry / lazy unmap — no unbounded spin.

Phase C — RECLAIM (after grace + slots done)
 10. Preconditions: all derived slots invalidated (S4); refcount == 0; epoch elapsed.
 11. Free object memory to untyped/quota.
```

#### Non-livelock rules (mandatory)

| # | Rule |
|---|------|
| R1 | **No nested “wait for CPU N’s runqueue lock while holding CNode lock”** reverse of global order. |
| R2 | **Try-lock + defer** for **slot invalidate**; never spin unbounded. Security already done (S1). |
| R3 | **DEAD/gen first (S1):** fail-closed even if slot clear and reclaim lag. |
| R4 | **Revoke never waits on userspace.** Only kernel refs and wait-queues. |
| R5 | **IPI handlers must not** take revoking object’s lock in a way that waits on the revoker (ABBA). IPI only sets flags / reschedule. |
| R6 | **One revoker:** CAS REVOKING; concurrent revoke no-op or sleep on completion (not spin). |
| R7 | **Progress:** deferred slot work **must** be driven (per-CPU worker / syscall exit / idle) until S4 complete — no silent drop of work items. |
| R8 | **Pins:** Phase A sets DEAD; Phase C waits pin==0 via **CV/turnstile sleep**, not spin. |
| R9 | **No free until slots cleared (S6)** and refs/epoch allow — prevents slot/object accounting bugs. |

#### CDT walk (bounded memory)

- Children linked from parent; iterative walk; work-limited batches then defer.
- CDT walk **does not** delay S1: object is already DEAD before long walks complete.
- CDT is how we find **every** derived slot to **invalidate (S4)**; fail-closed does not wait on full walk, but **revoke accounting complete** does.

#### Rejected revoke approaches

| Approach | Why rejected |
|----------|--------------|
| Stop-all-CPUs for entire revoke | Latency + livelock if one CPU stuck |
| Spin until every lock free while holding others | Classic SMP deadlock/livelock |
| Free object immediately without grace | UAF on other CPUs |
| Userspace-assisted revoke only | Malicious task won't cooperate |

---

### OpenBSD features (how they fit)

| Feature | Layer |
|---------|--------|
| `confine` / promises | **Userspace runtime**: drop caps / refuse to use classes of caps |
| `expose` | **vfsd policy** only (path → open returns FD-cap) |
| Kernel | Caps + quotas only |

---

## 2. Quotas & resource control

### Decision: **Kernel-enforced hierarchical quotas on all kernel objects + memory; CPU via sched budgets**

Every creatable kernel resource charges a **quota account** reachable from the creating space/CNode.

| Resource | Quota unit | Enforced where |
|----------|------------|----------------|
| CNode slots | slots | kernel |
| Threads | count + kernel stack pages | kernel |
| Address spaces / page tables | pages of PT memory | kernel |
| Endpoints / notifications | count | kernel |
| FRAME / mapped pages | pages | kernel |
| IPC message backlog | bytes / msgs per endpoint | kernel |
| IRQ bindings | count | kernel |
| CPU time | **sched_context** budget/period (or weight + max) | kernel |

### Rules

1. **No create without quota.** `NOMEM` / `QUOTA` error if account exhausted.
2. **Hierarchy:** child CNode/space gets a **delegated sub-quota**; cannot exceed parent remaining.
3. **Return on destroy:** destroy/revoke refunds quota to owner account.
4. **Kernel heap:** all kernel allocations for user-triggered objects come from **accounted pools**, not a global infinite slab.
5. **Desktop defaults:** `init` splits quotas: drivers, GUI, apps get finite pools so one app cannot exhaust system objects.
6. **Overcommit:** **off** for kernel objects; user anonymous memory overcommit is a **userspace pager policy** (default conservative for security).

### Rejected

| Alternative | Why |
|-------------|-----|
| Soft limits only (log and continue) | Fail-open under attack |
| No quotas until “later” | DoS ships by default |
| Pure userspace accounting | Malicious server lies; kernel still allocates |

---

## 3. IPC memory model

### Decision: **Synchronous endpoint IPC (Sun doors–like) + separate mapped rings for bulk data**

Two channels, never confused:

| Channel | Use | Security properties |
|---------|-----|---------------------|
| **A. Sync IPC** (`Call` / `Recv` / `Reply`) | RPCs like **Solaris doors**; short messages, **cap transfer** | Kernel copies small payload; optional cap slots; **timeout required**; **turnstile PI** |
| **B. Shared rings** | Bulk I/O (block, net, GPU cmd) | Explicit **map grants**; ownership protocol 6 |

### Sync IPC message

```
tag: label + flags + n_caps + n_bytes
regs[4]: small POD
extra: optional short buffer copy (capped, e.g. ≤ 256 B kernel copy)
caps[0..K]: transferable/mintable caps (K small, e.g. 4)
```

| Rule | Detail |
|------|--------|
| **Timeout** | Every `Call` and blocking `Recv` takes a **deadline** (or infinite only if caller holds `CapRightInfiniteWait` — rare, not default) |
| **Default timeout** | Finite (e.g. 5s desktop default in libc; drivers may use shorter) |
| **Dead peer** | Peer death ⇒ waiters woken with `GJ_ERR_PEER_DEAD`; no hang |
| **Cap transfer** | Only caps the sender holds with `Grant`; rights masked by sender; receiver must have free CNode slots + quota |
| **No implicit map** | Sync IPC never maps caller memory into callee AS |
| **Reply cap** | Single-use; only valid reply path for that call |

### Rejected

| Alternative | Why |
|-------------|-----|
| Async only (Mach queues unbounded) | Harder DoS; complex |
| Always map client buffer into server | Confused deputy / TOCTOU classic |
| Unlimited kernel-copied bulk | DoS + cache thrash |

---

## 4. Concurrency (SMP)

### Decision: **Short non-preemptible critical sections + fine-grained locks by subsystem; per-CPU scheduler; RCU-like deferred free for caps optional later**

| Area | Model |
|------|--------|
| **Preemption** | Kernel is **preemptible at safe points**; while holding spinlocks, preemption disabled on that CPU |
| **Scheduler** | **Per-CPU runqueues**; IPI for remote reschedule; load balance periodic (**Solaris-like** MP) |
| **PMM** | **Per-zone locks** (+ optional per-CPU page caches with drain) (**Solaris-class** MP scaling) |
| **Cap tables** | **Lock per CNode**; object header for state; revoke uses 1.1 (no lock order cycles with IPI) |
| **IPC endpoint** | **Lock per endpoint**; wait queues; **turnstile** for block (**Solaris turnstile** pattern) |
| **Priority inversion** | **Priority inheritance** on sync IPC / turnstile wait (**Sun turnstile/PI**) — desktop audio-safe |
| **Address space** | **Lock per space** for map/unmap; TLB shootdown IPI with generation |
| **Big kernel lock** | **Forbidden** as steady-state design (debug-only optional) |

### Rules

1. **Lock order** published and enforced in debug builds (e.g. `CNode → Object → Endpoint` never reverse).
2. **No blocking while holding spinlock** (only wait-queue sleep after drop) — **Solaris-style** sleepable locks vs spinlocks split.
3. **Revoke** concurrent-safe per **1.1**; readers see DEAD/gen mismatch → `STALE_CAP` / `DEAD`.
4. **IRQ handlers** only wake notifications / mark work; **no heavy work in hard IRQ**.
5. **Per-CPU data** for runqueue, idle thread, softirq equivalent (see 7).
6. **Revoke workqueue** may run on any CPU; must obey R1–R8.

### Rejected

| Alternative | Why |
|-------------|-----|
| Giant lock forever | Fails multi-CPU + desktop performance/security latency |
| Fully lock-free caps from day one | Too hard to get revoke right; revisit later |
| Userspace-only scheduling | Out of scope for v1 kernel |

---

## 5. Time

### Decision: **Monotonic clock in kernel; wall clock is userspace policy; all blocking has timeouts; timer queues are quota-backed**

| Clock | Source | Who trusts it |
|-------|--------|----------------|
| **Mono** (`CLOCK_MONOTONIC`) | TSC calibrated + HPET/APIC; never steps back | Kernel timeouts, sched budgets |
| **Boot** | Mono since boot | Logs |
| **Wall** | Userspace `timed` service + RTC; settable with cap | Apps; **not** used for security timeouts |

### Rules

1. **Security timeouts use mono only.**
2. **Every blocking primitive** accepts timeout (IPC, futex-like wait later, driver waits).
3. **Infinite wait** requires explicit privilege cap (default apps cannot).
4. **Timer objects** (one-shot/periodic notifies) charge **quota**; max timers per account.
5. **High-res:** APIC timer deadline mode preferred; coalescing allowed for non-RT.
6. **No kernel sleep(forever)** for user-triggered paths.

### Rejected

| Alternative | Why |
|-------------|-----|
| Wall clock for IPC timeouts | NTP step → DoS / early timeout races |
| No timeouts (“eventual”) | Deadlock as DoS |
| Ad-hoc busy-wait in drivers | Burns CPU; hides bugs |

---

## 6. Zero-copy ring ownership

### Decision: **Single-producer single-consumer (SPSC) rings in shared pages with explicit owner phase; kernel only establishes maps; protocol is userspace + optional kernel pin**

### Mapping

| Step | Who | Cap needed |
|------|-----|------------|
| 1. Allocate frames | Producer or broker | FRAME quota |
| 2. Map **RW** into owner, **RO** or **RW** into peer per role | Both ASes | `Map` + grant |
| 3. Register ring with server via sync IPC (pass **frame caps** or stable IDs) | Client | Grant |
| 4. On peer death | Kernel | Unmap + revoke frames if owned by dead; notify survivor |

### Ownership protocol (normative)

Rings use **indexes** (`head`/`tail`) with memory ordering (C11 atomics in userspace):

| Region | Owner | Peer |
|--------|-------|------|
| Free descriptors | Consumer returns | — |
| Filled descriptors | Producer publishes | Consumer processes |
| Payload pages | **Pinned owner** bit in descriptor: who may write | Other side RO map when not owner |

**Rules:**

1. **Only the current owner may write** a payload slot; peer’s PTE should be RO for that page when possible (best effort); always enforced by **protocol + revoke on violation kill**.
2. **One mutator of `head`, one of `tail`** (SPSC). Multi-producer requires **multiple rings** or a kernel queue (avoid MPMC shared rings in v1).
3. **Do not put secrets in rings** without end-to-end encryption if peer is mutually distrusting; for driver rings, peer is driver host (medium trust).
4. **Backpressure:** full ring ⇒ producer waits on **notification** with timeout, not spin forever (spin briefly then block — performance second).
5. **Reset:** either side may request reset via sync IPC; other must ACK; kernel can force unmap on timeout.

### Kernel role (minimal)

- Map/unmap/grant frames  
- Optional **pin** count so pages aren’t freed while mapped  
- On space death: unmap all grants from that space  

Kernel does **not** interpret ring contents (keeps TCB small).

### Rejected

| Alternative | Why |
|-------------|-----|
| Kernel copies all bulk I/O | Safe but fails perf + 1 TiB desktop I/O |
| Full MPMC lock-free rings in shared mem v1 | Easy to get wrong; use multiple SPSC |
| Server always maps all client RAM | Catastrophic confused deputy |

---

## 7. IRQ batching

### Decision: **Hard IRQ → per-CPU backlog + notification; userspace driver coalesces; optional adaptive moderation**

```
Device IRQ
  → kernel hard handler (minimal): mask/ack as needed, set pending bit, wake Notification
  → return
  → userspace driver host: pull completions in batch (ring), process N events
  → EOI / unmask policy under driver control where safe
```

| Piece | Choice |
|-------|--------|
| **Hard IRQ context** | No allocation; no IPC call; only notify + seqlock-ish counters |
| **Coalescing** | Device MSI-X + driver sets moderation; kernel allows **shared notification** for vector storms |
| **Storm control** | Per-IRQ **rate quota**: if exceeded, mask IRQ, notify `devmgr`, require re-enable cap path |
| **Batching wakeups** | Multiple IRQs can OR into one **Notification** badge bitmask |
| **Threading** | Driver host uses thread pool; one thread per HBA default (security: less shared state) |
| **Priority** | Driver threads can hold soft-RT sched_context (quota-backed) |

### Rules

1. **Never run driver logic in kernel IRQ.**  
2. **Storm = security event** (log + mask), not spin.  
3. **Batching must not reorder** completion identity incorrectly (device rings define order).  
4. **Latency-sensitive** (input, audio): separate vectors/notifications; less coalescing.  
5. **Storage/net:** aggressive batching OK under backpressure.

### Rejected

| Alternative | Why |
|-------------|-----|
| Threaded IRQs fully in kernel like Linux | Bloats TCB |
| One wakeup per descriptor always | Melts under 100G/NVMe; fails perf without security win |
| No storm control | IRQ DoS |

---

## 8. How the pieces fit (desktop bootstrap)

```
UEFI → kernel
  → creates root untyped + root CNode + init thread (quota = system)
  → init splits quotas: devmgr | vfs | net | gui | app_pool
  → seals dangerous retype/mint caps
  → devmgr mints IRQ/MMIO/IOMMU window caps to driver hosts
  → driver hosts: SPSC rings with storaged/netstackd/compositor
  → apps: only endpoints + FD-caps from vfs; finite quotas
```

| Server | Authority |
|--------|-----------|
| `init` | Bootstrap only; then sealed |
| `devmgr` | Device untyped / window mint; monitors storm masks |
| Driver hosts | Only their MMIO/IRQ/DMA windows + rings |
| `vfsd` / GUI | No raw MMIO; talk via IPC + rings |

---

## 9. Remaining subsystems — **Solaris-first** picks (IBM only if needed)

These close earlier design gaps. Lineage is **inspirational**, reimplemented clean-room under MIT OR Apache-2.0.  
**Default heritage: Solaris.** IBM appears only in notes where relevant as secondary.

### 9.1 Process lifecycle — **Solaris `posix_spawn` / door clients**, not full `fork`

| Decision | **Spawn with explicit attribute + cap set** (no v1 `fork` that clones all caps) |
|----------|----------------------------------------------------------------------------------|
| **Solaris** | **`posix_spawn`**, door clients attaching to servers; multi-LWP process model |
| **Why not full fork** | Full fork copies ambient authority → fights seal + least privilege (GreenJade law > classic Unix fork) |
| **API shape** | `spawn(space_spec, entry, stack, cap_bundle[], quota_slice)` |
| **exit / wait** | Parent holds **process/task cap**; `wait` via notification or sync IPC; quotas refunded on death (**1.1** teardown) |
| **threads** | Multiple threads per address space (**Solaris LWP-like**); thread caps under space |
| **Later** | Optional `fork` emulation in POSIX personality **without** cap duplication — not kernel v1 |

### 9.2 Page faults & pager — **regions + memory objects** (Apple VM; Solaris cookie path)

| Decision | **Region → memory object → pager**; PCB **default pager** is fallback only |
|----------|--------------------------------------------------------------------------------|
| **Full rules** | [CAP_ADDRESSING.md](CAP_ADDRESSING.md) · [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 1–2 · cookie [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) 7 |
| **Solaris** | HAT/segment spirit; pager is a **door server** |
| **Apple** | Memory **objects** own pages; maps are **views**; shared file maps first-class |
| **Path** | Fault → region lookup → object pager or default → Call (mono timeout) → map views or kill |
| **Root meta (slot 0)** | Process + CNode identity for **kernel ops only**; empty until bootstrap; not a cap factory |
| **Task port** | `GJ_CAP_PROCESS` to parent on spawn — [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 3 |
| **Default pager** | `sys_process_set_pager` → PCB; needs PROCESS VM / self |
| **Pager death / timeout** | Fail closed (kill faulting thread) |
| **File maps** | File **memory object** (often `vfsd` as object pager) |
| **Overcommit** | Default **off** |

### 9.3 Sync IPC style — **Solaris doors** (primary)

| Decision | **Sync Call/Recv/Reply** = **doors** |
|----------|--------------------------------------|
| **Server model** | Userspace door server thread pool; kernel is rendezvous only |
| **PI** | **Turnstile priority inheritance** when client blocks on server (4) |

### 9.4 Name service — **not kernel; door rendezvous / `door_call` to `ns`**

| Decision | **Userspace name service** holding endpoint caps; kernel has **no** global name table |
|----------|----------------------------------------------------------------------------------------|
| **Solaris** | Services reached by **doors** / well-known paths in userspace, not kernel namei for IPC |
| **Bootstrap** | `init` passes well-known caps; optional `ns` for late bind |
| **Restart** | Server death invalidates caps (1.1); clients re-resolve via `ns` with timeout |
| **Security** | `ns` high-trust; registration requires **register cap** from `init` |

### 9.5 IOMMU / DMA — **Solaris DDI DMA first**

| Decision | **Kernel mechanism + `devmgr` policy**; DMA = **window caps** |
|----------|----------------------------------------------------------------|
| **Solaris** | **DDI DMA** engine pattern: drivers don’t program raw bus-master; bind handles / windows |
| **Create window** | `{ device_id, iova, FRAME caps, rw }` only with IOMMU/DMA authority cap |
| **UDX** | `dma_alloc` / `dma_map` via IPC to **devmgr/iommu** helper |
| **Death** | Phase A 1.1: **disable IOMMU window** before reclaim |
| **No IOMMU** | Production: refuse bus-master (or refuse device); QEMU dev profile may warn-and-allow |

### 9.6 Device drivers — **Solaris DDI/DKI** (primary)

| Decision | UDX mirrors **DDI** `attach` / `detach` / `quiesce`, ops tables, per-instance soft state |
|----------|-----------------------------------------------------------------------------------------|
| **quiesce** | On host shutdown/revoke: detach DMA, mask IRQs (**SAS HBA must quiesce**) |
| **Not** | In-kernel **STREAMS** (even though historic Solaris — rejected for TCB/modern net); not Linux DRM/scsi source |

### 9.7 Signals / async events — **Solaris-shaped, smaller TCB**

| Decision | v1: **no full POSIX signal storm in kernel**; **Notification** + optional user `sig` server |
|----------|-----------------------------------------------------------------------------------------------|
| **Ctrl+C** | Terminal → session server → Notification to process group cap set |
| **CPU exception** | **Exception port** on PCB (separate from pager) if registered; else kill — Apple 12 |
| **Not** | Recreating full Solaris signal complexity in ring 0 |

### 9.8 Desktop product (Apple channel — Accepted)

| Topic | Decision |
|-------|----------|
| **QoS** | Thread QoS classes + capped PI — Apple 8 |
| **Futex** | Early `futex_wait`/`wake` — Apple 9 |
| **W^X / JIT** | Hard W^X; JIT only with cap — Apple 10 |
| **Session** | Single-seat compositor TCB — Apple 11 |
| **Devices** | Match → grant → UDX host — Apple 6 |
| **Reply IPC** | Kernel ephemeral single-use REPLY — Apple 4 |
| **Full text** | [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) |

### 9.9 Proton personality + Deck Top 50 (**Accepted**; precedence over Apple)

| Topic | Decision |
|-------|----------|
| **Adoption** | No major games ⇒ no popular desktop; **major** = **Deck Top 50** (arch 0.5.1–0.5.2) |
| **Critical path** | A0→A1 unblocks from M2 (arch 0.5.3) — not a late hobby track |
| **Priorities** | Still **Security → Performance → Portability → Readability** |
| **ABI** | **Clean-room Linux-compatible** personality grows with matrix needs; **no GPL source** |
| **Not** | SteamOS distro port; load Linux `.ko` |
| **Doc precedence** | Proton over Apple among secure designs — [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) 0.1 |
| **Surface** | `libprotonrt` + `proton_rt_query`; native GJ underneath |
| **Graphics** | Vulkan present path P0/P1 for A1; clean-room DRM shims only if Top 50 needs |
| **Anti-cheat** | GJ userspace agent OK; `.ko` no; PASS-OFFLINE / BLOCKED-SECURITY honesty |
| **P0 kernel needs** | Cross-process futex, named memory objects, JIT, PROCESS, Unix-shaped IPC, exception port for game tasks |
| **Doc** | [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) **v1.6 Accepted** |

### 9.10 Production freezes (isolation, doors, AC) — **Accepted**

Full text: [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md).

| Topic | Decision |
|-------|----------|
| IDT gate | Before untrusted ELF |
| Kernel maps | Supervisor-only (U=0); SMEP/SMAP product |
| AS | Per-process CR3; kernel half shared |
| Personality | Kernel thread = bring-up only; **userspace doors** product |
| Cold API | **Doors only** (queue API deprecated) |
| CapJit | Real PROCESS right, not bare bool forever |
| Doors | ENDPOINT + DEAD/gen + quota (scaffold → production) |
| Anti-cheat | Userspace agent optional; no `.ko` |
| Locks | Documented order runqueue → … → PMM |

### 9.8 Entropy — **hardware + virtio-rng; Solaris `/dev/random` as userspace service shape**

| Decision | Early: RDRAND/RDSEED/arch RNG; runtime: **entropyd** (userspace), apps via doors/IPC |
|----------|--------------------------------------------------------------------------------------|
| **KASLR** | CPU RNG + firmware/bootloader random before full userspace |

### 9.11 Multi-user / seats — **Solaris zones as policy analogy**

| Decision | v1 desktop may be **single seat**; multi-user = **separate quota roots + compositor instances** (**zones**-like), not shared ambient GUI |
|----------|------------------------------------------------------------------------------------------------------------------------------------------|
| **Not** | Kernel UID tables as primary authority |
| **IBM** | Not used as default framing |

### 9.10 Bulk I/O rings — **least privilege (Solaris device access spirit)**

| Peer class | Mapping |
|------------|---------|
| Driver ↔ storaged (same TCB tier) | SPSC; may use RW/RW with protocol (6) |
| App ↔ untrusted | Owner-flip remapping or RO peer PTEs; no secrets in clear rings |

---

## 10. Implementation mapping (when)

| Piece | Milestone |
|-------|-----------|
| Cap tables + mint + **1.1 revoke** + generations | M2 |
| CDT / parent links + deferred CNode slot clear | M2 |
| Quotas on create | M2 |
| IPC + timeouts + cap transfer + **turnstile PI** | M2 |
| Peer death wakeup | M2 |
| Spawn/exit (no full fork) | M2–M3 |
| Per-CPU sched + locks | M1–M4 |
| Mono clock + timer quota | M1–M2 |
| Pager + fault endpoint | M3–M4 |
| Ring map grants + death unmap | M3 |
| IOMMU windows + disable-on-revoke | M4–M5 |
| IRQ notify + storm mask | M3–M5 |
| Name service userspace | M3 |
| SPSC virtio / scsi rings | M3–M5 |

---

## 11. Non-negotiable checklist (code review)

- [ ] No new kernel object without **quota charge**  
- [ ] No blocking wait without **mono timeout** (or InfiniteWait cap)  
- [ ] No bulk path that maps foreign AS without **explicit grant**  
- [ ] No hard-IRQ allocation or driver logic  
- [ ] Spinlock order documented if new lock added  
- [ ] Death path **revokes** MMIO/IRQ/DMA/rings for that space  
- [ ] Cap user handles never raw kernel pointers  
- [ ] **Revoke follows 1.1** — no unbounded SMP spin; DEAD/gen before free  
- [ ] IOMMU window **disabled in hardware** before FRAME reclaim  
- [ ] Spawn does **not** clone full ambient cap set  

---

## 12. Summary table (the picks)

| Area | **Chosen direction** |
|------|----------------------|
| **Authority** | Untyped retype + CNodes + CDT (L4 mechanics); **bootstrap seal** |
| **Revoke (SMP)** | **1.1** invalidate → drain → epoch reclaim; **no livelock** (R1–R8) |
| **Quotas** | Hierarchical quotas; **zones-like** slices; CPU sched budgets |
| **IPC** | **Solaris doors**-like sync RPC + cap transfer; timeouts; **turnstile PI** |
| **IPC memory** | No implicit maps; bulk = **SPSC rings** |
| **Concurrency** | Fine-grained locks, per-CPU RQ, sleep vs spin (**Solaris MP**) |
| **Time** | Mono for security; wall userspace; timer quotas |
| **Process** | **posix_spawn-like** + explicit caps (not full fork) |
| **Pager** | Fault → **door/Call** to userspace pager (**Solaris HAT/segment** analogy) |
| **Names** | Userspace NS via doors |
| **DMA** | **DDI DMA**-shaped window caps; disable on revoke |
| **Drivers** | **DDI/DKI** UDX; quiesce on teardown |
| **IRQ** | Minimal hard IRQ → Notification; storm mask |
| **Heritage** | **Solaris default**; L4 for caps; **IBM only if Solaris lacks a fit** |

---

## 13. Honesty bounds — soft confine vs product multi-server (Wave 13 · 2026-07-23)

**Additive only (Wave 13 exclusive for this file).** Design decisions in 0–12 stay **Accepted**. This section is a Wave 13 honesty ledger: what is greppable **soft confine** (and related soft security surface) on the tree vs what remains **product multi-server** open. It does **not** re-litigate authority, revoke, quotas, IPC, or heritage picks, close any product bar, invent multi-server product completion, or claim **bar3**. Wave 10 13 ledger text is superseded here as the same honesty formula under Wave 13 exclusive ownership of this file.

| Term | Meaning in this document |
|------|--------------------------|
| **Accepted** | Security-first core design frozen — ship toward these rules |
| **Soft confine** | Partial / greppable confine or promise gates (e.g. process bitmask on some ambient-style Linux paths); OpenBSD-shaped *intent* in OpenBSD features — **not** product seal |
| **Product multi-server** | Full multi-server drop-ambient security product: servers + clients confined by caps/promises end-to-end; bootstrap seal; finite per-server authority as in 8 — **open** |
| **bar3** | Steam **client** on DUT + Deck Top 50 leave `NOT-TRIED` — **out of scope to claim here** |

**Hard stamp (Wave 13):** soft confine **≠** product multi-server. Soft greppable confine/promise bits do **not** ship sealed multi-server product. **No bar3 claim** from this document.

### 13.1 Soft confine (honest bound — may claim soft only)

| Soft surface | What shipped / greppable means | What it does **not** mean |
|--------------|--------------------------------|---------------------------|
| **Confine / promises soft** | Process bitmask gates some ambient-style Linux paths (`confine soft`); userspace drop-caps *intent* (OpenBSD features) | **Product multi-server closed**; every server/client sealed under promises |
| **Cap soft surface** | Mint/copy/move + soft CDT/quota/trylock; DEAD/gen first hooks — see [CAP_ADDRESSING.md](CAP_ADDRESSING.md) 9 | Full 1 / 1.1 product revoke, zone-like quotas everywhere, IPC cap-transfer K complete |
| **Doors / IPC soft** | Sync Call/Recv/Reply bring-up + timeout/peer-death soft | Full 3 doors product (cap transfer, turnstile PI under load, reply-only paths everywhere) |
| **Live server embeds** | `vfsd`, `sessiond`, `netstackd`, `storaged`, `sshd`, … freestanding spawn / door skeleton | Each server runs under sealed quota + only granted caps; full multi-server blast-radius product |
| **Bootstrap / init soft** | Root untyped + root CNode + init path; quota-split *policy* in design | Post-bootstrap **seal** product (1 seal rule); ambient retype gone for all untrusted |
| **Production freezes text** | 9.10 / [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) **Accepted** decisions | Every freeze row product-hard on DUT |

**Hard rule:** soft greppable `PASS` lines, soft confine bits, live embeds, and design **Accepted** are **bring-up / agent honesty**, not “security product shipped” and not “multi-server confine done.”  
**Wave 13 formula:** **soft confine ≠ product multi-server.**

### 13.2 Product multi-server — **open**

| Item | State |
|------|--------|
| Soft confine / promise gates on some paths | Soft only — not product seal |
| Soft cap / CDT / quota / doors surface | Present — soft / greppable (companion docs) |
| Live server embeds (`vfsd`, `sessiond`, `netstackd`, …) | Skeleton / door bring-up ≠ full multi-server product |
| Bootstrap seal (drop ambient retype / broad authority after init) | **Open** |
| Hierarchical quotas product for every creatable object class (2) | **Open** / incomplete product path |
| Cap transfer on IPC (copy/move K caps, all-or-nothing) (3) | **Open** / incomplete product path |
| Death cleanup product: revoke MMIO/IRQ/DMA/rings for dead space (0, 1.1) | Soft hooks ≠ full product under all pins |
| `expose` / path policy in `vfsd` (OpenBSD-shaped) | **Open** |
| **Full multi-server product** (caps + confine end-to-end) | **Open** — soft confine does **not** close it |

Soft confine and soft promise gates are **not** a claim that every server and client run under sealed multi-server confinement with bootstrap seal and least-privilege caps only. Product multi-server stays **open** until that path is actually product-complete (see [TODO.md](TODO.md) multi-server / confine items and [CAP_ADDRESSING.md](CAP_ADDRESSING.md) 9).

### 13.3 Explicit non-claims (Wave 13)

| Claim | Allowed? |
|-------|----------|
| “Security core design **Accepted**” | **Yes** — this document 0–12 |
| “Soft confine / soft cap / doors greppable” | **Yes** — with soft bound in 13.1 |
| “Soft confine = product multi-server closed” | **No** |
| “Bootstrap seal / full hierarchical quotas / IPC cap transfer product-complete” | **No** |
| “All live embeds are multi-server confined product” | **No** |
| “Continuum soft gates / media `STATUS=READY` close bar3” | **No** |
| “Deck Top 50 titles tried / PASS from this doc” | **No** — matrix stays **NOT-TRIED** until real client runs |
| Any **bar3** closed claim from security core design alone | **No** |

**Bar3 remains OPEN** (client + matrix). This Wave 13 edit is honesty-only: **soft confine ≠ product multi-server; no bar3 claim.** Matrix honesty lives in [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) / [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — this document does not promote those rows.

### 13.4 Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [CAP_ADDRESSING.md](CAP_ADDRESSING.md) 9 — soft cap surface ≠ product multi-server confine  
- [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) 19 — remaining vs soft; no bar3 claim  
- [TODO.md](TODO.md) — multi-server confine / `confine(promises)` / drop ambient open boxes  
- [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) — production freezes (Accepted; product path separate)  



## 14. Honesty bounds — Wave 52 restatement (2026-07-23)

**Additive only (Wave 52 exclusive for this file).** Design decisions in 0–12 stay **Accepted**. 13 Wave 13 soft-confine ledger stays historical. This section restates the product/bar3 bound under Wave 52 continuum honesty and does **not** re-litigate authority, revoke, quotas, IPC, or heritage picks.

| Term | Meaning in this document |
|------|--------------------------|
| **Soft confine** | 13 greppable confine / promise surface — **not** product multi-server seal |
| **Soft continuum** | CREATE-ONLY libcgj graph parent wire **advancing toward 19400**; honest scan may still report **makefile_max=19300** until parent wires — **not** product complete |
| **Product lamps** | Soft bar3-ready / product-score stubs remain **0** by design |
| **bar3** | Steam **client** on DUT + Deck Top 50 leave `NOT-TRIED` — **OPEN**; out of scope to close here |

**Hard stamp (Wave 52):** soft confine **≠** product multi-server. Soft continuum toward **19400** **≠** product complete and **≠** bar3. Product lamps remain **0**. **No bar3 claim** from this document. Matrix stays **NOT-TRIED × 50**.

### 14.1 Explicit non-claims (Wave 52)

| Claim | Allowed? |
|-------|----------|
| “Security core design **Accepted**” | **Yes** — this document 0–12 |
| “Soft confine / soft cap / doors greppable” | **Yes** — with soft bound in 13.1 |
| “Soft continuum high-water advancing toward **19400** (scan may still be 18400)” | **Yes** — soft only; scan is source of truth |
| “Soft confine = product multi-server closed” | **No** |
| “Soft continuum / product lamps close product DoD” | **No** — lamps remain **0**; soft ≠ product complete |
| “Continuum soft gates / media `STATUS=READY` close bar3” | **No** |
| “Deck Top 50 titles tried / PASS from this doc” | **No** — matrix stays **NOT-TRIED** |
| Any **bar3** closed claim from security core design alone | **No** |

**Bar3 remains OPEN** (client + matrix). Wave 52 honesty-only: **soft confine ≠ product multi-server; soft continuum ≠ product complete; no bar3 claim.**

### 14.2 Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [IMPLEMENTATION.md](IMPLEMENTATION.md) — Wave 52 continuum toward 19400 soft stamp  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — title rows (**NOT-TRIED × 50**)  
- 13 above — Wave 13 soft confine ≠ product multi-server ledger  

---

*Accepted security-first core design v1.7 — DEAD/gen first; mandatory deferred slot invalidate; SMP-safe revoke; Solaris-first.*  
*13 Wave 13 honesty (historical): soft confine ≠ product multi-server; **no bar3 claim**.*  
*14 Wave 52 honesty (2026-07-23): soft confine ≠ product multi-server; continuum toward **19400** soft only (scan may still be **19300**); product lamps **0**; **soft ≠ product complete**; **no bar3 claim**.*  
*Code: `kernel/include/gj/cap.h`, `kernel/cap/revoke.c`.*
