/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2831: single IPv4 octet range check (_u).
 *
 * Surface (unique symbols):
 *   int gj_ipv4_ok_octet_u(unsigned o);
 *     - Return 1 if o is in [0, 255], else 0.
 *   int __gj_ipv4_ok_octet_u  (alias)
 *   __libcgj_batch2831_marker = "libcgj-batch2831"
 *
 * IPv4/endian exclusive wave (2831-2840). Distinct from
 * gj_ipv4_octets_ok (batch851 four-signed-int form) — single-octet
 * unsigned _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2831_marker[] = "libcgj-batch2831";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff uO is a legal single IPv4 octet (0..255). */
static int
b2831_ok(unsigned uO)
{
	if (uO > 255u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_ok_octet_u - true when o fits in one IPv4 octet.
 *
 * o: candidate octet value (full unsigned domain)
 *
 * Returns 1 when o is in 0..255 inclusive, else 0. Does not call libc.
 */
int
gj_ipv4_ok_octet_u(unsigned uO)
{
	(void)NULL;
	return b2831_ok(uO);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv4_ok_octet_u(unsigned uO)
    __attribute__((alias("gj_ipv4_ok_octet_u")));
