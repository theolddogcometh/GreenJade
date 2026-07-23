/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1819: clear (zero) a 16-octet IPv6 address.
 *
 * Surface (unique symbols for this TU):
 *   void gj_ipv6_addr_clear(uint8_t addr[16]);
 *     — Write all 16 octets of addr to 0 (::). addr == NULL → no-op.
 *   void __gj_ipv6_addr_clear  (alias)
 *   __libcgj_batch1819_marker = "libcgj-batch1819"
 *
 * IPv6 helpers exclusive continuum 1811..1820. Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1819_marker[] = "libcgj-batch1819";

/* ---- freestanding helpers ---------------------------------------------- */

/* Zero all 16 octets of aAddr. */
static void
b1819_zero16(uint8_t aAddr[16])
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		aAddr[i] = 0u;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_addr_clear — set addr to the unspecified address (::).
 *
 * Edge cases:
 *   addr == NULL → no-op
 *   After clear, gj_ipv6_is_unspecified(addr) == 1
 */
void
gj_ipv6_addr_clear(uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return;
	}
	b1819_zero16(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ipv6_addr_clear(uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_addr_clear")));
