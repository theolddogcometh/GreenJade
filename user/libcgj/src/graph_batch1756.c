/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1756: DNS RR type AAAA predicate.
 *
 * Surface (unique symbols):
 *   int gj_dns_type_aaaa_p(unsigned type);
 *     — Return 1 if type is DNS TYPE AAAA (28), IPv6 address
 *       (RFC 3596 §2.1). Else 0.
 *   int __gj_dns_type_aaaa_p  (alias)
 *   __libcgj_batch1756_marker = "libcgj-batch1756"
 *
 * Distinct from gj_dns_type_a_p (batch1755). DNS/wire exclusive
 * lightweight wave (1751–1759).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1756_marker[] = "libcgj-batch1756";

/* ---- freestanding helpers ---------------------------------------------- */

/* RFC 3596 TYPE AAAA = 28. */
static int
b1756_type_aaaa_p(unsigned uType)
{
	return (uType == 28u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_type_aaaa_p — 1 if type is DNS TYPE AAAA (28).
 *
 * uType: QTYPE / RR TYPE field (host order integer)
 * Returns 1 when uType == 28, else 0.
 */
int
gj_dns_type_aaaa_p(unsigned uType)
{
	return b1756_type_aaaa_p(uType);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_type_aaaa_p(unsigned uType)
    __attribute__((alias("gj_dns_type_aaaa_p")));
