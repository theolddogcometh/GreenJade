/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Portable cooperative scheduler core (shared C kernel).
 * aarch64 product uses this; x86 keeps full thread.c / SYSCALL path.
 */
#pragma once

#include <gj/types.h>

#define GJ_COOP_MAX_THR 8u
#define GJ_COOP_STACK   (8u * 1024u)

enum gj_coop_state {
    GJ_COOP_UNUSED = 0,
    GJ_COOP_RUNNABLE = 1,
    GJ_COOP_RUNNING = 2,
    GJ_COOP_EXITED = 3
};

typedef void (*gj_coop_entry_fn)(void *pArg);

void gj_coop_init(void);
/** Create thr; returns id ≥ 1 or 0 on fail. */
u32 gj_coop_create(gj_coop_entry_fn pfn, void *pArg);
void gj_coop_yield(void);
void gj_coop_exit(void);
u32 gj_coop_current_id(void);
/** Soft selftest: create thr, yield, exit. Returns 1 on PASS. */
int gj_coop_selftest(void);

/* Arch must provide: switch stacks (save *pOldSp, load u64NewSp). */
void gj_coop_switch(u64 *pOldSp, u64 u64NewSp);
