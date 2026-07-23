/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2177: ICMP Echo Reply type predicate.
 *
 * Surface (unique symbols):
 *   int gj_icmp_type_echoreply_p(unsigned type);
 *     - Return 1 if type is ICMP Echo Reply (type 0, RFC 792), else 0.
 *   int __gj_icmp_type_echoreply_p  (alias)
 *   __libcgj_batch2177_marker = "libcgj-batch2177"
 *
 * Post-2170 net checksum exclusive wave (2171-2180). Distinct from
 * gj_icmp_type_echo_p (batch2176) - reply type only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2177_marker[] = "libcgj-batch2177";

/* ICMP Echo Reply type (RFC 792). */
#define B2177_ICMP_ECHOREPLY 0u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2177_echoreply_p(unsigned uType)
{
	return uType == B2177_ICMP_ECHOREPLY ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_icmp_type_echoreply_p - true if type is ICMP Echo Reply (0).
 *
 * type: ICMP type field (8-bit value, zero-extended)
 *
 * Returns 1 when type == 0, else 0. Does not call libc.
 */
int
gj_icmp_type_echoreply_p(unsigned type)
{
	(void)NULL;
	return b2177_echoreply_p(type);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_icmp_type_echoreply_p(unsigned type)
    __attribute__((alias("gj_icmp_type_echoreply_p")));
