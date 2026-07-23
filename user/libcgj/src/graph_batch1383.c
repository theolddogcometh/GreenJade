/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1383: pack two u16 halves into one u32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pack_u16x2(uint16_t lo, uint16_t hi);
 *     — Form ((uint32_t)hi << 16) | lo. lo occupies bits [15:0],
 *       hi occupies bits [31:16].
 *   uint32_t __gj_u32_pack_u16x2  (alias)
 *   __libcgj_batch1383_marker = "libcgj-batch1383"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1383_marker[] = "libcgj-batch1383";

/* ---- freestanding helpers ---------------------------------------------- */

/* Combine lo (low 16) and hi (high 16) into a 32-bit word. */
static uint32_t
b1383_pack(uint16_t u16Lo, uint16_t u16Hi)
{
	return ((uint32_t)u16Hi << 16) | (uint32_t)u16Lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pack_u16x2 — pack two uint16 values into one uint32.
 *
 * u16Lo: low half  → bits [15:0]
 * u16Hi: high half → bits [31:16]
 */
uint32_t
gj_u32_pack_u16x2(uint16_t u16Lo, uint16_t u16Hi)
{
	return b1383_pack(u16Lo, u16Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pack_u16x2(uint16_t u16Lo, uint16_t u16Hi)
    __attribute__((alias("gj_u32_pack_u16x2")));
