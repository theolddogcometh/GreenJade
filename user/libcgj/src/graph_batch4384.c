/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4384: fixed 32-octet equality (_u).
 *
 * Surface (unique symbols):
 *   int gj_bytes_eq32_u(const uint8_t *a, const uint8_t *b);
 *     - Return 1 if both non-NULL and all 32 octets equal; else 0.
 *   int __gj_bytes_eq32_u  (alias)
 *   __libcgj_batch4384_marker = "libcgj-batch4384"
 *
 * Exclusive continuum CREATE-ONLY (4381-4390). Distinct from
 * gj_bytes_eq16_u (batch4383) and gj_bytes_eq_consttime (batch1248).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4384_marker[] = "libcgj-batch4384";

/* ---- freestanding helpers ---------------------------------------------- */

/* Compare 32 octets. pA and pB must be non-NULL. */
static int
b4384_eq32(const uint8_t *pA, const uint8_t *pB)
{
	unsigned i;

	for (i = 0u; i < 32u; i++) {
		if (pA[i] != pB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_eq32_u - equality of two 32-octet buffers.
 *
 * a, b: 32-octet regions (NULL either -> 0)
 *
 * Returns 1 if equal, 0 otherwise. Does not call libc.
 * Self-contained; no parent wires.
 */
int
gj_bytes_eq32_u(const uint8_t *pA, const uint8_t *pB)
{
	(void)NULL;
	if (pA == NULL || pB == NULL) {
		return 0;
	}
	return b4384_eq32(pA, pB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bytes_eq32_u(const uint8_t *pA, const uint8_t *pB)
    __attribute__((alias("gj_bytes_eq32_u")));
