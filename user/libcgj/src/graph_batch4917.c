/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4917: freestanding UUID 16-octet swap.
 *
 * Surface (unique symbols):
 *   int gj_uuid_swap_u(uint8_t a[16], uint8_t b[16]);
 *     - Exchange the 16 octets at a and b. Either NULL -> no-op
 *       return 0. Same pointer -> return 1. Success -> 1.
 *   int __gj_uuid_swap_u  (alias)
 *   __libcgj_batch4917_marker = "libcgj-batch4917"
 *
 * Exclusive continuum CREATE-ONLY UUID v4 bits wave (4911-4920).
 * Distinct from gj_mem_swap (batch663) and gj_mem_swap_bytes
 * (batch2029) — unique gj_uuid_swap_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4917_marker[] = "libcgj-batch4917";

/* ---- freestanding helpers ---------------------------------------------- */

/* Swap 16 octets. pA and pB must be non-NULL and distinct. */
static void
b4917_swap(uint8_t *pA, uint8_t *pB)
{
	unsigned i;
	uint8_t u8T;

	for (i = 0u; i < 16u; i++) {
		u8T = pA[i];
		pA[i] = pB[i];
		pB[i] = u8T;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_swap_u - exchange two 16-octet UUID values in place.
 *
 * a, b: 16-octet UUID wire values (either NULL -> no-op, return 0)
 *
 * Does not call libc. Same-pointer is a no-op success (return 1).
 * No parent wires.
 */
int
gj_uuid_swap_u(uint8_t a[16], uint8_t b[16])
{
	(void)NULL;
	if (a == NULL || b == NULL) {
		return 0;
	}
	if (a == b) {
		return 1;
	}
	b4917_swap(a, b);
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_swap_u(uint8_t a[16], uint8_t b[16])
    __attribute__((alias("gj_uuid_swap_u")));
