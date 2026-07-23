/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8375: mmap MAP_PRIVATE flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_map_private_id_8375(void);
 *     - Returns soft MAP_PRIVATE (0x2). Catalog constant only.
 *   uint32_t __gj_map_private_id_8375  (alias)
 *   __libcgj_batch8375_marker = "libcgj-batch8375"
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

const char __libcgj_batch8375_marker[] = "libcgj-batch8375";

/* Soft MAP_PRIVATE (Linux/glibc-shaped). */
#define B8375_MAP_PRIVATE  ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8375_map_private_id(void)
{
	return B8375_MAP_PRIVATE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_map_private_id_8375 - soft MAP_PRIVATE flag value.
 *
 * Always returns 2. Pure catalog stub for mmap map flags.
 * No parent wires.
 */
uint32_t
gj_map_private_id_8375(void)
{
	(void)NULL;
	return b8375_map_private_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_map_private_id_8375(void)
    __attribute__((alias("gj_map_private_id_8375")));
