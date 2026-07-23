/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6617: mmap MAP_SHARED flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mmap_map_shared_u_6617(uint32_t flags);
 *     - Return 1 when MAP_SHARED (0x1) is set in flags; else 0.
 *   uint32_t __gj_mmap_map_shared_u_6617  (alias)
 *   __libcgj_batch6617_marker = "libcgj-batch6617"
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

const char __libcgj_batch6617_marker[] = "libcgj-batch6617";

/* Soft MAP_SHARED (Linux/glibc-shaped). */
#define B6617_MAP_SHARED  ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6617_map_shared(uint32_t u32Flags)
{
	if ((u32Flags & B6617_MAP_SHARED) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mmap_map_shared_u_6617 - soft MAP_SHARED presence test.
 *
 * flags: raw mmap flags word
 *
 * Returns 1 when bit 0 (MAP_SHARED) is set, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_mmap_map_shared_u_6617(uint32_t flags)
{
	(void)NULL;
	return b6617_map_shared(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mmap_map_shared_u_6617(uint32_t flags)
    __attribute__((alias("gj_mmap_map_shared_u_6617")));
