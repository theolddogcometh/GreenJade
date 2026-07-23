/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4503: reverse octets of a uint32_t (bswap32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_rev_bytes_u(uint32_t x);
 *     - Byte-swap x: ABCD -> DCBA. Portable pure-C; no builtins.
 *   uint32_t __gj_u32_rev_bytes_u  (alias)
 *   __libcgj_batch4503_marker = "libcgj-batch4503"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510). Distinct from gj_bswap32
 * (batch416) and gj_u32_bswap_buf (batch640) — unique gj_u32_rev_bytes_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4503_marker[] = "libcgj-batch4503";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4503_rev_bytes(uint32_t u32X)
{
	return ((u32X & 0x000000FFu) << 24)
	     | ((u32X & 0x0000FF00u) << 8)
	     | ((u32X & 0x00FF0000u) >> 8)
	     | ((u32X & 0xFF000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_rev_bytes_u - reverse the four octets of a 32-bit word.
 *
 * x: value to byte-swap
 *
 * Returns the 32-bit bswap of x. Example:
 * gj_u32_rev_bytes_u(0x11223344u) -> 0x44332211u.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_rev_bytes_u(uint32_t u32X)
{
	(void)NULL;
	return b4503_rev_bytes(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_rev_bytes_u(uint32_t u32X)
    __attribute__((alias("gj_u32_rev_bytes_u")));
