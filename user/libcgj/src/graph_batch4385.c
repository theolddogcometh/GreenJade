/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4385: fixed 16-octet all-zero test (_u).
 *
 * Surface (unique symbols):
 *   int gj_bytes_is_zero16_u(const uint8_t *a);
 *     - Return 1 if a is non-NULL and all 16 octets are zero; else 0.
 *   int __gj_bytes_is_zero16_u  (alias)
 *   __libcgj_batch4385_marker = "libcgj-batch4385"
 *
 * Exclusive continuum CREATE-ONLY (4381-4390). Distinct from
 * gj_bytes_is_zero_consttime (batch1249) — fixed-width non-CT surface.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4385_marker[] = "libcgj-batch4385";

/* ---- freestanding helpers ---------------------------------------------- */

/* All-zero over 16 octets. pA must be non-NULL. */
static int
b4385_is_zero16(const uint8_t *pA)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		if (pA[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_is_zero16_u - 1 iff 16-octet buffer is all zeros.
 *
 * a: 16-octet region (NULL -> 0)
 *
 * Returns 1 if all zero, 0 otherwise. Does not call libc.
 * Self-contained; no parent wires.
 */
int
gj_bytes_is_zero16_u(const uint8_t *pA)
{
	(void)NULL;
	if (pA == NULL) {
		return 0;
	}
	return b4385_is_zero16(pA);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bytes_is_zero16_u(const uint8_t *pA)
    __attribute__((alias("gj_bytes_is_zero16_u")));
