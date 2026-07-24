# GreenJade — Remaining Design Choices (Solaris-channel)

| Field | Value |
|-------|--------|
| **Document** | Remaining decisions v1.2 |
| **Status** | **Accepted** — Solaris-first judgment under GreenJade law |
| **Persona** | “What would a Sun kernel engineer ship?” |
| **Honesty** | **2026-07-24 Wave 85** — remaining (design→code) vs soft ship (19); continuum **toward 22700** soft only; product lamps **0**; **no bar3 claim** |
| **Companion** | [CAP_ADDRESSING.md](CAP_ADDRESSING.md) · [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) |

GreenJade law still wins: caps, security-first, pure C, dual MIT/Apache, no GPL, no ambient root.  
Where Solaris had no caps, we keep **L4/seL4 mechanics** but **Solaris ergonomics and lifecycle**.

**VM / task / session / QoS / futex / JIT:** deferred to **Apple channel** — [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) (Accepted). Cookie, cluster, fault lock, W^X checks in 7 still apply.

**Soft stamp (Wave 8, historical):** parallel continuum / kernel / media soft gates on tree do **not** close this channel’s remaining implementation work, and do **not** close Steam **bar3**.

**Soft stamp (Wave 85):** same remaining-vs-soft bound restated under Wave 85 exclusive ownership of this file’s honesty ledger. Continuum high-water **advancing toward 22700** soft only. Product lamps remain **0**. Soft ship ≠ remaining closed ≠ bar3. See 19.

---

## 0. Solaris engineering instincts used here

1. **Simple names for apps** — FD-like handles; deep machinery in the kernel.  
2. **Doors for control, bulk elsewhere** — sync RPC; rings for data.  
3. **Process is the unit of authority** — shared CNode = shared “FD table.”  
4. **DDI for devices** — attach/detach/quiesce; DMA via handles, not raw bus-master.  
5. **Turnstiles + PI** — don’t starve interactive paths under sync IPC.  
6. **Fail closed, then recover** — kill the bad actor; don’t hang the box.  
7. **Ship a support matrix** — “desktop” is a product, not infinite drivers.  
8. **Don’t put policy in the trap path** — registration on PCB; hot path stays dumb.

---

## 1. Untyped retype (P0)

### Decision: **Two untyped classes + power-of-two size ladder (seL4-shaped, Solaris resource pools)**

| Kind | Source | May retype to | Forbidden |
|------|--------|---------------|-----------|
| **RAM untyped** | Boot free list / HHDM | FRAME (4K), large FRAME (2M/1G where supported), CNODE backing, PAGE_TABLE, kernel objects that consume RAM | Device MMIO |
| **Device untyped** | `devmgr` from UEFI/ACPI/PCI BARs | MMIO_FRAME (map device only), IOMMU window ingredients | **Never EXEC**; never anonymous RAM object |

**Size ladder (RAM):** retype only into sizes `4K × 2^n` up to a cap (e.g. 2M/1G for large pages).  
**Recycle:** destroy object → memory returns to **parent untyped** (or quota account), not a global free-for-all.  
**Who retypes device untyped:** only holders of that untyped (bootstrap → `devmgr`).  
**Who retypes RAM:** process quota + untyped cap with MINT/retype right.

*Solaris voice:* treat untyped like **resource pools / kmem magazines at the authority layer** — explicit, accountable, no silent overcommit of kernel objects.

---

## 2. CDT + `slots_left` (P0)

### Decision: **CDT child list on object; every install/copy/move is explicit**

| Operation | CDT | `u32SlotsLeft` on object |
|-----------|-----|---------------------------|
| **Install / mint new binding** | Add child edge slot→object | `++` |
| **Copy** (new slot, same object) | Add child edge | `++` |
| **Move** (same process) | Relink edge; no net `++` | unchanged |
| **IPC transfer copy** | Child in dest process | `++` on object |
| **IPC transfer move** | Edge moves to dest | unchanged |
| **Slot invalidate (S7)** | Unlink edge | `--` (not below 0) |

**Quota:** each **occupied CNode slot** charges the process’s slot quota (Solaris: finite FD limits).  
**CDT node memory:** charged to the **creating process’s** kernel-object quota (or small fixed slab per mint).

*Solaris voice:* every name in the table costs something — no free infinite C-list.

---

## 3. IPC cap transfer under Scheme A (P0)

