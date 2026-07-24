# GreenJade — Capability Addressing (x86_64)

| Field | Value |
|-------|--------|
| **Document** | Cap addressing v1.7 |
| **Status** | **Accepted** for x86_64 product path (1–8); 9 Wave 12 honesty; 9.5 Wave 48 soft continuum |
| **Handle ABI** | **Scheme A** — `u64` slot index + separate generation |
| **Slot 0** | **Root meta** — **kernel ops only**; process + CNode identity; empty until bootstrap |
| **Fault/pager** | **Region → memory object → pager**; PCB **default pager** fallback ([APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 1) |
| **CNode scope** | **One CNode per process** — all threads in that process **share** it |
| **PROCESS** | Typed **task port** minted to parent on spawn — not root meta |
| **Honesty (Wave 12)** | Soft cap surface ≠ product multi-server confine; **no bar3 claim** (9) |
| **Honesty (Wave 48)** | Soft continuum toward 19000 ≠ product; product lamps **0**; **no bar3 claim** (9.5) |
| **Companion** | [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) · [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) (game tier; overrides Apple on conflict) · [IMPLEMENTATION.md](IMPLEMENTATION.md) · [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) |

---

## 1. Decision (normative)

| Layer | Choice |
|-------|--------|
| **Kernel authority** | seL4-style **CNodes** holding typed caps; **untyped** retype; **CDT** + revoke 1.1 |
| **App-visible handle (v1)** | **Scheme A:** root-relative **`u64` slot** + **`u32` slot_gen** (two syscall args / two registers) |
| **Slot 0** | **Root meta** — reserved; not a general-purpose install target |
| **IPC** | Doors-like Call/Recv/Reply on endpoint caps |
| **Not v1** | Packed `(gen<<32)|slot` in one `u64`; multi-level CPtr for every app call |
| **Never** | Hardware tagged pointers (IBM i); SPARC ASI naming |

```text
x86_64 user / syscall:
  arg0 = u64Slot      // full 64-bit index
  arg1 = u32SlotGen   // must match slot.u32Gen
        │
        ▼
  current process → one shared CNode → pSlots[u64Slot]
  (all threads in the process use this same table)
        │  check slot.u32Gen == u32SlotGen
        ▼
  pObj = slot.pObj
        │  check LIVE && pObj->u32Gen == slot.u32ObjGen
        ▼
  use with slot.u16Rights
```

---

## 2. Why Scheme A on x86_64

| Concern | Scheme A |
|---------|----------|
| Slot width | Full **64-bit** index (headroom; size still quota-bounded) |
| Syscall ABI | Natural: `rdi` = slot, `rsi` = gen (System V AMD64) |
| No pack/unpack bugs | Gen never steals high bits of slot |
| SMEP/SMAP | Still never pass kernel pointers as handles |

Packed 32+32 was rejected in favor of this.

---

## 3. Handle representation

### User / ABI

```c
/* Two values — not a single packed u64 */
u64  u64Slot;     /* CNode index; 0 = root meta */
u32  u32SlotGen;  /* must match kernel slot.u32Gen */
```

Optional struct for C APIs:

```c
struct gj_cap_ref {
    u64 u64Slot;
    u32 u32SlotGen;
};
```

### Null / invalid

| Encoding | Meaning |
|----------|---------|
| `u32SlotGen == 0` | **Null / invalid handle** (gens on live slots start at 1) |
| `u64Slot` out of range | `GJ_ERR_INVAL` |
| Slot type `INVALID` | `GJ_ERR_NOENT` |
| Gen mismatch | `GJ_ERR_STALE_CAP` |

### Slot 0 — root meta (normative contents)

| Rule | Detail |
|------|--------|
| **Reserved** | Index `0` is **root meta** only (`GJ_CAP_ROOT_META`) |
| **Empty until bootstrap** | After `gj_cnode_init`, slot 0 may be `INVALID` until kernel/`init` installs meta |
| **Cap to self process** | **Yes** — as **kernel-internal** identity on the root meta object (not a transferable Scheme A PROCESS cap) |
| **Cap to this CNode** | **Yes** — as **kernel-internal** link to the process CNode (not a transferable Scheme A CNODE cap minted from slot 0) |
| **User model** | **Kernel ops only** (see below) |
| **Not** general mint target | `gj_cap_alloc_install` starts at slot 1; only bootstrap installs slot 0 |
| **User resolve** | `(slot=0, gen)` may **Identify** as `ROOT_META`; no raw kernel pointers returned |

**Root meta is identity for the kernel and limited query ops — not a factory for process/CNode caps, and not the fault path.**

### Root meta — kernel ops only (normative)

| Rule | Detail |
|------|--------|
| **K1** | `GJ_CAP_ROOT_META` supports **kernel-mediated operations only** (syscalls that take slot 0 / process context). |
| **K2** | Resolving slot 0 does **not** yield a portable object userspace can Grant/Mint into another process as “the process” or “the CNode.” |
| **K3** | No `mint_process_cap` / `mint_cnode_cap` from root meta in v1. Need those later ⇒ separate typed caps installed by **kernel/`init` policy**, not generic meta derivation. |
| **K4** | Allowed examples: identify “this is my root meta”; kernel-internal `current_process()` / `current_cnode()` after auth; debug/info under capability-gated syscalls. |
| **K5** | Disallowed: using slot 0 as IPC transfer payload meaning “here is authority over my CNode”; treating meta as doors server endpoint. |
| **K6** | Pager remains **PCB registration** (`set_pager`), independent of root meta. |

**Rationale:** keeps slot 0 a small, non-ambient identity object; process/CNode authority stays in explicit kernel paths and future typed caps, not a confused-deputy-friendly “meta is everything.”

### Fault / pager endpoint — decision

#### Recommendation (accepted): **canonical registration on the process object**

| Choice | Detail |
|--------|--------|
| **Where the kernel looks on page fault** | **Process control block** field: registered **endpoint** (and optional badge) for the pager — **not** “read slot 0” on every fault |
| **Empty until filled** | **Yes** — default **no pager** ⇒ faulting thread/space is **killed** (fail closed), or blocked only if a policy cap allows wait (v1: kill is fine) |
| **Who registers** | `init` / libc / `vfsd` via privileged op: `sys_process_set_pager(endpoint_slot, endpoint_gen)` (must hold GRANT on that endpoint and authority over the process) |
| **Solaris analogy** | Segment/HAT fault → call out to filler; registration is process state, not a magic FD number required for correctness |
| **Doors analogy** | Pager is a **door server** (often another process); faulting process holds a **send right** (endpoint cap) the kernel uses on fault |

#### Why not put the pager only in slot 0?

| Issue | Why it hurts |
|-------|----------------|
| Slot 0 is already **identity** (process + CNode) | Overloading meta with pager couples identity and memory policy |
| Every fault would resolve slot 0 | Extra CNode lock/traffic; harder to reason under SMP |
| User can scramble slot 0 gen | Accidental self-DoS of all faults if pager were “whatever is in slot 0” |
| Pager is often **another process** | You store an **endpoint cap to that server**, best as process registration (+ optional user-visible slot) |

#### Why not *only* a well-known CNode slot (e.g. slot 1)?

| Issue | Why |
|-------|-----|
| Correctness depends on user not clobbering slot 1 | Less safe unless kernel also mirrors into PCB |
| Dual source of truth if both PCB and slot | Drift unless install path updates both |

#### Optional later (not required for v1 correctness)

| Optional | Detail |
|----------|--------|
| **Well-known slot 1** `GJ_CAP_SLOT_PAGER` | User-visible mirror for introspection / `door_call` style “call my pager”; kernel **still** uses PCB field as canonical |
| Sync: `set_pager` installs into PCB **and** slot 1 | Single syscall keeps them aligned |

#### Fault path (normative — SOLARIS cookie rules + APPLE regions/objects)

See [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 1–2 and [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) 7.

```
user page fault
  → process P, space S; acquire S.fault_lock  (one fault lock per space)
  → lookup region at VA; if none → kill; unlock
  → object = region.memory_object
  → pager = object.pager || P.default_pager (PCB); if none → kill; unlock
  → build multi-page cluster within region (capped)
  → create kernel-only map cookie bound to {S, object, cluster, access, thread, deadline}
  → Call pager (mono timeout) with fault_msg + cookie
  → pager replies FAIL or OK + FRAMEs (pager never ambient-maps client)
  → kernel validates cookie; maps PTEs as **views**; pages owned by **memory object**
  → invalidate cookie; resume or kill; release S.fault_lock
```

| Policy | v1 default |
|--------|------------|
| No region / no pager | **Kill** faulting thread (fail closed) |
| Pager timeout / bad cookie | **Kill** faulting thread; log |
| Token | **Kernel-only cookie** in message (not a CNode cap) |
| Cluster | **Multi-page**, size-capped, region-bounded |
| Page ownership after map | **Memory object owns pages; maps are views** |
| Concurrency | **One fault lock per space** |
| File-backed / anon | File/anon **memory objects**; object pager or default |

#### How this fits root meta

| Item | Where |
|------|--------|
| Cap to self process (kernel identity) | **Root meta (slot 0)** — kernel-internal only |
| Cap to this CNode (kernel identity) | **Root meta (slot 0)** — kernel-internal only |
| Transferable **task** handle | **`GJ_CAP_PROCESS`** minted to **parent** on spawn — not root meta |
| Empty until bootstrap | Slot 0 and default pager **empty until bootstrap** |
| Default pager endpoint | **Process PCB** fallback; per-region object pagers preferred |
| Exception port | **PCB**, separate from pager ([APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 12) |

```text
Process (task)
  ├── CNode (shared by threads)
  │     slot 0: ROOT_META ──► { process self, cnode self }  // kernel ops only
  │     slot 1..: normal caps (incl. optional mirrors)
  ├── default_pager   ◄── PCB fallback, empty until set_pager
  ├── exception_ep    ◄── optional, not pager
  ├── ledger          ◄── quota account
  └── space
        └── regions → memory_objects → (pager, pages)
```

---

## 4. Slot contents (kernel)

```text
struct gj_cap_slot {
    u16  u16Type;
    u16  u16Rights;
    u32  u32Gen;       /* handle.u32SlotGen */
    u32  u32ObjGen;    /* object gen at install */
    void *pObj;
};
```

CNode may have `cSlots` up to implementation max; index type is **u64** for ABI, practical size still limited by allocated table + quota.

### Resolve algorithm

```
1. if u32SlotGen == 0 → INVAL (null handle)
2. pCnode = current process CNode (shared by all threads of that process)
3. if u64Slot >= pCnode->cSlots → INVAL
4. pSlot = &pCnode->pSlots[u64Slot]
5. if type == INVALID → NOENT
6. if pSlot->u32Gen != u32SlotGen → STALE_CAP
7. pObj = pSlot->pObj
8. if !LIVE → DEAD
9. if pObj->u32Gen != pSlot->u32ObjGen → STALE_CAP / DEAD
10. OK + rights
```

---

## 5. Root CNode (v1)

| Property | Value |
|----------|--------|
| Binding | **Exactly one CNode per process** (address space) |
| Threads | **Share** that CNode — no per-thread tables |
| Slot 0 | Root meta |
| Default size | e.g. 1024 slots (configurable; grow later via quota) |
| Destroy | Process death ⇒ revoke/teardown entire CNode (1.1) |

---

## 6. Rights (unchanged)

```text
GJ_RIGHT_READ, WRITE, GRANT, MINT, DESTROY, MAP, IDENTIFY
```

---

## 7. Explicit non-goals

| Non-goal | Reason |
|----------|--------|
| Pack slot+gen in one u64 | Scheme A chosen |
| 32-bit-only slot | ABI uses u64 slot |
| Per-thread private CNode | Desktop multi-thread model; **process shares one CNode** |
| HW tagged user pointers | Wrong arch |

---

## 8. Code map

| File | Role |
|------|------|
| `kernel/include/gj/cap.h` | `gj_cap_ref`, resolve/install |
| `kernel/cap/cnode.c` | Init, resolve, install (skip slot 0 on alloc) |
| `kernel/cap/revoke.c` | DEAD/gen first + deferred slot invalidate |

---

## 9. Honesty bounds — soft cap surface vs product multi-server (Wave 12 · 2026-07-23)

**Additive only (Wave 12 exclusive for this file).** Scheme A, root meta (K1–K6), one CNode per process, PROCESS mint-to-parent, fault/pager registration, rights, and the resolve algorithm in 1–8 stay **Accepted**. This section is a Wave 12 honesty ledger: what is greppable **soft cap surface** on the tree vs what remains **product-open** multi-server confine. It does **not** re-litigate the handle ABI, close any product bar, invent multi-server product completion, or claim **bar3**.

| Term | Meaning in this document |
|------|--------------------------|
| **Accepted** | Addressing design frozen for x86_64 — ship toward these rules |
| **Soft cap surface** | Scheme A resolve/install, mint/copy/move, soft CDT/quota/trylock, root-meta bootstrap, PROCESS parent mint — greppable / bring-up strength |
| **Product multi-server confine** | Full multi-server drop-ambient security product (servers + clients confined by caps/promises end-to-end) — **open** |
| **bar3** | Steam **client** on DUT + Deck Top 50 leave `NOT-TRIED` — **out of scope to claim here** |

**Hard stamp (Wave 12):** soft cap surface **≠** product multi-server confine. Soft greppable paths do **not** ship sealed multi-server product. **No bar3 claim** from this document.

### 9.1 Soft cap surface (honest bound — may claim soft only)

| Soft surface | What shipped / greppable means | What it does **not** mean |
|--------------|--------------------------------|---------------------------|
| **Scheme A handle** | `(u64 slot, u32 gen)` resolve path; null gen=0; gen mismatch → stale | Universal `gj_cap_resolve` on every product op; packed-u64 never “also OK” |
| **Root meta slot 0** | Bootstrap install + identify; kernel ops only (K1–K6) | Factory for process/CNode caps; ambient identity transfer |
| **One CNode / process** | Shared table; alloc skips slot 0 | Nested `GJ_CAP_CNODE` product; per-thread private tables |
| **mint / copy / move / delete** | Soft path + CDT edge hooks; greppable e.g. `cap: mint/copy/move+cdt PASS` | Full product CDT everywhere; IPC cap-transfer K complete |
| **Soft CDT + try-lock** | Edge list, deferred walk, soft `u32SoftLock` (R2 stub); greps `cap:cdt*` | Full R2 product try-lock under load; accounting-complete revoke audit closed |
| **Soft slot quota** | Flat/hierarchical ledger; NULL account no-op; greppable e.g. `cap: quota exhaust PASS` | Zone-like product ledgers for every object class |
| **PROCESS task port** | Parent mint on spawn (not root meta) | Full rights matrix product-hard for all callers |
| **Revoke DEAD/gen first** | Phase A fail-closed + deferred hygiene hooks | Stop-the-world never; full Phase C reclaim product under all pins |
| **Confine / promises soft** | Process bitmask gates some ambient-style Linux paths (`confine soft`) | **Product multi-server confine closed** |

**Hard rule:** soft greppable `PASS` lines and design **Accepted** are **bring-up / agent honesty**, not “capability product shipped” and not “multi-server confine done.”  
**Wave 12 formula:** **soft cap surface ≠ product multi-server.**

### 9.2 Product multi-server confine — **open**

| Item | State |
|------|--------|
| Soft cap surface (this doc 1–8 + `kernel/cap/*`) | Present — soft / greppable |
| Soft confine / promise gates on some paths | Soft only — not product seal |
| Live server embeds (`vfsd`, `sessiond`, `netstackd`, …) | Skeleton / door bring-up ≠ full confine product |
| **Full multi-server confine product** | **Open** — soft cap surface does **not** close it |
| Bootstrap seal (drop ambient retype / broad authority) | **Open** (companion security docs) |
| Cap transfer on IPC (copy/move K caps, all-or-nothing) | **Open** / incomplete product path |

Soft cap surface and soft promise gates are **not** a claim that every server and client run under sealed multi-server confinement. Product multi-server confine stays **open** until that path is actually product-complete (see [TODO.md](TODO.md) multi-server / confine items and [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md)).

### 9.3 Explicit non-claims (Wave 12)

| Claim | Allowed? |
|-------|----------|
| “Scheme A / root meta / one CNode design **Accepted**” | **Yes** — this document 1–8 |
| “Soft cap surface greppable (mint/CDT/quota/root meta)” | **Yes** — with soft bound in 9.1 |
| “Soft cap surface = product multi-server confine closed” | **No** |
| “All revoke / CDT / quota / IPC transfer product-complete” | **No** |
| “Continuum soft gates / media `STATUS=READY` close bar3” | **No** |
| “Deck Top 50 titles tried / PASS from this doc” | **No** — matrix stays **NOT-TRIED** until real client runs |
| Any **bar3** closed claim from cap addressing alone | **No** |

**Bar3 remains OPEN** (client + matrix). This Wave 12 edit is honesty-only: **soft cap surface ≠ product multi-server confine; no bar3 claim.** Matrix honesty lives in [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) / [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — this document does not promote those rows.

### 9.4 Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) — CDT/revoke/quota remaining vs soft  
- [TODO.md](TODO.md) — multi-server confine / CDT try-lock product / cap transfer open boxes  
- [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) · [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) — soft surface ≠ product bar  

### 9.5 Soft continuum note (Wave 48 · 2026-07-23)

**Additive only (Wave 48 exclusive for this file).** 9 Wave 12 soft-cap bounds stay in force. Soft deepen on non-w13 surfaces does **not** re-litigate Scheme A or close multi-server confine.

| Soft surface | Bound |
|--------------|--------|
| Continuum high-water **advancing toward 19000** | Parent-wire CREATE-ONLY soft only — **not** product multi-server; **not** bar3 |
| Soft deepen (non-w13) | Honesty / bring-up only — **soft ≠ product complete** |
| Product lamps | Remain **0** by design |
| Deck Top 50 | **NOT-TRIED × 50** — no title PASS from this doc |
| **bar3** | Still **OPEN** |

**Hard stamp (Wave 48):** soft continuum toward **19000** ≠ product multi-server confine ≠ bar3. Soft cap surface still ≠ product multi-server. **No bar3 claim.** Product lamps **0**.

---

*Accepted x86_64 capability addressing v1.7 — Scheme A, root meta, fault cookie / multi-page cluster, per-space fault lock.*  
*9 Wave 12 honesty (2026-07-23): soft cap surface ≠ product multi-server confine; **no bar3 claim**.*  
*9.5 Wave 48 soft continuum (2026-07-23): high-water **toward 19000 soft only**; product lamps **0**; **soft ≠ product complete**; **bar3 remains OPEN**.*
