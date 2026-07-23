/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6786: inotify event-mask popcount.
 *
 * Surface (unique symbols):
 *   uint32_t gj_inotify_mask_pop_6786(uint32_t mask);
 *     - Count set bits in mask (0..32).
 *   uint32_t __gj_inotify_mask_pop_6786  (alias)
 *   __libcgj_batch6786_marker = "libcgj-batch6786"
 *
 * Exclusive continuum CREATE-ONLY (6781-6790: inotify event mask stubs).
 * Unique gj_inotify_mask_pop_6786 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6786_marker[] = "libcgj-batch6786";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6786_pop(uint32_t u32Mask)
{
	uint32_t u32N = 0u;
	uint32_t u32Bits = u32Mask;

	while (u32Bits != 0u) {
		u32N += (u32Bits & 1u);
		u32Bits >>= 1;
	}
	return u32N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inotify_mask_pop_6786 - count set bits in an inotify event mask.
 *
 * mask: soft inotify event bitmask
 *
 * Returns 0..32. Soft pure-data popcount; no inotify fd.
 */
uint32_t
gj_inotify_mask_pop_6786(uint32_t u32Mask)
{
	(void)NULL;
	return b6786_pop(u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_inotify_mask_pop_6786(uint32_t u32Mask)
    __attribute__((alias("gj_inotify_mask_pop_6786")));
