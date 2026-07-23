/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Memory objects own frames; vmm only installs PTEs (G-MO-*).
 * Product maps are USER and confined to [GJ_USER_VA_BASE, GJ_USER_VA_END)
 * (G-MAP-2). Regions are views; objects may be multi-mapped (G-MO-3).
 *
 * Soft product surface (this module only):
 *   G-MO-1   anon mmap → object + PTEs
 *   G-MO-3   named shareable maps (wine-shm / winesrv-shm / mfd:*)
 *   G-MAP-2  USER map flags always forced on product maps
 *   region   fixed GJ_PROC_REGION_MAX table with soft full / reuse / overlap
 *
 * Grep markers (kernel/mm/memobj.c):
 *   memobj: named
 *   memobj: share
 *   memobj: region table soft
 *   memobj: USER map
 *   wine-shm
 */
#pragma once

#include <gj/cap.h>
#include <gj/error.h>
#include <gj/types.h>

struct gj_process;

enum gj_memobj_kind {
    GJ_MEMOBJ_ANON  = 1,
    GJ_MEMOBJ_FILE  = 2, /* cold/pager — not fully wired in v1 */
    GJ_MEMOBJ_NAMED = 3, /* published in named registry (shareable) */
};

/** Soft object flags (u32Flags). */
#define GJ_MEMOBJ_F_SHAREABLE (1u << 0) /* multi-map ok (G-MO-3) */
#define GJ_MEMOBJ_F_NAMED     (1u << 1) /* still in named publish table */
#define GJ_MEMOBJ_F_ZEROED    (1u << 2) /* frames zeroed at create */

#define GJ_MEMOBJ_MAX_PAGES 256u
#define GJ_MEMOBJ_NAME_MAX  32

struct gj_memobj {
    struct gj_obj_hdr hdr;
    u32               u32Kind;
    u32               cPages;
    u32               cMapped;  /* live region views across processes */
    u32               u32Flags; /* GJ_MEMOBJ_F_* */
    gj_paddr_t        aPa[GJ_MEMOBJ_MAX_PAGES];
};

void memobj_init(void);

/** Allocate an anon memory object with cPages frames (zeroed ownership). */
struct gj_memobj *memobj_create_anon(u32 cPages);

/**
 * Destroy object frames + free pool slot.
 * Named objects should be unpublished first (or destroy clears named soft).
 */
void memobj_destroy(struct gj_memobj *pObj);

/**
 * Map anon object into process AS (creates region + installs PTEs).
 * Always maps with GJ_VMM_PROT_USER (G-MAP-2). Returns VA base or 0.
 */
gj_vaddr_t memobj_map_anon(struct gj_process *pProc, u64 u64Hint, size_t cbLen,
                           u32 u32Prot, int fFixed);

/** Unmap VA range: drop regions + PTEs; free object if last map + not named. */
gj_status_t memobj_unmap(struct gj_process *pProc, gj_vaddr_t va, size_t cbLen);

/** Ensure process has private AS (clone if needed). */
gj_status_t process_as_ensure(struct gj_process *pProc);

/** Switch CPU to process AS (or kernel if NULL/0). */
void process_as_activate(struct gj_process *pProc);

/* ---- Named shareable objects (Proton A0 / wine-server) ------------------- */

/**
 * Create anon object and publish under name (max 31 chars + NUL).
 * Marks SHAREABLE|NAMED. Returns object or NULL (EEXIST / pool / pages).
 * Wine path names: "wine-shm0", "winesrv-shm", "mfd:*", "init-shm", …
 */
struct gj_memobj *memobj_create_named(const char *szName, u32 cPages);

/** Look up published object by name (does not create, does not map). */
struct gj_memobj *memobj_lookup_named(const char *szName);

/**
 * Map a named object into process (same frames; shareable — G-MO-3).
 * Always maps with GJ_VMM_PROT_USER. Returns VA base or 0.
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
 */
gj_vaddr_t memobj_map_share(struct gj_process *pProc, struct gj_memobj *pObj,
                            u64 u64Hint, u32 u32Prot, int fFixed);

/* ---- Soft region table / USER flags / observability ---------------------- */

/**
 * Sanitize product map prot: keep R/W/X only, force GJ_VMM_PROT_USER
 * (G-MAP-2). Empty rights soft-default to READ|USER.
 */
u32 memobj_sanitize_user_prot(u32 u32Prot);

/** Soft: free slots in process fixed region table (0 if pProc NULL). */
u32 memobj_region_slots_free(const struct gj_process *pProc);

/**
 * Soft: find region index by exact base VA.
 * Returns index or GJ_PROC_REGION_MAX if missing / bad args.
 */
u32 memobj_region_find(const struct gj_process *pProc, gj_vaddr_t va);

/** Soft: PA of page iPage (G-MO-3 futex shared-key helper). 0 if OOB. */
gj_paddr_t memobj_page_pa(const struct gj_memobj *pObj, u32 iPage);

/**
 * Soft stats: named slots used, pool slots used, sum of cMapped.
 * Any out pointer may be NULL.
 */
void memobj_soft_stats(u32 *pNamedUsed, u32 *pPoolUsed, u32 *pMappedTotal);
