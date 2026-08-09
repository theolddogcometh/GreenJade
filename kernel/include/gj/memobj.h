/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Memory objects own frames; VMM only installs PTEs (G-MO-*).
 *
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0.
 * Implementation: kernel/mm/memobj.c.
 * Soft!=product. G-AC-1: no Linux .ko product path; product drivers are
 * Linux-shaped UDX/DDI userspace hosts over hot+cold ABI.
 *
 * Scope
 * -----
 * Apple-channel VM model (docs/APPLE_CHANNEL_REMAINING.md): a memory object
 * is the owner of physical frames; process regions are *views* (G-MO-3
 * multi-map). Product user maps are always USER and confined to
 * [GJ_USER_VA_BASE, GJ_USER_VA_END) (G-MAP-2, user_access.h).
 *
 * Design anchors
 * --------------
 *   docs/DESIGN_SPEC_COMPLETE.md     G-MO-1..4, G-MAP-2, G-AS-1
 *   docs/APPLE_CHANNEL_REMAINING.md  objects vs maps; wine-shm share
 *   docs/CAP_ADDRESSING.md           GJ_CAP_MEMORY_OBJECT / obj_hdr
 *   docs/PROTON_PERSONALITY.md       wine-server shm names
 *   docs/SECURITY_CORE_DESIGN.md     no ambient map; fail closed
 *   docs/DDI_SOFT.md / UDX README    userspace hosts; mmap + cap windows eng
 *
 * Soft product surface (this module only)
 * ---------------------------------------
 *   G-MO-1   anon mmap -> object + PTEs
 *   G-MO-3   named shareable maps (wine-shm / winesrv-shm / mfd:*)
 *   FILE    soft file-backed mmap via vfs_ram fd snapshot (not live pager)
 *   G-MAP-2  USER map flags always forced on product maps
 *   region   fixed GJ_PROC_REGION_MAX table with soft full / reuse / overlap
 *   G-AS     process_as_ensure / process_as_activate (private CR3)
 *
 * UDX mmap / cap windows eng residual (soft only; this unit)
 * ---------------------------------------------------------
 *   USER VA window          - all product maps confined (G-MAP-2)
 *   GJ_CAP_MEMORY_OBJECT    - object kind behind future mint (mint OPEN here)
 *   shareable multi-map     - G-MO-3 views for host/shared buffers
 *   private AS ensure       - UDX hosts run in userspace CR3
 *   Soft!=product: no live MEMORY_OBJECT / DMA window cap mint in this TU
 *   G-AC-1: no .ko; Linux-shaped UDX hosts stay userspace
 *
 * Lean named residual for UDX host maps (soft only; this unit)
 * -----------------------------------------------------------
 *   named path              - create_named | lookup | map_named | map_share | unlink
 *   G-MO-3 multi-map        - F_SHAREABLE views (ring/bounce-style host buffers)
 *   sticky publish          - F_NAMED until unlink; reclaim only when idle
 *   USER / G-MAP-2          - product host maps always force U + window check
 *   Soft!=product dual MIT OR Apache-2.0; MEMORY_OBJECT mint OPEN; G-AC-1
 *
 * Object kinds
 * ------------
 *   ANON   - private-ish frames; shareable only if F_SHAREABLE
 *   FILE   - soft snapshot from vfs_ram regular fd; full pager remains OPEN
 *   NAMED  - published in soft name table; multi-process map (G-MO-3)
 *
 * Layering
 * --------
 *   pmm_alloc*  -> frames stored in gj_memobj.aPa[]
 *   vmm_map_page under process CR3 after process_as_ensure
 *   region row on gj_process.aRegions[] (view: va, len, prot, pObj)
 *   fault / pager may bind object pointer on map cookie (soft)
 *   futex shared key: memobj_page_pa (PA of word - Linux-true G-MO-3)
 *
 * Placement channels (Tailslayer-inspired Option A; soft residual)
 * ----------------------------------------------------------------
 *   docs/MEM_PLACE_CHANNEL.md  Option A N VAs; no PA leak; L0 soft diversify
 *   CREATE_PLACED / MAP_REPLICA syscalls 104/105
 *   Soft L0: (pa >> 12) & 3 class diversify; L2 product channel map OPEN
 *   Soft!=product: not DRAM channel product; fail-closed min_channels
 *
 * Grep markers (kernel/mm/memobj.c)
 * ---------------------------------
 *   memobj: named
 *   memobj: share
 *   memobj: region table soft
 *   memobj: USER map
 *   memobj: file map soft
 *   memobj: file create soft
 *   memobj: soft map_file PASS   - first soft vfs_ram file-map success
 *   memobj: soft honesty
 *   memobj: soft inventory
 *   memobj: soft residual lean
 *   memobj: soft residual lean PASS
 *   memobj: soft residual UDX mmap/cap windows
 *   memobj: soft residual named
 *   memobj: soft residual named UDX host maps
 *   memobj: soft residual named PASS
 *   memobj: soft place CHAN_STRIPED residual
 *   memobj: soft place PASS
 *   memobj: place create | place map replica | place diversify
 *   wine-shm
 *
 * greppable: G-MO-1 G-MO-3 G-MAP-2 MEMOBJ_NAMED MEMOBJ_SHARE MEMOBJ_REGION
 * greppable: MEMOBJ_FILE_SOFT MEMOBJ_UDX_MMAP_CAP MEMOBJ_NAMED_UDX_HOST
 * greppable: memobj: soft map_file PASS
 * greppable: MEM_PLACE_L0 MEM_PLACE_L2_OPEN MEM_PLACE_NO_PA_LEAK
 * greppable: MEM_PLACE_OPTION_B_STUB CHAN_STRIPED
 * greppable: Soft!=product G-AC-1 dual MIT OR Apache-2.0
 */
