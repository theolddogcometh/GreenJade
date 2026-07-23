/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4386: fixed 32-octet all-zero test (_u).
 *
 * Surface (unique symbols):
 *   int gj_bytes_is_zero32_u(const uint8_t *a);
 *     - Return 1 if a is non-NULL and all 32 octets are zero; else 0.
 *   int __gj_bytes_is_zero32_u  (alias)
 *   __libcgj_batch4386_marker = "libcgj-batch4386"
 *
 * Exclusive continuum CREATE-ONLY (4381-4390). Distinct from
 * gj_bytes_is_zero16_u (batch4385) and gj_bytes_is_zero_consttime
 * (batch1249). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4386_marker[] = "libcgj-batch4386";

/* ---- freestanding helpers ---------------------------------------------- */

/* All-zero over 32 octets. pA must be non-NULL. */
static int
b4386_is_zero32(const uint8_t *pA)
{
	unsigned i;

	for (i = 0u; i < 32u; i++) {
		if (pA[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_is_zero32_u - 1 iff 32-octet buffer is all zeros.
 *
 * a: 32-octet region (NULL -> 0)
 *
 * Returns 1 if all zero, 0 otherwise. Does not call libc.
 * Self-contained; no parent wires.
 */
int
gj_bytes_is_zero32_u(const uint8_t *pA)
{
	(void)NULL;
	if (pA == NULL) {
		return 0;
	}
	return b4386_is_zero32(pA);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bytes_is_zero32_u(const uint8_t *pA)
    __attribute__((alias("gj_bytes_is_zero32_u")));
