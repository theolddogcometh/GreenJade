/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1105: freestanding 64-bit host-to-network.
 *
 * Surface (unique symbols):
 *   uint64_t gj_net_htonll_u64(uint64_t x);
 *     — Always byte-swap under little-endian host assumption
 *       (portable pure-C bswap of eight octets). Network order is BE.
 *   uint64_t __gj_net_htonll_u64  (alias)
 *   __libcgj_batch1105_marker = "libcgj-batch1105"
 *
 * Distinct from gj_bswap64 (batch417) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1105_marker[] = "libcgj-batch1105";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_htonll_u64 — 64-bit host-to-network (LE host: pure octet swap).
 */
uint64_t
gj_net_htonll_u64(uint64_t u64X)
{
	return ((u64X & 0x00000000000000ffull) << 56) |
	       ((u64X & 0x000000000000ff00ull) << 40) |
	       ((u64X & 0x0000000000ff0000ull) << 24) |
	       ((u64X & 0x00000000ff000000ull) << 8) |
	       ((u64X & 0x000000ff00000000ull) >> 8) |
	       ((u64X & 0x0000ff0000000000ull) >> 24) |
	       ((u64X & 0x00ff000000000000ull) >> 40) |
	       ((u64X & 0xff00000000000000ull) >> 56);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_net_htonll_u64(uint64_t u64X)
    __attribute__((alias("gj_net_htonll_u64")));
