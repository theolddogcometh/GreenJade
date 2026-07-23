/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1757: DNS header QR response bit.
 *
 * Surface (unique symbols):
 *   int gj_dns_qr_is_response(unsigned flags);
 *     — Return 1 if the QR bit (0x8000) of a DNS header FLAGS word is
 *       set (message is a response), else 0 (query). RFC 1035 §4.1.1.
 *   int __gj_dns_qr_is_response  (alias)
 *   __libcgj_batch1757_marker = "libcgj-batch1757"
 *
 * Distinct from gj_dns_parse_header (batch113) — QR predicate only.
 * DNS/wire exclusive lightweight wave (1751–1759).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1757_marker[] = "libcgj-batch1757";

/* ---- freestanding helpers ---------------------------------------------- */

/* QR is bit 15 of the 16-bit FLAGS word (network/host same bit index). */
static int
b1757_qr_response(unsigned uFlags)
{
	return ((uFlags & 0x8000u) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_qr_is_response — 1 if FLAGS QR bit marks a DNS response.
 *
 * uFlags: 16-bit DNS header FLAGS (host-order integer of wire word)
 * Returns 1 when QR is set, else 0.
 */
int
gj_dns_qr_is_response(unsigned uFlags)
{
	return b1757_qr_response(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_qr_is_response(unsigned uFlags)
    __attribute__((alias("gj_dns_qr_is_response")));
