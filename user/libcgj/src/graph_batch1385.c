/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1385: pack two u8 halves into one u16.
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_pack_u8x2(uint8_t lo, uint8_t hi);
 *     — Form ((uint16_t)hi << 8) | lo. lo occupies bits [7:0],
 *       hi occupies bits [15:8].
 *   uint16_t __gj_u16_pack_u8x2  (alias)
 *   __libcgj_batch1385_marker = "libcgj-batch1385"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1385_marker[] = "libcgj-batch1385";

/* ---- freestanding helpers ---------------------------------------------- */

/* Combine lo (low 8) and hi (high 8) into a 16-bit word. */
static uint16_t
b1385_pack(uint8_t u8Lo, uint8_t u8Hi)
{
	return (uint16_t)(((uint16_t)u8Hi << 8) | (uint16_t)u8Lo);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_pack_u8x2 — pack two uint8 values into one uint16.
 *
 * u8Lo: low half  → bits [7:0]
 * u8Hi: high half → bits [15:8]
 */
uint16_t
gj_u16_pack_u8x2(uint8_t u8Lo, uint8_t u8Hi)
{
	return b1385_pack(u8Lo, u8Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_pack_u8x2(uint8_t u8Lo, uint8_t u8Hi)
    __attribute__((alias("gj_u16_pack_u8x2")));
