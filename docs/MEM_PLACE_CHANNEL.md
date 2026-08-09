# GreenJade — Memory Placement Channels (Tailslayer-inspired)

| Field | Value |
|-------|--------|
| **Status** | **Soft residual L0** — Option A userspace hedge wired; **not** product DRAM channel map |
| **Law** | Dual **MIT OR Apache-2.0**; freestanding pure C11; **Soft ≠ product** |
| **Security** | **Physical addresses NEVER exposed to userspace**; opaque soft channel class 0..N−1 per object only |
| **Code** | `kernel/include/gj/memobj.h` · `kernel/mm/memobj.c` · `kernel/syscall/native.c` · `user/libgj/include/gj/syscalls.h` |
| **Companions** | [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) · [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) · [PURE_C_CONCURRENCY_AND_OPS.md](PURE_C_CONCURRENCY_AND_OPS.md) |

**Soft ≠ product.** Soft striping / soft class diversify does **not** claim product multi-channel DRAM hedge, NUMA placement, or refresh-domain isolation.

---

## 1. Goal

Provide a **memory placement ABI** so a userspace client (e.g. Tailslayer-style load hedge) can:

1. Ask the kernel for **N replica views** of the same logical object.
2. Map each replica at a **distinct user VA**.
3. **Race reads** across those VAs (userspace hedge).
4. Never learn **physical addresses** or a product phys-bit channel recipe.

Kernel owns frame placement. Success with a hard `u32MinChannels` means **distinct soft channel classes** were achieved (or the call **fails closed**).

---

## 2. Option A vs Option B

| Option | Who hedges | Userspace sees | Soft residual | Product |
|--------|------------|----------------|---------------|---------|
| **A (ship soft)** | Userspace races N VAs | Opaque name + replica index → VA | L0 soft diversify | OPEN (L2 channel map) |
| **B (stub only)** | Kernel `hedge_load` / single VA | One VA; kernel picks replica | **Not implemented** | OPEN |

### Option A — N replica VAs (implemented soft)

```text
  CREATE_PLACED(cPages, place_req) → name + replicas + soft_channels
       │
       ├─ MAP_REPLICA(name, 0, hint, prot) → VA0
       ├─ MAP_REPLICA(name, 1, hint, prot) → VA1
       └─ MAP_REPLICA(name, r, …)          → VAr
       │
       userspace: race load from VA0..VAr  (software hedge)
```

- One **named** memobj holds `cPagesLogical * cReplicas` frames.
- Replica `r` is pages `[r * cPagesLogical, (r+1) * cPagesLogical)`.
- Each `MAP_REPLICA` installs **only** that slice as a G-MAP-2 USER region.
- Frames may sit on different soft L0 classes (best-effort or hard min).

### Option B — Kernel hedge (stub only)

```text
  /* OPEN product: kernel hedge_load / single-VA multi-frame pick.
   * Not implemented in this residual. Soft path remains Option A only.
   * greppable: MEM_PLACE_OPTION_B_STUB hedge_load OPEN
   */
```

Do **not** implement Option B in soft residual code paths.

---

## 3. Security (normative)

| Rule | Soft residual |
|------|----------------|
| **No PA leak** | Never `copyout` PA; `memobj_page_pa` stays **kernel-only** (futex shared-key helper) |
| **Opaque class** | Userspace may learn **count** of soft classes achieved (`u32SoftChannels`); not per-page PA or class bit recipe for product DRAM |
| **No phys bit recipe** | Soft L0 class = `(pa >> 12) & (SOFT_CHANS-1)` — **not** a product channel map; do not document as HW channel ID |
| **USER maps only** | Product maps always force `GJ_VMM_PROT_USER` and G-MAP-2 window |
| **Fail closed** | Hard `u32MinChannels` not met → destroy object, return `GJ_ERR_NOSUPPORT` |

**greppable:** `MEM_PLACE_NO_PA_LEAK` · `Soft!=product` · `G-MAP-2`

---

## 4. Guarantee levels

| Level | Meaning | Soft residual | Product |
|-------|---------|---------------|---------|
| **L0** | Soft diversify: retry `pmm_alloc`, free rejects; class from low PA bits / page index | **Wired** (best-effort + hard min fail-closed) | Not a DRAM map |
| **L1** | NUMA / node preference (`u32NodePref`) | **Ignored soft** (reserved) | OPEN |
| **L2** | Product channel-class map (real DIMM/channel topology) | **OPEN** — no full DRAM channel map | OPEN |
| **L3** | Refresh-domain / bank isolation | **OPEN** | OPEN |

Current claim: **L0 soft striping + fail-closed product-shaped ABI**. Product L2 remains **OPEN** (see [TODO.md](TODO.md)).

**greppable:** `MEM_PLACE_L0` · `MEM_PLACE_L2_OPEN` · `CHAN_STRIPED residual`

---

## 5. Soft model (one object, replica page layout)

Recommended soft model (keep simple):

