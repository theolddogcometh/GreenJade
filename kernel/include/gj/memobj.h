/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Memory objects own frames; vmm only installs PTEs (G-MO-*).
 * Product maps are USER and confined to [GJ_USER_VA_BASE, GJ_USER_VA_END).
 */
#pragma once

#include <gj/cap.h>
#include <gj/error.h>
#include <gj/types.h>

struct gj_process;

enum gj_memobj_kind {
    GJ_MEMOBJ_ANON = 1,
    GJ_MEMOBJ_FILE = 2, /* cold/pager — not fully wired in v1 */
};

#define GJ_MEMOBJ_MAX_PAGES  256u

struct gj_memobj {
    struct gj_obj_hdr   hdr;
    u32                 u32Kind;
    u32                 cPages;
    u32                 cMapped;
    u32                 u32Pad;
    gj_paddr_t          aPa[GJ_MEMOBJ_MAX_PAGES];
};

void memobj_init(void);

/** Allocate an anon memory object with cPages frames (zeroed ownership). */
struct gj_memobj *memobj_create_anon(u32 cPages);

void memobj_destroy(struct gj_memobj *pObj);

/**
 * Map anon object into process AS (creates region + installs PTEs).
 * Always maps with GJ_VMM_PROT_USER. Returns VA base or 0.
 */
gj_vaddr_t memobj_map_anon(struct gj_process *pProc, u64 u64Hint, size_t cbLen,
                           u32 u32Prot, int fFixed);

/** Unmap VA range: drop regions + PTEs; free object if last map. */
gj_status_t memobj_unmap(struct gj_process *pProc, gj_vaddr_t va, size_t cbLen);

/** Ensure process has private AS (clone if needed). */
gj_status_t process_as_ensure(struct gj_process *pProc);

/** Switch CPU to process AS (or kernel if NULL/0). */
void process_as_activate(struct gj_process *pProc);

/**
 * Named shareable memory objects (Proton A0 / wine-server).
 * Name is a short null-terminated key (max 31 chars).
 */
#define GJ_MEMOBJ_NAME_MAX 32

/** Create anon object and publish under name. Returns object or NULL. */
struct gj_memobj *memobj_create_named(const char *szName, u32 cPages);

/** Look up published object by name (does not create). */
struct gj_memobj *memobj_lookup_named(const char *szName);

/**
 * Map a named object into process (same frames; shareable).
 * Always maps with GJ_VMM_PROT_USER. Returns VA base or 0.
 */
gj_vaddr_t memobj_map_named(struct gj_process *pProc, const char *szName,
                            u64 u64Hint, u32 u32Prot);
