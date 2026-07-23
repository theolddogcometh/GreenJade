/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch798: all-zero MAC address predicate.
 *
 * Surface (unique symbols):
 *   int gj_mac_is_zero(const unsigned char mac[6]);
 *     — Return 1 if all six octets are zero, else 0.
 *       mac == NULL → 0.
 *   int __gj_mac_is_zero  (alias)
 *   __libcgj_batch798_marker = "libcgj-batch798"
 *
 * Distinct from gj_mem_is_zero (batch607) and gj_mac_parse (batch157) —
 * unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch798_marker[] = "libcgj-batch798";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mac_is_zero — 1 if mac is 00:00:00:00:00:00.
 */
int
gj_mac_is_zero(const unsigned char aMac[6])
{
	unsigned i;

	if (aMac == NULL) {
		return 0;
	}

	for (i = 0u; i < 6u; i++) {
		if (aMac[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mac_is_zero(const unsigned char aMac[6])
    __attribute__((alias("gj_mac_is_zero")));
