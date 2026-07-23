/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5065: all-zero Ethernet MAC predicate.
 *
 * Surface (unique symbols):
 *   int gj_net_eth_is_zero_u(const uint8_t *m);
 *     - Return 1 if all six octets are zero, else 0.
 *       m == NULL → 0.
 *   int __gj_net_eth_is_zero_u  (alias)
 *   __libcgj_batch5065_marker = "libcgj-batch5065"
 *
 * Exclusive continuum CREATE-ONLY (5061-5070). Distinct from
 * gj_mac_is_zero (batch798) / gj_mem_is_zero (batch607) — unique
 * gj_net_eth_is_zero_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5065_marker[] = "libcgj-batch5065";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5065_is_zero(const uint8_t *pM)
{
	unsigned i;

	if (pM == NULL) {
		return 0;
	}
	for (i = 0u; i < 6u; i++) {
		if (pM[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_eth_is_zero_u - 1 if m is 00:00:00:00:00:00.
 *
 * m: pointer to 6 MAC octets; NULL → 0
 *
 * Self-contained; no parent wires.
 */
int
gj_net_eth_is_zero_u(const uint8_t *m)
{
	(void)NULL;
	return b5065_is_zero(m);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_net_eth_is_zero_u(const uint8_t *m)
    __attribute__((alias("gj_net_eth_is_zero_u")));
