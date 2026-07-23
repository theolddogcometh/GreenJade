/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1814: IPv6 site-local predicate (fec0::/10).
 *
 * Surface (unique symbols for this TU):
 *   int gj_ipv6_is_sitelocal(const uint8_t addr[16]);
 *     — Return 1 if addr is in fec0::/10 (deprecated site-local,
 *       RFC 3513 / RFC 3879), else 0. addr == NULL → 0.
 *   int __gj_ipv6_is_sitelocal  (alias)
 *   __libcgj_batch1814_marker = "libcgj-batch1814"
 *
 * IPv6 helpers exclusive continuum 1811..1820. Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1814_marker[] = "libcgj-batch1814";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if the first 10 bits of aAddr form 1111111011 (fec0::/10).
 * Network-order: addr[0] == 0xfe and (addr[1] & 0xc0) == 0xc0.
 */
static int
b1814_fec0_prefix10(const uint8_t aAddr[16])
{
	if (aAddr[0] != 0xfeu) {
		return 0;
	}
	if ((aAddr[1] & 0xc0u) != 0xc0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_is_sitelocal — 1 if first 10 bits are 1111111011 (fec0::/10).
 *
 * Site-local unicast was deprecated by RFC 3879 but remains useful for
 * classification of legacy packets/configs.
 * Edge cases:
 *   addr == NULL → 0
 *   fe80::/10 (link-local) → 0
 *   fc00::/7 (ULA) → 0
 */
int
gj_ipv6_is_sitelocal(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	return b1814_fec0_prefix10(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_sitelocal(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_sitelocal")));
