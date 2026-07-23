/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8377: mmap MAP_ANON/MAP_ANONYMOUS flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_map_anon_id_8377(void);
 *     - Returns soft MAP_ANONYMOUS / MAP_ANON (0x20). Catalog constant only.
 *   uint32_t __gj_map_anon_id_8377  (alias)
 *   __libcgj_batch8377_marker = "libcgj-batch8377"
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

const char __libcgj_batch8377_marker[] = "libcgj-batch8377";

/* Soft MAP_ANONYMOUS / MAP_ANON (Linux/glibc-shaped). */
#define B8377_MAP_ANON  ((uint32_t)0x20u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8377_map_anon_id(void)
{
	return B8377_MAP_ANON;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_map_anon_id_8377 - soft MAP_ANON flag value.
 *
 * Always returns 0x20. Pure catalog stub for mmap map flags.
 * No parent wires.
 */
uint32_t
gj_map_anon_id_8377(void)
{
	(void)NULL;
	return b8377_map_anon_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_map_anon_id_8377(void)
    __attribute__((alias("gj_map_anon_id_8377")));
