/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Memory objects own frames; VMM only installs PTEs (G-MO-*).
 *
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0.
 * Implementation: kernel/mm/memobj.c.
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
 *
 * Soft product surface (this module only)
 * ---------------------------------------
 *   G-MO-1   anon mmap → object + PTEs
 *   G-MO-3   named shareable maps (wine-shm / winesrv-shm / mfd:*)
 *   G-MAP-2  USER map flags always forced on product maps
 *   region   fixed GJ_PROC_REGION_MAX table with soft full / reuse / overlap
 *   G-AS     process_as_ensure / process_as_activate (private CR3)
 *
 * Object kinds
 * ------------
 *   ANON   — private-ish frames; shareable only if F_SHAREABLE
 *   FILE   — cold/pager (not fully wired in v1)
 *   NAMED  — published in soft name table; multi-process map (G-MO-3)
 *
 * Layering
 * --------
 *   pmm_alloc*  → frames stored in gj_memobj.aPa[]
 *   vmm_map_page under process CR3 after process_as_ensure
 *   region row on gj_process.aRegions[] (view: va, len, prot, pObj)
 *   fault / pager may bind object pointer on map cookie (soft)
 *   futex shared key: memobj_page_pa (PA of word — Linux-true G-MO-3)
 *
 * Grep markers (kernel/mm/memobj.c)
 * ---------------------------------
 *   memobj: named
 *   memobj: share
 *   memobj: region table soft
 *   memobj: USER map
 *   wine-shm
 *
 * greppable: G-MO-1 G-MO-3 G-MAP-2 MEMOBJ_NAMED MEMOBJ_SHARE MEMOBJ_REGION
 */
#pragma once

#include <gj/cap.h>
#include <gj/error.h>
#include <gj/types.h>

struct gj_process;

/**
 * Memory object kind (u32Kind). FILE is reserved for pager-backed cold path.
 */
enum gj_memobj_kind {
    GJ_MEMOBJ_ANON  = 1,
    GJ_MEMOBJ_FILE  = 2, /* cold/pager — not fully wired in v1 */
    GJ_MEMOBJ_NAMED = 3, /* published in named registry (shareable) */
};

/** Soft object flags (u32Flags). */
#define GJ_MEMOBJ_F_SHAREABLE (1u << 0) /* multi-map ok (G-MO-3) */
#define GJ_MEMOBJ_F_NAMED     (1u << 1) /* still in named publish table */
#define GJ_MEMOBJ_F_ZEROED    (1u << 2) /* frames zeroed at create */

/**
 * Bring-up frame cap per object (not a product TiB limit).
 * Larger maps need multi-object or future large-object path.
 */
#define GJ_MEMOBJ_MAX_PAGES 256u

/** Named registry key capacity including trailing NUL (31 chars usable). */
#define GJ_MEMOBJ_NAME_MAX  32

/**
 * Kernel memory object (cap type GJ_CAP_MEMORY_OBJECT when minted).
 *
 * hdr       — lifecycle (LIVE/DEAD/gen/ref/pin/CDT) via gj_obj_hdr
 * cPages    — valid entries in aPa[0 .. cPages)
 * cMapped   — live region views across processes (destroy when 0 + !named)
 * aPa       — owned frames; freed only on destroy after last map drop
 */
struct gj_memobj {
    struct gj_obj_hdr hdr;
    u32               u32Kind;
    u32               cPages;
    u32               cMapped;  /* live region views across processes */
    u32               u32Flags; /* GJ_MEMOBJ_F_* */
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
 * Unmap VA range: drop regions + PTEs; free object if last map + not named.
 * Partial range may split/trim soft; page-granular via VMM.
 * Returns GJ_OK or GJ_ERR_* (INVAL / NOENT).
 */
gj_status_t memobj_unmap(struct gj_process *pProc, gj_vaddr_t va, size_t cbLen);

/**
 * Ensure process has private AS (clone if u64Cr3 == 0 — G-AS-1).
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
 * Wine path names: "wine-shm0", "winesrv-shm", "mfd:*", "init-shm", …
 * greppable: memobj: named wine-shm
 */
struct gj_memobj *memobj_create_named(const char *szName, u32 cPages);

/**
 * Look up published object by name (does not create, does not map).
 * Returns NULL if missing or szName NULL. Does not bump cMapped.
 */
struct gj_memobj *memobj_lookup_named(const char *szName);

/**
 * Map a named object into process (same frames; shareable — G-MO-3).
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
 * Map an existing object into process AS (shareable view — G-MO-3).
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
 * Returns 0 if pObj NULL or iPage ≥ cPages.
 */
gj_paddr_t memobj_page_pa(const struct gj_memobj *pObj, u32 iPage);

/**
 * Soft stats: named slots used, pool slots used, sum of cMapped.
 * Any out pointer may be NULL (skipped). Diagnostics only; not a hard gate.
 */
void memobj_soft_stats(u32 *pNamedUsed, u32 *pPoolUsed, u32 *pMappedTotal);
