/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1102: freestanding 16-bit network-to-host.
 *
 * Surface (unique symbols):
 *   uint16_t gj_net_ntohs_u16(uint16_t x);
 *     — Symmetric to htons under LE host assumption: portable bswap.
 *   uint16_t __gj_net_ntohs_u16  (alias)
 *   __libcgj_batch1102_marker = "libcgj-batch1102"
 *
 * Distinct from gj_ntohs16 (batch488) / ntohs — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1102_marker[] = "libcgj-batch1102";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ntohs_u16 — 16-bit network-to-host (LE host: pure octet swap).
 */
uint16_t
gj_net_ntohs_u16(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_net_ntohs_u16(uint16_t u16X)
    __attribute__((alias("gj_net_ntohs_u16")));
