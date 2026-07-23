/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1759: DNS RR class IN predicate.
 *
 * Surface (unique symbols):
 *   int gj_dns_class_in_p(unsigned class);
 *     — Return 1 if class is DNS CLASS IN (1), the Internet class
 *       (RFC 1035 §3.2.4). Else 0.
 *   int __gj_dns_class_in_p  (alias)
 *   __libcgj_batch1759_marker = "libcgj-batch1759"
 *
 * DNS/wire exclusive lightweight wave (1751–1759).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1759_marker[] = "libcgj-batch1759";

/* ---- freestanding helpers ---------------------------------------------- */

/* RFC 1035 CLASS IN = 1. */
static int
b1759_class_in_p(unsigned uClass)
{
	return (uClass == 1u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_class_in_p — 1 if class is DNS CLASS IN (1).
 *
 * uClass: QCLASS / RR CLASS field (host order integer)
 * Returns 1 when uClass == 1, else 0.
 */
int
gj_dns_class_in_p(unsigned uClass)
{
	return b1759_class_in_p(uClass);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_class_in_p(unsigned uClass)
    __attribute__((alias("gj_dns_class_in_p")));