```text
  primary logical size: cPagesLogical frames per replica
  total frames:         cTotal = cPagesLogical * cReplicas
  constrain:            cTotal <= GJ_MEMOBJ_MAX_PAGES else fail INVAL/NOMEM
  layout:               replica r → aPa[r * cPagesLogical + i], i in [0, cPagesLogical)
```

| Field on `gj_memobj` | Role |
|----------------------|------|
| `cPages` | Total owned frames (`cPagesLogical * cReplicas`) |
| `cPagesLogical` | Pages per replica |
| `cReplicas` | 1..`GJ_MEM_PLACE_MAX_REPLICAS` (4) |
| `u32SoftChannels` | Achieved distinct soft L0 class count |
| `u32Flags` | `GJ_MEMOBJ_F_CHAN_STRIPED` when place striped |

Soft L0 diversify (kernel-only):

1. Soft class = `(pa >> 12) & 3` (4 soft classes; **not** product channel).
2. When allocating replica `r`’s first page, prefer a class not yet used by prior replicas; retry `pmm_alloc` a few times; free rejects.
3. Remaining pages of that replica: best-effort.
4. If `u32MinChannels == 0`: keep best effort; set `u32SoftChannels` to achieved count.
5. If `u32MinChannels > 0` and achieved `< min`: **destroy** object, return NULL / `GJ_ERR_NOSUPPORT`.

**greppable:** `memobj: soft place CHAN_STRIPED residual` · `memobj: soft place PASS` · `Soft!=product`

---

## 6. Syscall ABI

Numbers (after DDI 103; frozen for this residual):

| NR | Name | Args | Return |
|----|------|------|--------|
| **104** | `GJ_SYS_MEMOBJ_CREATE_PLACED` | arg0 = user `gj_mem_place_req *` · arg1 = `cPages` logical · arg2 = user `gj_mem_place_out *` | `0` or `GJ_ERR_*` |
| **105** | `GJ_SYS_MEMOBJ_MAP_REPLICA` | arg0 = name ptr · arg1 = replica index · arg2 = hint · arg3 = prot | VA or `0` |

### `struct gj_mem_place_req`

```c
#define GJ_MEM_PLACE_DEFAULT       0u
#define GJ_MEM_PLACE_CHAN_STRIPED  (1u << 1)
#define GJ_MEM_PLACE_MAX_REPLICAS  4u

struct gj_mem_place_req {
    u32 u32Flags;        /* GJ_MEM_PLACE_* */
    u32 u32Replicas;     /* 1..MAX */
    u32 u32MinChannels;  /* 0 = best effort; else hard min distinct soft classes */
    u32 u32NodePref;     /* reserved L1; ignore soft */
};
```

### `struct gj_mem_place_out`

```c
struct gj_mem_place_out {
    char szName[32];       /* auto name e.g. placed-%u */
    u32  u32Replicas;
    u32  u32LogicalPages;
    u32  u32Flags;         /* echo place flags / object flags soft */
    u32  u32SoftChannels;  /* achieved soft L0 class count; 0 if unknown */
};
```

### User helpers (`user/libgj/include/gj/syscalls.h`)

```c
long gj_memobj_create_placed(const struct gj_mem_place_req *req, unsigned cPages,
                             struct gj_mem_place_out *out);
long gj_memobj_map_replica(const char *name, unsigned replica,
                           unsigned long hint, unsigned prot);
```

### Kernel APIs (`gj/memobj.h`)

```c
struct gj_memobj *memobj_create_placed(u32 cPagesLogical,
                                       const struct gj_mem_place_req *pReq);
gj_vaddr_t memobj_map_replica(struct gj_process *pProc, struct gj_memobj *pObj,
                              u32 u32Replica, u64 u64Hint, u32 u32Prot, int fFixed);
u32 memobj_placed_replicas(const struct gj_memobj *pObj);
u32 memobj_placed_logical_pages(const struct gj_memobj *pObj);
gj_status_t memobj_place_last_status(void); /* last create_placed errno soft */
```

---

## 7. Soft residual lamps (greppable)

| Marker | Meaning |
|--------|---------|
| `memobj: soft place CHAN_STRIPED residual` | Place path residual present |
| `memobj: soft place PASS` | Soft place residual self-check ok |
| `memobj: place create` | CREATE_PLACED success |
| `memobj: place map replica` | MAP_REPLICA success |
| `memobj: place diversify` | Soft L0 diversify note |
| `MEM_PLACE_L0` / `MEM_PLACE_L2_OPEN` | Guarantee level honesty |
| `MEM_PLACE_NO_PA_LEAK` | Security contract |
| `MEM_PLACE_OPTION_B_STUB` | Option B not implemented |
| `Soft!=product` | Soft residual ≠ product AC |

---

## 8. Explicit non-goals (this residual)

- Full DRAM / IMC channel map (L2) — **OPEN**
- NUMA node placement (L1) — reserved arg only
- Option B kernel `hedge_load`
- Exposing PA or product phys-bit channel IDs to userspace
- Touching NIC / rtl8168 / UDX product path
- Bumping `GJ_IMAGE_VERSION`

---

*Soft residual Option A only. Product channel placement remains OPEN. Soft ≠ product.*