#pragma once

#include <gj/cap.h>
#include <gj/error.h>
#include <gj/types.h>

struct gj_process;

/**
 * Memory object kind (u32Kind). FILE soft path snapshots vfs_ram bytes;
 * a live demand-pager remains OPEN (not product-complete).
 */
enum gj_memobj_kind {
    GJ_MEMOBJ_ANON  = 1,
    GJ_MEMOBJ_FILE  = 2, /* soft vfs_ram snapshot; full pager OPEN */
    GJ_MEMOBJ_NAMED = 3, /* published in named registry (shareable) */
};

/** Soft object flags (u32Flags). */
#define GJ_MEMOBJ_F_SHAREABLE    (1u << 0) /* multi-map ok (G-MO-3) */
#define GJ_MEMOBJ_F_NAMED        (1u << 1) /* still in named publish table */
#define GJ_MEMOBJ_F_ZEROED       (1u << 2) /* frames zeroed at create */
#define GJ_MEMOBJ_F_CHAN_STRIPED (1u << 3) /* placed multi-replica soft L0 */

/**
 * Bring-up frame cap per object (not a product TiB limit).
 * Larger maps need multi-object or future large-object path.
 * Placed objects use cPagesLogical * cReplicas <= MAX.
 */
#define GJ_MEMOBJ_MAX_PAGES 256u

/** Named registry key capacity including trailing NUL (31 chars usable). */
#define GJ_MEMOBJ_NAME_MAX  32

/* ---- Placement (Option A; docs/MEM_PLACE_CHANNEL.md) -------------------- */

/** Place request flags (gj_mem_place_req.u32Flags). Soft residual. */
#define GJ_MEM_PLACE_DEFAULT      0u
#define GJ_MEM_PLACE_CHAN_STRIPED (1u << 1)
/** Max replicas per placed object (Option A soft). */
#define GJ_MEM_PLACE_MAX_REPLICAS 4u
/** Soft L0 channel class count (not product DRAM map). Soft!=product. */
#define GJ_MEM_PLACE_SOFT_CHANS   4u

/**
 * CREATE_PLACED request (copyin from userspace).
 * u32MinChannels: 0 = best effort; else hard min distinct soft L0 classes.
 * u32NodePref: reserved L1 NUMA; ignored soft.
 * greppable: MEM_PLACE_L0 MEM_PLACE_L2_OPEN Soft!=product
 */
struct gj_mem_place_req {
    u32 u32Flags;        /* GJ_MEM_PLACE_* */
    u32 u32Replicas;     /* 1..GJ_MEM_PLACE_MAX_REPLICAS */
    u32 u32MinChannels;  /* 0 best effort; else hard min soft classes */
    u32 u32NodePref;     /* reserved; ignore soft */
};

/**
 * CREATE_PLACED result (copyout to userspace). Never includes PA.
 * greppable: MEM_PLACE_NO_PA_LEAK
 */
struct gj_mem_place_out {
    char szName[GJ_MEMOBJ_NAME_MAX];
    u32  u32Replicas;
    u32  u32LogicalPages;
    u32  u32Flags;
    u32  u32SoftChannels; /* achieved soft L0 class count; 0 if unknown */
};

