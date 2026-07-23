/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5558: Steam Input digital action state pack.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_digital_pack_5558(uint32_t pressed, uint32_t active,
 *                                       uint32_t rising, uint32_t falling);
 *     - Pack four 1-bit flags into a state word:
 *         bit0=pressed bit1=active bit2=rising bit3=falling
 *       Non-zero inputs coerce to 1 before packing.
 *   uint32_t gj_steam_digital_pressed_5558(uint32_t packed);
 *   uint32_t gj_steam_digital_active_5558(uint32_t packed);
 *   uint32_t gj_steam_digital_edge_5558(uint32_t prev, uint32_t cur);
 *     - Compute rising|falling from prev/cur pressed bits:
 *       returns pack(cur, cur, rising, falling) with active==pressed.
 *   uint32_t gj_batch_id_5558(void);
 *   __ aliases
 *   __libcgj_batch5558_marker = "libcgj-batch5558"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs). Unique *_5558
 * surfaces only; no multi-def. No parent wires. No __int128.
 * Not affiliated with Valve; clean-room integer state packing only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5558_marker[] = "libcgj-batch5558";

#define B5558_BATCH_ID 5558u
#define B5558_BIT_PRESSED  (1u << 0)
#define B5558_BIT_ACTIVE   (1u << 1)
#define B5558_BIT_RISING   (1u << 2)
#define B5558_BIT_FALLING  (1u << 3)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5558_bit(uint32_t u32V)
{
	return (u32V != 0u) ? 1u : 0u;
}

static uint32_t
b5558_pack(uint32_t u32Pressed, uint32_t u32Active, uint32_t u32Rising,
    uint32_t u32Falling)
{
	uint32_t u32P = 0u;

	if (b5558_bit(u32Pressed) != 0u) {
		u32P |= B5558_BIT_PRESSED;
	}
	if (b5558_bit(u32Active) != 0u) {
		u32P |= B5558_BIT_ACTIVE;
	}
	if (b5558_bit(u32Rising) != 0u) {
		u32P |= B5558_BIT_RISING;
	}
	if (b5558_bit(u32Falling) != 0u) {
		u32P |= B5558_BIT_FALLING;
	}
	return u32P;
}

static uint32_t
b5558_edge(uint32_t u32Prev, uint32_t u32Cur)
{
	uint32_t u32P;
	uint32_t u32C;
	uint32_t u32Rise;
	uint32_t u32Fall;

	u32P = b5558_bit(u32Prev);
	u32C = b5558_bit(u32Cur);
	u32Rise = (u32C != 0u && u32P == 0u) ? 1u : 0u;
	u32Fall = (u32C == 0u && u32P != 0u) ? 1u : 0u;
	return b5558_pack(u32C, u32C, u32Rise, u32Fall);
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_steam_digital_pack_5558(uint32_t pressed, uint32_t active, uint32_t rising,
    uint32_t falling)
{
	(void)NULL;
	return b5558_pack(pressed, active, rising, falling);
}

uint32_t
gj_steam_digital_pressed_5558(uint32_t packed)
{
	return (packed & B5558_BIT_PRESSED) != 0u ? 1u : 0u;
}

uint32_t
gj_steam_digital_active_5558(uint32_t packed)
{
	return (packed & B5558_BIT_ACTIVE) != 0u ? 1u : 0u;
}

uint32_t
gj_steam_digital_edge_5558(uint32_t prev, uint32_t cur)
{
	return b5558_edge(prev, cur);
}

uint32_t
gj_batch_id_5558(void)
{
	return B5558_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_steam_digital_pack_5558(uint32_t pressed, uint32_t active,
    uint32_t rising, uint32_t falling)
    __attribute__((alias("gj_steam_digital_pack_5558")));

uint32_t __gj_steam_digital_pressed_5558(uint32_t packed)
    __attribute__((alias("gj_steam_digital_pressed_5558")));

uint32_t __gj_steam_digital_active_5558(uint32_t packed)
    __attribute__((alias("gj_steam_digital_active_5558")));

uint32_t __gj_steam_digital_edge_5558(uint32_t prev, uint32_t cur)
    __attribute__((alias("gj_steam_digital_edge_5558")));

uint32_t __gj_batch_id_5558(void)
    __attribute__((alias("gj_batch_id_5558")));
