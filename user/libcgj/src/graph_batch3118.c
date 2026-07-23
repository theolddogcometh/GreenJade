/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3118: pack two u16 values into one u32 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u16_pack2_u(uint16_t lo, uint16_t hi);
 *     - Form ((uint32_t)hi << 16) | lo.
 *       lo → bits [15:0], hi → bits [31:16].
 *   uint32_t __gj_u16_pack2_u  (alias)
 *   __libcgj_batch3118_marker = "libcgj-batch3118"
 *
 * Exclusive continuum CREATE-ONLY (3111-3120). Distinct from
 * gj_u32_pack_u16x2 — unique gj_u16_pack2_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3118_marker[] = "libcgj-batch3118";

/* ---- freestanding helpers ---------------------------------------------- */

/* Combine lo (low 16) and hi (high 16) into a 32-bit word. */
static uint32_t
b3118_pack2(uint16_t u16Lo, uint16_t u16Hi)
{
	return ((uint32_t)u16Hi << 16) | (uint32_t)u16Lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_pack2_u - pack two uint16 values into one uint32.
 *
 * u16Lo: low half  → bits [15:0]
 * u16Hi: high half → bits [31:16]
 */
uint32_t
gj_u16_pack2_u(uint16_t u16Lo, uint16_t u16Hi)
{
	(void)NULL;
	return b3118_pack2(u16Lo, u16Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u16_pack2_u(uint16_t u16Lo, uint16_t u16Hi)
    __attribute__((alias("gj_u16_pack2_u")));
