/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2176: ICMP Echo Request type predicate.
 *
 * Surface (unique symbols):
 *   int gj_icmp_type_echo_p(unsigned type);
 *     - Return 1 if type is ICMP Echo Request (type 8, RFC 792), else 0.
 *   int __gj_icmp_type_echo_p  (alias)
 *   __libcgj_batch2176_marker = "libcgj-batch2176"
 *
 * Post-2170 net checksum exclusive wave (2171-2180). Type constant only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2176_marker[] = "libcgj-batch2176";

/* ICMP Echo Request type (RFC 792). */
#define B2176_ICMP_ECHO 8u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2176_echo_p(unsigned uType)
{
	return uType == B2176_ICMP_ECHO ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_icmp_type_echo_p - true if type is ICMP Echo Request (8).
 *
 * type: ICMP type field (8-bit value, zero-extended)
 *
 * Returns 1 when type == 8, else 0. Does not call libc.
 */
int
gj_icmp_type_echo_p(unsigned type)
{
	(void)NULL;
	return b2176_echo_p(type);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_icmp_type_echo_p(unsigned type)
    __attribute__((alias("gj_icmp_type_echo_p")));
