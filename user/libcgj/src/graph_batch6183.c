/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6183: cgroup memory.max byte clamp stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_cg_mem_max_clamp_6183(uint64_t bytes);
 *     - Clamp a memory.max-style budget into [4096, UINT64_MAX - 4095]
 *       page-aligned down to a multiple of 4096. bytes < 4096 → 4096.
 *       Unlimited/"max" encodings are out of scope for this clamp.
 *   uint64_t __gj_cg_mem_max_clamp_6183  (alias)
 *   __libcgj_batch6183_marker = "libcgj-batch6183"
 *
 * Exclusive continuum CREATE-ONLY (6181-6190: container runtime cgroup
 * stubs). Unique gj_cg_mem_max_clamp_6183 surface only; no multi-def.
 * Pure data — not a memory.max writer. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6183_marker[] = "libcgj-batch6183";

#define B6183_PAGE       4096ull
#define B6183_PAGE_MASK  (~(B6183_PAGE - 1ull))

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6183_mem_clamp(uint64_t uBytes)
{
	if (uBytes < B6183_PAGE) {
		return B6183_PAGE;
	}
	/* Page-align down; top of range already fits in uint64_t. */
	return uBytes & B6183_PAGE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cg_mem_max_clamp_6183 - clamp memory.max into page-aligned range.
 *
 * Suitable for sanitizing parsed memory.max fields before container
 * runtime policy math. No mm_struct, no cgroupfs. Does not call libc.
 * No parent wires.
 */
uint64_t
gj_cg_mem_max_clamp_6183(uint64_t uBytes)
{
	(void)NULL;
	return b6183_mem_clamp(uBytes);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_cg_mem_max_clamp_6183(uint64_t uBytes)
    __attribute__((alias("gj_cg_mem_max_clamp_6183")));