/**
 * Kernel memory object (cap type GJ_CAP_MEMORY_OBJECT when minted).
 *
 * hdr            - lifecycle (LIVE/DEAD/gen/ref/pin/CDT) via gj_obj_hdr
 * cPages         - valid entries in aPa[0 .. cPages) (total frames)
 * cMapped        - live region views across processes (destroy when 0 + !named)
 * cReplicas      - 0 = not placed; else 1..MAX; pages = cPagesLogical * cReplicas
 * cPagesLogical  - pages per replica (0 if not placed)
 * u32SoftChannels- achieved soft L0 class count (placed only)
 * aPa            - owned frames; freed only on destroy after last map drop
 *                  replica r uses aPa[r * cPagesLogical + i]
 */
struct gj_memobj {
    struct gj_obj_hdr hdr;
    u32               u32Kind;
    u32               cPages;
    u32               cMapped;         /* live region views across processes */
    u32               u32Flags;        /* GJ_MEMOBJ_F_* */
    u32               cReplicas;       /* 0 or 1..MAX placed */
    u32               cPagesLogical;   /* per-replica pages when placed */
    u32               u32SoftChannels; /* soft L0 achieved class count */
    gj_paddr_t        aPa[GJ_MEMOBJ_MAX_PAGES];
};

/**
 * One-time module init (pools, named table, soft stats).
 * Call before create/map; idempotent soft preferred.
 */
void memobj_init(void);

/**
 * Allocate an anon memory object with cPages frames (zeroed ownership).
 * Returns NULL if cPages==0, > MAX, or PMM exhausted.
 * Object is not published; cMapped starts at 0 until map_*.
 */
struct gj_memobj *memobj_create_anon(u32 cPages);

/**
 * Allocate a FILE-kind memory object with cPages zeroed frames (soft).
 * Soft product only: no pager / no live fd binding - filled by map_file_fd.
 * Returns NULL if cPages==0, > MAX, or PMM exhausted.
 * greppable: memobj: file create soft
 */
struct gj_memobj *memobj_create_file(u32 cPages);

/**
 * Destroy object frames + free pool slot.
 * Named objects should be unpublished first (or destroy clears named soft).
 * Caller must ensure no live maps (cMapped == 0) for hard safety; soft may
 * refuse or tear views depending on impl path.
 */
void memobj_destroy(struct gj_memobj *pObj);

/**
 * Map anon object into process AS (creates region + installs PTEs).
 *
 * Always maps with GJ_VMM_PROT_USER (G-MAP-2). Ensures private AS (G-AS-1).
 * u64Hint / fFixed: placement; 0 hint uses per-process anon cursor.
 * Returns VA base or 0 (region full, overlap, NOMEM, bad proc).
 */
gj_vaddr_t memobj_map_anon(struct gj_process *pProc, u64 u64Hint, size_t cbLen,
                           u32 u32Prot, int fFixed);

/**
 * Soft file-backed map from a live vfs_ram regular-file fd.
 *
 * Snapshot: pread file bytes at u64Off into a new FILE memobj, then install
 * USER PTEs (G-MAP-2). Beyond-EOF bytes stay zero. Not a live pager - shared
 * file mutations after map are not reflected (soft product path).
 *
 * Returns VA base or 0 when fd is not a ram regular file, length exceeds
 * GJ_MEMOBJ_MAX_PAGES, offset unaligned, or map install fails.
 * greppable: memobj: file map soft
 */
gj_vaddr_t memobj_map_file_fd(struct gj_process *pProc, i64 i64Fd, u64 u64Hint,
                              size_t cbLen, u32 u32Prot, int fFixed,
                              u64 u64Off);

/**
 * Unmap VA range: drop regions + PTEs; free object if last map + not named.
 * Partial range may split/trim soft; page-granular via VMM.
 * Returns GJ_OK or GJ_ERR_* (INVAL / NOENT).
 */
gj_status_t memobj_unmap(struct gj_process *pProc, gj_vaddr_t va, size_t cbLen);

/**
 * Ensure process has private AS (clone if u64Cr3 == 0 - G-AS-1).
 * No-op if already private. Returns GJ_OK or NOMEM/INVAL.
 */
gj_status_t process_as_ensure(struct gj_process *pProc);

/**
 * Switch CPU to process AS (load CR3), or kernel template if pProc NULL / 0.
 * G-AS-3 schedule path uses this; IRQ/kernel threads may force kernel CR3.
 */
void process_as_activate(struct gj_process *pProc);

/* ---- Named shareable objects (Proton A0 / wine-server) ------------------- */

/**
 * Create anon object and publish under name (max 31 chars + NUL).
 *
 * Marks SHAREABLE|NAMED. Returns object or NULL (EEXIST / pool / pages).
 * Wine path names: "wine-shm0", "winesrv-shm", "mfd:*", "init-shm", ...
 * greppable: memobj: named wine-shm
 */
struct gj_memobj *memobj_create_named(const char *szName, u32 cPages);

