/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch853: host-order IPv4 → four octets.
 *
 * Surface (unique symbols):
 *   void gj_ipv4_to_octets(uint32_t addr, unsigned *a, unsigned *b,
 *                          unsigned *c, unsigned *d);
 *     — Write host-order addr into *a..*d (NULL pointer skipped).
 *   void __gj_ipv4_to_octets  (alias)
 *   __libcgj_batch853_marker = "libcgj-batch853"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch853_marker[] = "libcgj-batch853";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_to_octets — unpack host-order IPv4 into four octet values.
 *
 * Inverse of gj_ipv4_from_octets. NULL out-pointers are ignored.
 */
void
gj_ipv4_to_octets(uint32_t u32Addr, unsigned *pA, unsigned *pB,
    unsigned *pC, unsigned *pD)
{
	if (pA != NULL) {
		*pA = (unsigned)((u32Addr >> 24) & 0xffu);
	}
	if (pB != NULL) {
		*pB = (unsigned)((u32Addr >> 16) & 0xffu);
	}
	if (pC != NULL) {
		*pC = (unsigned)((u32Addr >> 8) & 0xffu);
	}
	if (pD != NULL) {
		*pD = (unsigned)(u32Addr & 0xffu);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ipv4_to_octets(uint32_t u32Addr, unsigned *pA, unsigned *pB,
    unsigned *pC, unsigned *pD)
    __attribute__((alias("gj_ipv4_to_octets")));
