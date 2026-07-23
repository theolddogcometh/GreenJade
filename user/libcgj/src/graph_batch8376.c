/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8376: mmap MAP_SHARED flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_map_shared_id_8376(void);
 *     - Returns soft MAP_SHARED (0x1). Catalog constant only.
 *   uint32_t __gj_map_shared_id_8376  (alias)
 *   __libcgj_batch8376_marker = "libcgj-batch8376"
 *
 * Exclusive continuum CREATE-ONLY (8371-8380: mmap/prot flag stubs
 * integer). Unique batch-suffixed surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8376_marker[] = "libcgj-batch8376";

/* Soft MAP_SHARED (Linux/glibc-shaped). */
#define B8376_MAP_SHARED  ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8376_map_shared_id(void)
{
	return B8376_MAP_SHARED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_map_shared_id_8376 - soft MAP_SHARED flag value.
 *
 * Always returns 1. Pure catalog stub for mmap map flags.
 * No parent wires.
 */
uint32_t
gj_map_shared_id_8376(void)
{
	(void)NULL;
	return b8376_map_shared_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_map_shared_id_8376(void)
    __attribute__((alias("gj_map_shared_id_8376")));
