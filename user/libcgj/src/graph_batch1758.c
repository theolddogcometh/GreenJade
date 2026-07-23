/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1758: DNS RCODE NOERROR predicate.
 *
 * Surface (unique symbols):
 *   int gj_dns_rcode_ok(unsigned rcode);
 *     — Return 1 if rcode is NOERROR (0), the success response code
 *       (RFC 1035 §4.1.1). Non-zero RCODEs (FORMERR, SERVFAIL, …) → 0.
 *       Callers that hold full FLAGS should mask with 0xF first.
 *   int __gj_dns_rcode_ok  (alias)
 *   __libcgj_batch1758_marker = "libcgj-batch1758"
 *
 * DNS/wire exclusive lightweight wave (1751–1759).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1758_marker[] = "libcgj-batch1758";

/* ---- freestanding helpers ---------------------------------------------- */

/* RCODE 0 = NOERROR. */
static int
b1758_rcode_ok(unsigned uRcode)
{
	return (uRcode == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_rcode_ok — 1 if rcode is DNS NOERROR (0).
 *
 * uRcode: response code field (low 4 bits of FLAGS, or bare RCODE)
 * Returns 1 when uRcode == 0, else 0.
 */
int
gj_dns_rcode_ok(unsigned uRcode)
{
	return b1758_rcode_ok(uRcode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_rcode_ok(unsigned uRcode)
    __attribute__((alias("gj_dns_rcode_ok")));
