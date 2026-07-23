/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8378: mmap MAP_FIXED flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_map_fixed_id_8378(void);
 *     - Returns soft MAP_FIXED (0x10). Catalog constant only.
 *   uint32_t __gj_map_fixed_id_8378  (alias)
 *   __libcgj_batch8378_marker = "libcgj-batch8378"
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

const char __libcgj_batch8378_marker[] = "libcgj-batch8378";

/* Soft MAP_FIXED (Linux/glibc-shaped). */
#define B8378_MAP_FIXED  ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8378_map_fixed_id(void)
{
	return B8378_MAP_FIXED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_map_fixed_id_8378 - soft MAP_FIXED flag value.
 *
 * Always returns 0x10. Pure catalog stub for mmap map flags.
 * No parent wires.
 */
uint32_t
gj_map_fixed_id_8378(void)
{
	(void)NULL;
	return b8378_map_fixed_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_map_fixed_id_8378(void)
    __attribute__((alias("gj_map_fixed_id_8378")));