/**
 * Look up published object by name (does not create, does not map).
 * Returns NULL if missing or szName NULL. Does not bump cMapped.
 */
struct gj_memobj *memobj_lookup_named(const char *szName);

/**
 * Map a named object into process (same frames; shareable - G-MO-3).
 * Always maps with GJ_VMM_PROT_USER. Returns VA base or 0.
 * greppable: memobj: share
 */
gj_vaddr_t memobj_map_named(struct gj_process *pProc, const char *szName,
                            u64 u64Hint, u32 u32Prot);

/**
 * Unpublish name. Object lives while cMapped > 0; else reclaimed.
 * Soft OK if name missing (NOENT). Keeps wine-shm create/map path stable.
 */
gj_status_t memobj_unlink_named(const char *szName);

/**
 * Map an existing object into process AS (shareable view - G-MO-3).
 * Same USER / region-table soft rules as map_named. Returns VA or 0.
 * Object should be F_SHAREABLE for multi-map; soft may still allow one map.
 */
gj_vaddr_t memobj_map_share(struct gj_process *pProc, struct gj_memobj *pObj,
                            u64 u64Hint, u32 u32Prot, int fFixed);

/* ---- Soft region table / USER flags / observability ---------------------- */

/**
 * Sanitize product map prot: keep R/W/X only, force GJ_VMM_PROT_USER
 * (G-MAP-2). Empty rights soft-default to READ|USER.
 * Call at every product map entry (anon/named/share).
 */
u32 memobj_sanitize_user_prot(u32 u32Prot);

/**
 * Soft: free slots in process fixed region table (0 if pProc NULL).
 * Table size is GJ_PROC_REGION_MAX on gj_process (bring-up fixed).
 * greppable: memobj: region table soft
 */
u32 memobj_region_slots_free(const struct gj_process *pProc);

/**
 * Soft: find region index by exact base VA.
 * Returns index or GJ_PROC_REGION_MAX if missing / bad args.
 */
u32 memobj_region_find(const struct gj_process *pProc, gj_vaddr_t va);

/**
 * Soft: PA of page iPage (G-MO-3 futex shared-key helper).
 * Returns 0 if pObj NULL or iPage >= cPages.
 * Kernel-only: NEVER copyout to userspace (MEM_PLACE_NO_PA_LEAK).
 */
gj_paddr_t memobj_page_pa(const struct gj_memobj *pObj, u32 iPage);

/**
 * Soft stats: named slots used, pool slots used, sum of cMapped.
 * Any out pointer may be NULL (skipped). Diagnostics only; not a hard gate.
 */
void memobj_soft_stats(u32 *pNamedUsed, u32 *pPoolUsed, u32 *pMappedTotal);

/* ---- Placement Option A (docs/MEM_PLACE_CHANNEL.md) --------------------- */

/**
 * Create a placed multi-replica object (soft L0 CHAN_STRIPED residual).
 *
 * One object: frames = cPagesLogical * replicas, layout per replica slice.
 * Publishes under auto name "placed-%u". SHAREABLE|NAMED|CHAN_STRIPED.
 * Soft diversify: prefer distinct soft L0 classes across replicas.
 * If u32MinChannels > 0 and diversify fails: destroy, return NULL,
 * memobj_place_last_status() == GJ_ERR_NOSUPPORT.
 * Soft!=product: not product DRAM channel map (L2 OPEN).
 * greppable: memobj: place create | memobj: soft place
 */
struct gj_memobj *memobj_create_placed(u32 cPagesLogical,
                                       const struct gj_mem_place_req *pReq);

/**
 * Map replica r (0..cReplicas-1): only that replica's logical pages.
 * Always USER (G-MAP-2). Returns VA base or 0.
 * greppable: memobj: place map replica
 */
gj_vaddr_t memobj_map_replica(struct gj_process *pProc, struct gj_memobj *pObj,
                              u32 u32Replica, u64 u64Hint, u32 u32Prot,
                              int fFixed);

/** Placed replica count (0 if not a placed object). */
u32 memobj_placed_replicas(const struct gj_memobj *pObj);

/** Logical pages per replica (0 if not placed). */
u32 memobj_placed_logical_pages(const struct gj_memobj *pObj);

/**
 * Soft status of last memobj_create_placed failure (or GJ_OK after success).
 * Syscall maps: INVAL / NOMEM / NOSUPPORT. Soft!=product.
 */
gj_status_t memobj_place_last_status(void);

/**
 * Soft: last successful create_placed published name ("" if none).
 * For syscall fill of place_out.szName. Never includes PA.
 */
const char *memobj_place_last_name(void);
