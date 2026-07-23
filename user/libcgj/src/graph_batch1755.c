/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1755: DNS RR type A predicate.
 *
 * Surface (unique symbols):
 *   int gj_dns_type_a_p(unsigned type);
 *     — Return 1 if type is DNS TYPE A (1), IPv4 host address
 *       (RFC 1035 §3.2.2). Else 0.
 *   int __gj_dns_type_a_p  (alias)
 *   __libcgj_batch1755_marker = "libcgj-batch1755"
 *
 * DNS/wire exclusive lightweight wave (1751–1759).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1755_marker[] = "libcgj-batch1755";

/* ---- freestanding helpers ---------------------------------------------- */

/* RFC 1035 TYPE A = 1. */
static int
b1755_type_a_p(unsigned uType)
{
	return (uType == 1u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_type_a_p — 1 if type is DNS TYPE A (1).
 *
 * uType: QTYPE / RR TYPE field (host order integer)
 * Returns 1 when uType == 1, else 0.
 */
int
gj_dns_type_a_p(unsigned uType)
{
	return b1755_type_a_p(uType);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_type_a_p(unsigned uType)
    __attribute__((alias("gj_dns_type_a_p")));
