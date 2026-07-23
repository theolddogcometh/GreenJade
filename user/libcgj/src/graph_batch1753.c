/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1753: DNS name compression pointer detect.
 *
 * Surface (unique symbols):
 *   int gj_dns_is_compression_ptr(unsigned octet);
 *     — Return 1 if the high two bits of octet are 11 (0xC0 mask),
 *       i.e. the start of a RFC 1035 §4.1.4 compression pointer.
 *       Other high-bit patterns (0x00 label, 0x40/0x80 reserved) → 0.
 *   int __gj_dns_is_compression_ptr  (alias)
 *   __libcgj_batch1753_marker = "libcgj-batch1753"
 *
 * Distinct from gj_dns_skip_name / dn_skipname — predicate only, no
 * message walk. DNS/wire exclusive lightweight wave (1751–1759).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1753_marker[] = "libcgj-batch1753";

/* ---- freestanding helpers ---------------------------------------------- */

/* True when high two bits of uOctet are 11 (compression pointer). */
static int
b1753_is_ptr(unsigned uOctet)
{
	return ((uOctet & 0xc0u) == 0xc0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_is_compression_ptr — 1 if octet starts a DNS compression ptr.
 *
 * uOctet: first wire byte of a name element (label length or pointer)
 * Returns 1 when (uOctet & 0xC0) == 0xC0, else 0.
 */
int
gj_dns_is_compression_ptr(unsigned uOctet)
{
	return b1753_is_ptr(uOctet);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_is_compression_ptr(unsigned uOctet)
    __attribute__((alias("gj_dns_is_compression_ptr")));
