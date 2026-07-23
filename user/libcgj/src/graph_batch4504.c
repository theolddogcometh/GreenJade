/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4504: reverse octets of a uint64_t (bswap64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_rev_bytes_u(uint64_t x);
 *     - Byte-swap x across eight octets. Portable pure-C; no builtins.
 *   uint64_t __gj_u64_rev_bytes_u  (alias)
 *   __libcgj_batch4504_marker = "libcgj-batch4504"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510). Distinct from gj_bswap64
 * (batch417) — unique gj_u64_rev_bytes_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4504_marker[] = "libcgj-batch4504";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4504_rev_bytes(uint64_t u64X)
{
	return ((u64X & 0x00000000000000FFULL) << 56)
	     | ((u64X & 0x000000000000FF00ULL) << 40)
	     | ((u64X & 0x0000000000FF0000ULL) << 24)
	     | ((u64X & 0x00000000FF000000ULL) << 8)
	     | ((u64X & 0x000000FF00000000ULL) >> 8)
	     | ((u64X & 0x0000FF0000000000ULL) >> 24)
	     | ((u64X & 0x00FF000000000000ULL) >> 40)
	     | ((u64X & 0xFF00000000000000ULL) >> 56);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_rev_bytes_u - reverse the eight octets of a 64-bit word.
 *
 * x: value to byte-swap
 *
 * Returns the 64-bit bswap of x. Example:
 * gj_u64_rev_bytes_u(0x0123456789ABCDEFULL) -> 0xEFCDAB8967452301ULL.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_rev_bytes_u(uint64_t u64X)
{
	(void)NULL;
	return b4504_rev_bytes(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_rev_bytes_u(uint64_t u64X)
    __attribute__((alias("gj_u64_rev_bytes_u")));