### Decision: **Receiver allocates; reply carries new handles**

```
Call(payload, send_caps[0..K-1]):
  sender must GRANT each sent cap
  kernel:
    for each cap:
      alloc free slot in receiver process CNode (skip 0)
      install copy or move
      record new (slot, gen) for reply
  if any alloc fails → whole Call fails (no partial installs); QUOTA/NOSPC
  deliver message + list of new handles in reply/regs
```

| Rule | Detail |
|------|--------|
| Dest slot | **Kernel picks** free slot ≥ 1 (not caller-chosen) — avoids clobber races |
| Partial failure | **All-or-nothing** |
| Max K | Small (e.g. 4) — doors-scale, not SCM_RIGHTS flood |
| Reply | `n_caps` + pairs `(slot, gen)` in message regs/buffer |

*Solaris voice:* `door_call` with descriptors — server gets new FDs; doesn’t pick FD numbers.

---

## 4. Syscall ABI freeze sketch (P0)

### Decision: **Stable numbers from M2; Scheme A always `(slot, gen)` first when a cap is named**

**Normative sketch (includes Apple additions):** [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 5 — futex, vm_*, process_wait/kill, QoS.  
Exact layouts: `kernel/include/gj/syscall.h` when frozen — **do not renumber after first userland**.

*Solaris voice:* stable syscall surface; version if you must, don’t reshuffle.
---

## 5. CNode locking (P0)

### Decision: **Per-process CNode mutex; resolve under RCU-later, lock for now**

| Path | v1 | Later perf |
|------|----|------------|
| **Install / move / invalidate** | Hold **CNode lock** | same |
| **Resolve** | Hold CNode lock **shared** or exclusive (v1: exclusive is fine) | Gen-checked lock-free read if needed |
| **Order** | `CNode → Object → Endpoint` (never reverse); PMM below Object |

*Solaris voice:* one `fdt` lock per process first; optimize the read path when profiles hurt.

---

## 6. Process / space / thread lifecycle (P0)

### Decision: **Process owns space + CNode; threads are LWPs**

```
spawn:
  1. create process object (quota)
  2. create address space
  3. create CNode (slots from untyped/pages)
  4. bootstrap root meta slot 0
  5. install initial cap bundle (from parent GRANT list)
  6. create first thread (entry, stack)
  7. on any failure: reverse tear down (1.1 revoke)
exit last thread / process_kill:
  clear pager; revoke all CNode slots; destroy space; free process
```

| Rule | Detail |
|------|--------|
| Thread | Holds `pProc`; uses `pProc->pCnode` |
| No per-thread CNode | Already decided |
| `wait` | Parent holds process cap (typed, from spawn return — **kernel mint**, not root meta) |

*Solaris voice:* process is the container; threads are LWPs; death cleans the FD table.

---

## 7. Pager map-into-client (P1) — **most secure map path** (normative)

**Region/object model + page ownership:** [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 1–2 (Accepted).  
This section remains normative for **cookie, cluster, kernel-only PTE install, W^X, fail-closed**.

### Decision summary

| Item | Choice |
|------|--------|
| **Token** | **Kernel-only cookie** in the fault message — **not** a CNode cap type, not forgeable by userspace |
| **Cluster** | **Multi-page fault cluster** (contiguous VA range), not only the single faulting page |
| **Map security** | **Maximum practical:** least privilege, single-use, range-bound, verified by kernel |
| **Page ownership after map** | **Memory object owns pages; client map is a view** (Apple 2). Anon object charged to process ledger. |
| **Concurrency** | **One fault lock per address space** — serialize fault handling for that space |

### 7.1 Kernel-only map cookie

| Rule | Detail |
|------|--------|
| **M1** | Cookie is a large random/secret **kernel value** (or index into a kernel table keyed by secret). Userspace **cannot** mint or forge it. |
| **M2** | Cookie appears **only** in the fault **Call** message from kernel → pager. Not installable into any CNode. |
| **M3** | Pager returns cookie **unchanged** on reply with FRAME caps (or frame list); kernel validates cookie before any map. |
| **M4** | Cookie is **single-use**: successful map, FAIL reply, timeout, process death, or space revoke ⇒ **invalidate cookie**. Replay ⇒ error. |
| **M5** | Cookie is bound to: `{ space_id, memory_object_id, cluster_base, cluster_npages, access, faulting_thread, mono_deadline }`. Mismatch ⇒ reject. |

*Not a cap type:* avoids CNode pollution, GRANT confusion, and transfer of map authority via IPC.

### 7.2 Multi-page fault cluster

| Rule | Detail |
|------|--------|
| **C1** | On fault at VA, kernel computes a **cluster**: page-aligned range covering the fault, optionally coalesced with adjacent not-present pages (same protection intent). |
| **C2** | Cluster size **capped** (e.g. max N pages, default small like 16) — DoS/security bound. |
| **C3** | Fault message carries **base VA + page count** (and access bits), not only one address. |
| **C4** | Pager may supply **1..count** FRAMEs; kernel maps only within the cluster; unfilled pages stay faultable later. |
| **C5** | Partial fill is OK if pager replies success with subset; remaining pages fault again (no ambient expand of cookie range). |

### 7.3 Most secure map rules (kernel verifies everything)

| Rule | Detail |
|------|--------|
| **S1** | Pager **never** receives ambient Map rights on the client space. |
| **S2** | Only the **kernel** installs PTEs in the client, after validating cookie + FRAMEs. |
| **S3** | FRAMEs must be LIVE, RAM FRAME (not device MMIO unless policy allows), rights ⊆ requested access; **no EXEC** unless fault access asked X and policy allows. |
| **S4** | W^X: refuse WRITE\|EXEC together on client user maps (existing GreenJade policy). |
| **S5** | After successful map: pages attach to the **memory object**; client holds a **view** (PTE). Pager does not keep Map into client AS. App-visible FRAME CNode slots are **optional** (usually hidden by libc). |
| **S6** | Pager must not retain a mapping into the client AS. Shared file objects may remain mapped in other processes. |
| **S7** | On FAIL/timeout: no PTE change; FRAMEs stay with pager/object path; cookie dead; faulting thread **killed** (v1). |

### 7.4 Object owns pages; maps are views (Apple 2)

| Before map | After successful map |
|------------|----------------------|
| FRAME held by pager path (GRANT to kernel) | Pages owned by **memory object**; client has **PTE view** |
| Cookie authorizes map | Cookie **invalid** |
| Client had hole at VA | Client has PTE; charge **object / process ledger** |

Revoke of client space ⇒ drop views; object refcount may free pages.  
Shared object: other processes’ views remain until their unmap.

### 7.5 One fault lock per address space

| Rule | Detail |
|------|--------|
| **L1** | Each `gj_space` (or process space) has **one fault lock**. |
| **L2** | Page-fault path: acquire space fault lock → handle one cluster → release. |
| **L3** | Concurrent faults in same space **serialize** (no parallel map races / double-fill). |
| **L4** | Faults in **different** spaces may proceed in parallel. |
| **L5** | Do not hold CNode lock across the whole pager Call (deadlock risk); copy needed state under locks, Call pager, then map under fault lock + space map lock per lock order. |
| **L6** | Lock order (extend global ranks): … → **SpaceFault → SpaceMap → …** (document with CNode/Object; never hold SpaceFault while taking remote process CNode for unrelated ops). |

### 7.6 End-to-end flow

```
fault in space S at VA
  → acquire S.fault_lock (one per space)
  → if another fault in progress on S: wait on fault CV or kill policy (v1: wait with mono timeout then kill)
  → lookup region; memory object; pager = object.pager || default_pager; else kill
  → build cluster [base, npages] within region; cookie bound to {S, object, cluster, …}
  → Call pager (mono timeout) with fault_msg { base, npages, access, cookie }
  → pager replies FAIL | OK + FRAME list (kernel-validated path)
  → kernel: validate cookie; validate FRAMEs; map views into S; pages owned by object
  → invalidate cookie; resume faulting thread or kill on fail/timeout
  → release S.fault_lock
```

*Solaris voice:* segment filler gets a **bounded fault context**, not the process keys to the city; HW tables updated only by the kernel after checks.

---

## 8. Fault message + timeout (P1)

### Decision: **Fixed message + kernel cookie; multi-page cluster fields**

```c
struct gj_fault_msg {
    u64 u64ClusterBase;   /* page-aligned */
    u32 u32NPages;        /* cluster size, >= 1, <= GJ_FAULT_CLUSTER_MAX */
    u32 u32Access;        /* R/W/X */
    u64 u64CookieLo;      /* kernel-only cookie (128-bit recommended) */
    u64 u64CookieHi;
    u32 u32Flags;
    u32 u32Pad;
};
/* Reply: status; FRAME identity passed via kernel-validated path (see impl) */
```

| Policy | v1 default |
|--------|------------|
| Timeout | Short mono (e.g. 100ms–1s); **kill** faulting thread |
| No pager | **Kill** |
| Cookie reuse | **Reject** |
| Cluster max | Compile-time cap (e.g. 16 pages) |

---

## 9. Pager endpoint lifecycle (P1)

### Decision: **Refcount endpoint on set_pager; hook revoke**

| Event | Action |
|-------|--------|
| `set_pager` | Resolve **ENDPOINT**; require **GRANT**; `ref++`; store PCB object pointer |
| Endpoint DEAD/revoke | Clear PCB pager for all processes; in-flight cookies for that pager **invalidate** |
| `clear_pager` | `ref--` |
| User slot overwrite of ep name | PCB **object ref** remains until clear/death |

**ENDPOINT only; GRANT required** (not NOTIFICATION / READ-only).

---

## 10. Nested Call / late reply / PI (P1)

### Decision: **Solaris-practical limits**

| Topic | Choice |
|-------|--------|
| **Max Call depth** | Small fixed (e.g. **8**); overflow → error |
| **Late reply** | Discard; reply cap already invalid; no double resume |
| **PI** | Turnstile PI along **one Call chain**; depth ≤ Call depth |
| **Budget** | Soft-RT budget still applies; PI raises priority within budget rules (no infinite boost) |

---

## 11. Revoke queue overflow + epoch (P1)

### Decision: **Never drop work; epoch = per-CPU quiescent**

| Topic | Choice |
|-------|--------|
| Queue full | Grow from reserved reclaim pool **or** run **sync slot clear** under try-lock loop with sleep (not spin) for this object only |
| Epoch | Each CPU notes idle/syscall boundary; reclaim when all CPUs have passed a generation counter (Solaris-like reaping, RCU-ish) |
| Debug | Panic if deferred work stalled past N seconds |

---

## 12. IOMMU API (P2)

### Decision: **DDI DMA window caps**

```
create_window(device, iova, frames[], rw) → window_cap
  only with IOMMU authority from devmgr
destroy/revoke window → disable HW first (1.1 Phase A), then free
```

No IOMMU on production device class → **no bus-master** (already policy).

---

## 13. HCL / support tiers (P2)

### Decision: **Ship tiers like Sun hardware compatibility**

| Tier | Meaning |
|------|---------|
| **T0** | QEMU virtio + UEFI OVMF — CI must pass |
| **T1** | Documented NVMe + AHCI + common USB HID |
| **T2** | One SAS HBA family (clean-room) when docs allow |
| **T3** | GPU accel opportunistic |

“General-purpose desktop” **means T0–T1 first**; SAS is on the roadmap, not day-one infinite HBAs.

---

## 14. TCB inventory (P2)

| Component | If compromised |
|-----------|----------------|
| Microkernel | Game over |
| `init` (pre-seal) | Game over |
| `init` (post-seal) | Limited to remaining caps |
| `devmgr` | All devices / DMA |
| Pager / `vfsd` | Memory + files of clients that use it |
| `ns` | Service bind/connect |
| Compositor | Input/display of its session |
| App | Only its CNode/quota |

*Solaris voice:* zones/doors reduce blast radius; still list what’s trusted.

---

## 15. `GJ_CAP_CNODE` in v1 (P2)

### Decision: **Type reserved; no user nested CNodes in v1**

Only process root CNode exists for apps. Nested CNodes = later server feature. Install of type CNODE from user → `NOSUPPORT` until then.

---

## 16. Spawn return: process cap

### Decision: **Kernel mints a PROCESS typed cap (task port) into parent CNode on spawn success**

Not derived from child’s root meta (kernel ops only). Parent `wait`/`kill` uses that handle.  
**Rights matrix and self-port rules:** [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 3.

---

## 17. What I would *not* do (as a Sun person)

| Avoid | Why |
|-------|-----|
| Full signal storm in kernel | Complexity; use notifications |
| STREAMS in kernel | Wrong century for GreenJade net |
| Ambient `fork` of whole CNode | Authority explosion |
| Pager with permanent map-any on client | Instant confused deputy |
| Infinite driver promises on day one | Sun always had an HCL |

---

## 18. Implementation order (Sun pragmatism)

1. Process/space/thread + CNode lock + root meta bootstrap  
2. Syscall table + cap mint/copy/move/identify  
3. Untyped retype + FRAME map  
4. IPC Call/Recv/Reply + transfer + PI  
5. set_pager kernel ref + fault Call + single-use map token  
6. virtio + then storage  
7. UEFI + big PMM + SMP enablement in parallel once UP paths work  

---

*Channel Solaris: ship doors-scale IPC, process-scale names, DDI-scale devices, and an honest compatibility tier list — with L4 teeth for revoke and quotas.*

---

## 19. Honesty refresh — remaining vs soft (Wave 85 · 2026-07-24)

**Additive only (Wave 85 exclusive for this file).** Design decisions in 1–18 stay **Accepted**. This section is a Wave 85 honesty ledger: what is still **remaining** to implement to product strength vs what is only **soft** on the tree. It does **not** re-litigate architecture. Wave 85 19 ledger text is superseded here as the same honesty formula under Wave 85 exclusive ownership of this file.

| Term | Meaning in this document |
|------|--------------------------|
| **Accepted** | Design frozen for Solaris channel — ship toward these rules |
| **Soft** | Partial / greppable / bring-up path exists; not full product close of the decision |
| **Remaining** | Decision accepted; code/product path still open or incomplete |
| **Soft continuum** | CREATE-ONLY libcgj graph parent wire; high-water **advancing toward 22700** soft only |
| **Product lamps** | Soft score / bar3-ready continuum stubs — remain **0** by design |
| **bar3** | Steam **client** on DUT + Deck Top 50 leave `NOT-TRIED` — **out of scope to claim here** |

### 19.1 Soft does not close remaining (or bar3)

| Soft surface (parallel waves / host) | Closes Solaris remaining? | Closes bar3? |
|--------------------------------------|---------------------------|--------------|
| Continuum high-water **toward 22700** CREATE-ONLY graph decades | **No** | **No** |
| Host Steam media inventory **READY** (`steam-bar3-check`) | **No** | **No** |
| Kernel smokes (doors timeout/peer, cap mint/CDT soft, io_uring min, HDA multi-stream, 768G soak, aarch64 M0) | **No** (may soft-touch related surfaces) | **No** |
| Live embeds (sessiond / netstackd / sshd / storaged / vfsd / shell / scsi_mid / hda_client) | **No** (product skeleton ≠ full 1–18) | **No** |
| Design freeze in this file | Decisions only | **No** |

**Hard rule:** never claim bar3 closed, Deck Top 50 title PASS, product lamps > 0, or “Solaris remaining done” from continuum soft gates (toward **22700**), media `STATUS=READY`, or design **Accepted** alone. **Soft ≠ product complete.** Matrix honesty lives in [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) / [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — this channel does not promote those rows.

**Hard stamp (Wave 85):** soft ship **≠** Solaris remaining closed **≠** bar3. Soft continuum **toward 22700** is CREATE-ONLY parent wire only. Product lamps **0**. **No bar3 claim.**

### 19.2 Per-decision ledger (design Accepted → ship honesty)

|  | Decision (short) | Soft on tree (honest bound) | Remaining (do not claim done) |
|---|------------------|-----------------------------|-------------------------------|
| **1** Untyped retype | Two classes + size ladder | `GJ_SYS_UNTYPED_RETYPE` / untyped counters soft path; no full RAM vs device resource-pool product | Full power-of-two ladder + recycle-to-parent + device-untyped only via `devmgr` |
| **2** CDT + `slots_left` | Explicit child list; slot quota | `cap: mint/copy/move+cdt PASS`; `slots_left` + soft CDT; empty-edge soft gap known | Full CDT edges everywhere; R2 try-lock slot walk in deferred revoke; no free infinite C-list product audit |
| **3** IPC cap transfer | Receiver alloc; all-or-nothing; small K | Doors message path + badge soft; **not** full Scheme A cap transfer K | Cap transfer (copy/move K caps); dest slot kernel-picked; partial-fail atomicity product |
| **4** Syscall ABI freeze | Stable NRs; Scheme A `(slot,gen)` | Native `GJ_SYS_*` + Linux hybrid live; not universal `gj_cap_resolve` on all ops | Freeze `syscall.h` with Apple 5 surface; no renumber after first userland contract |
| **5** CNode locking | Per-process CNode mutex; order CNode→Object→Endpoint | Exclusive lock on mutate/resolve v1 soft | Shared resolve / RCU-later; full lock-order audit under load |
| **6** Process lifecycle | Process owns space+CNode; LWPs; kernel PROCESS mint | Spawn + PROCESS cap to parent; live embeds; death path partial | Full reverse tear-down on spawn fail; complete revoke-all on last-thread exit product |
| **7–8** Pager map / fault msg | Cookie + cluster + kernel-only PTE; fault lock | Cookie/cluster/fault-lock **policy** in `fault.c`; cluster max compile bound | Region→Call pager→cookie validate→map **views**; single-use cookie end-to-end; kill-on-timeout product path |
| **9** Pager endpoint lifecycle | Ref on `set_pager`; clear on ep revoke | `gj_process_set_pager` + death clear soft | Full ENDPOINT GRANT ref++; ep DEAD/revoke clears all PCBs + invalidates in-flight cookies |
| **10** Nested Call / late reply / PI | Depth cap; discard late reply; turnstile PI | Doors single-flight + mono timeout; soft QoS rank | Fixed Call depth; late-reply discard; **turnstile PI** along Call chain within budget |
| **11** Revoke queue + epoch | Never drop; per-CPU quiescent | Deferred revoke from timer (R7); soft reclaim | Grow reclaim pool / sync clear; epoch across CPUs; stall panic debug |
| **12** IOMMU API | DDI DMA window caps | Probe + enforce windows + VT-d TE soft (`iommu:* PASS`) | `create_window` as true window **cap**; destroy/revoke HW-first product; no bus-master without IOMMU class |
| **13** HCL tiers | T0–T3 ship matrix | T0 virtio/OVMF CI path soft; [HCL.md](HCL.md) | Documented T1+ product matrix; no infinite driver promise |
| **14** TCB inventory | Blast-radius list | Inventory text only | Keep honest as servers gain caps; post-seal `init` limited |
| **15** `GJ_CAP_CNODE` v1 | Type reserved; no nested | Nested install → not product | Stay `NOSUPPORT` until server feature; no silent nested CNodes |
| **16** Spawn PROCESS cap | Kernel mint task port | Parent PROCESS mint on spawn live | Rights matrix + self-port rules fully per Apple 3 |
| **17–18** Avoid / order | Sun pragmatism | Implementation order partially started (process/CNode/doors/virtio soft) | Finish order without ambient fork, permanent pager map-any, or STREAMS |

### 19.3 Explicit non-claims (Wave 85)

| Claim | Allowed? |
|-------|----------|
| “Solaris channel design **Accepted**” | **Yes** — this document |
| “Soft CDT / doors / IOMMU / fault policy greppable” | **Yes** — with soft bound |
| “Soft continuum high-water advancing toward **22700** (CREATE-ONLY)” | **Yes** — soft only |
| “All 1–18 product-complete” | **No** |
| “Continuum toward 22700 closes Solaris remaining” | **No** |
| “Product lamps lit / product complete from soft continuum” | **No** — lamps remain **0** |
| “Media READY / soft smokes close bar3” | **No** |
| “Deck Top 50 titles tried / PASS from this doc” | **No** — matrix stays **NOT-TRIED** until real client runs |
| “Wave 85 honesty closes remaining or bar3” | **No** — honesty refresh only |

**Bar3 remains OPEN** (client + matrix). This Wave 85 edit is honesty-only on the Solaris remaining ledger: soft ship ≠ remaining closed; continuum **toward 22700 soft only**; product lamps **0**.

### 19.4 Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [TODO.md](TODO.md) — coding boxes (M2 CDT try-lock, set_pager full path, fault→pager Call, cap transfer, PI)  
- [IMPLEMENTATION.md](IMPLEMENTATION.md) · [HCL.md](HCL.md) — parallel soft stamp; continuum toward 22700; open bars  
- [X86_64_INTEL_PLATFORM.md](X86_64_INTEL_PLATFORM.md) 14 — soft IOMMU/platform ≠ product  
- [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 20 — soft shipped vs remaining open  

*Wave 85 channel Solaris: keep Accepted decisions; ship remaining product paths; never promote soft continuum **toward 22700** or media READY to bar3 or “remaining closed.” Product lamps **0**. **Soft ≠ product complete**.*
