/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1104: freestanding 32-bit network-to-host.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_ntohl_u32(uint32_t x);
 *     — Symmetric to htonl under LE host assumption: portable bswap.
 *   uint32_t __gj_net_ntohl_u32  (alias)
 *   __libcgj_batch1104_marker = "libcgj-batch1104"
 *
 * Distinct from gj_ntohl32 (batch489) / ntohl — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1104_marker[] = "libcgj-batch1104";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ntohl_u32 — 32-bit network-to-host (LE host: pure octet swap).
 */
uint32_t
gj_net_ntohl_u32(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) |
	       ((u32X & 0x0000ff00u) << 8) |
	       ((u32X & 0x00ff0000u) >> 8) |
	       ((u32X & 0xff000000u) >> 24);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_ntohl_u32(uint32_t u32X)
    __attribute__((alias("gj_net_ntohl_u32")));
