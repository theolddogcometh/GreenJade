/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2833: unpack host-order IPv4 into four octets (_u).
 *
 * Surface (unique symbols):
 *   void gj_ipv4_unpack_u(uint32_t addr, unsigned *a, unsigned *b,
 *                         unsigned *c, unsigned *d);
 *     - Write host-order addr into *a..*d. NULL out-pointers are ignored.
 *   void __gj_ipv4_unpack_u  (alias)
 *   __libcgj_batch2833_marker = "libcgj-batch2833"
 *
 * IPv4/endian exclusive wave (2831-2840). Distinct from
 * gj_ipv4_to_octets (batch853) — unique unpack_u surface; no multi-def.
 * Inverse of gj_ipv4_pack_u (batch2832).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2833_marker[] = "libcgj-batch2833";

/* ---- freestanding helpers ---------------------------------------------- */

/* Split host-order IPv4 into four octets; NULL outs skipped. */
static void
b2833_unpack(uint32_t u32Addr, unsigned *pA, unsigned *pB, unsigned *pC,
    unsigned *pD)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_unpack_u - unpack host-order IPv4 into four octet values.
 *
 * addr: IPv4 as host-order uint32 (e.g. 0xc0a80001 -> 192.168.0.1)
 * a,b,c,d: optional out-pointers for each octet (NULL skipped)
 *
 * Inverse of gj_ipv4_pack_u. Does not call libc.
 */
void
gj_ipv4_unpack_u(uint32_t u32Addr, unsigned *pA, unsigned *pB, unsigned *pC,
    unsigned *pD)
{
	(void)NULL;
	b2833_unpack(u32Addr, pA, pB, pC, pD);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ipv4_unpack_u(uint32_t u32Addr, unsigned *pA, unsigned *pB,
    unsigned *pC, unsigned *pD)
    __attribute__((alias("gj_ipv4_unpack_u")));
