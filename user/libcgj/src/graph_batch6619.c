/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6619: mmap map-type soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mmap_map_type_ok_6619(uint32_t flags);
 *     - Return 1 when exactly one of MAP_SHARED or MAP_PRIVATE is set;
 *       0 if neither or both (soft mutual-exclusion gate).
 *   uint32_t __gj_mmap_map_type_ok_6619  (alias)
 *   __libcgj_batch6619_marker = "libcgj-batch6619"
 *
 * Exclusive continuum CREATE-ONLY (6611-6620: mmap/mprotect flag stubs —
 * prot_read, prot_write, prot_exec, prot_none_ok, prot_pack, prot_mask_ok,
 * map_shared, map_private, map_type_ok, flags_ready). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6619_marker[] = "libcgj-batch6619";

#define B6619_MAP_SHARED   ((uint32_t)0x1u)
#define B6619_MAP_PRIVATE  ((uint32_t)0x2u)
#define B6619_MAP_TYPE     ((uint32_t)0x3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6619_map_type_ok(uint32_t u32Flags)
{
	uint32_t u32Type = u32Flags & B6619_MAP_TYPE;

	/* Exactly one of SHARED or PRIVATE. */
	if (u32Type == B6619_MAP_SHARED || u32Type == B6619_MAP_PRIVATE) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mmap_map_type_ok_6619 - soft MAP_SHARED/PRIVATE mutual exclusion.
 *
 * flags: raw mmap flags word
 *
 * Returns 1 when exactly one of MAP_SHARED or MAP_PRIVATE is set, else 0.
 * Self-contained; does not call map_shared/map_private stubs.
 * No parent wires.
 */
uint32_t
gj_mmap_map_type_ok_6619(uint32_t flags)
{
	(void)NULL;
	return b6619_map_type_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mmap_map_type_ok_6619(uint32_t flags)
    __attribute__((alias("gj_mmap_map_type_ok_6619")));
