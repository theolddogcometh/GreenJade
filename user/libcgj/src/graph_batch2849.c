/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2849: ring occupancy empty predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ring_u8_empty_p_u(size_t count);
 *     - Return 1 when a count-based ring is empty (count == 0), else 0.
 *   int __gj_ring_u8_empty_p_u  (alias)
 *   __libcgj_batch2849_marker = "libcgj-batch2849"
 *
 * Endian/ring exclusive wave (2841-2850). Predicate companion for
 * batch281 gj_ring_u8_pop (empty when count == 0) and opposite of
 * gj_ring_u8_full_p_u (batch2848). Unique _u name; no multi-def.
 * Pure integer; no buffer access.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2849_marker[] = "libcgj-batch2849";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff occupancy watermark is zero. */
static int
b2849_empty(size_t cN)
{
	if (cN == 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_empty_p_u - true when a count-based u8 ring is empty.
 *
 * count: live occupancy (0..cap under correct use)
 *
 * Returns 1 when count == 0, else 0.
 */
int
gj_ring_u8_empty_p_u(size_t cN)
{
	(void)NULL;
	return b2849_empty(cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u8_empty_p_u(size_t cN)
    __attribute__((alias("gj_ring_u8_empty_p_u")));
