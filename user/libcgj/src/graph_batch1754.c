/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1754: DNS compression pointer offset extract.
 *
 * Surface (unique symbols):
 *   unsigned gj_dns_ptr_offset(unsigned wire16);
 *     — Extract the 14-bit message offset from a big-endian 16-bit
 *       compression pointer word (RFC 1035 §4.1.4). Returns wire16 &
 *       0x3FFF. Does not validate that a pointer is well-formed —
 *       pair with gj_dns_is_compression_ptr on the high octet.
 *   unsigned __gj_dns_ptr_offset  (alias)
 *   __libcgj_batch1754_marker = "libcgj-batch1754"
 *
 * Distinct from gj_dns_skip_name / dn_expand — pure bit extract.
 * DNS/wire exclusive lightweight wave (1751–1759).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1754_marker[] = "libcgj-batch1754";

/* ---- freestanding helpers ---------------------------------------------- */

/* Low 14 bits of a 16-bit compression pointer word. */
static unsigned
b1754_ptr_off(unsigned uWire16)
{
	return uWire16 & 0x3fffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ptr_offset — 14-bit offset from a DNS compression pointer word.
 *
 * uWire16: big-endian 16-bit pointer word (top bits typically 11)
 * Returns uWire16 masked to 0x3FFF (offset into the message).
 */
unsigned
gj_dns_ptr_offset(unsigned uWire16)
{
	return b1754_ptr_off(uWire16);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_dns_ptr_offset(unsigned uWire16)
    __attribute__((alias("gj_dns_ptr_offset")));
