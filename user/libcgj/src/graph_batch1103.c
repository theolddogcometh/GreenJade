/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1103: freestanding 32-bit host-to-network.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_htonl_u32(uint32_t x);
 *     — Always byte-swap under little-endian host assumption
 *       (portable pure-C bswap of four octets). Network order is BE.
 *   uint32_t __gj_net_htonl_u32  (alias)
 *   __libcgj_batch1103_marker = "libcgj-batch1103"
 *
 * Distinct from gj_htonl32 (batch487) / htonl — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1103_marker[] = "libcgj-batch1103";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_htonl_u32 — 32-bit host-to-network (LE host: pure octet swap).
 */
uint32_t
gj_net_htonl_u32(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) |
	       ((u32X & 0x0000ff00u) << 8) |
	       ((u32X & 0x00ff0000u) >> 8) |
	       ((u32X & 0xff000000u) >> 24);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_htonl_u32(uint32_t u32X)
    __attribute__((alias("gj_net_htonl_u32")));
