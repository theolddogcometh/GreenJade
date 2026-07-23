/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch851: IPv4 octet range check (signed ints).
 *
 * Surface (unique symbols):
 *   int gj_ipv4_octets_ok(int a, int b, int c, int d);
 *     — 1 if each octet is in [0, 255], else 0.
 *   int __gj_ipv4_octets_ok  (alias)
 *   __libcgj_batch851_marker = "libcgj-batch851"
 *
 * Signed int parameters so values outside 0..255 are rejected.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch851_marker[] = "libcgj-batch851";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_octets_ok — validate four signed octets as an IPv4 address.
 *
 * Each of a,b,c,d must lie in [0, 255]. Values outside (including
 * negatives and >255) yield 0. Does not check reserved ranges.
 */
int
gj_ipv4_octets_ok(int nA, int nB, int nC, int nD)
{
	if (nA < 0 || nA > 255) {
		return 0;
	}
	if (nB < 0 || nB > 255) {
		return 0;
	}
	if (nC < 0 || nC > 255) {
		return 0;
	}
	if (nD < 0 || nD > 255) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv4_octets_ok(int nA, int nB, int nC, int nD)
    __attribute__((alias("gj_ipv4_octets_ok")));
