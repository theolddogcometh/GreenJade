/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5066: Ethernet MAC equality.
 *
 * Surface (unique symbols):
 *   int gj_net_eth_eq_u(const uint8_t *a, const uint8_t *b);
 *     - Return 1 if the six MAC octets of a equal those of b, else 0.
 *       a == NULL or b == NULL → 0 (unless both are the same non-NULL
 *       pointer path handled by octet compare; pure NULL either side
 *       yields 0).
 *   int __gj_net_eth_eq_u  (alias)
 *   __libcgj_batch5066_marker = "libcgj-batch5066"
 *
 * Exclusive continuum CREATE-ONLY (5061-5070). Distinct from
 * gj_mem_eq (batch365) / gj_mem_eq_n (batch634) — unique
 * gj_net_eth_eq_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5066_marker[] = "libcgj-batch5066";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5066_eth_eq(const uint8_t *pA, const uint8_t *pB)
{
	unsigned i;

	if (pA == NULL || pB == NULL) {
		return 0;
	}
	for (i = 0u; i < 6u; i++) {
		if (pA[i] != pB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_eth_eq_u - 1 if a and b are equal 6-octet MAC addresses.
 *
 * a, b: pointers to 6 MAC octets; either NULL → 0
 *
 * Self-contained; no parent wires.
 */
int
gj_net_eth_eq_u(const uint8_t *a, const uint8_t *b)
{
	(void)NULL;
	return b5066_eth_eq(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_net_eth_eq_u(const uint8_t *a, const uint8_t *b)
    __attribute__((alias("gj_net_eth_eq_u")));
