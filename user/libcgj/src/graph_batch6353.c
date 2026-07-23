/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6353: Xbox pad D-pad hat encode stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_dpad_hat_6353(uint32_t up, uint32_t down,
 *                                  uint32_t left, uint32_t right);
 *     - Encode four digital D-pad lines into a hat nibble:
 *         bit0=Up, bit1=Down, bit2=Left, bit3=Right.
 *       Non-zero inputs mean pressed. Opposite pairs may both be set
 *       (no hardware exclusivity enforced here). Result in 0..15.
 *   uint32_t __gj_xpad_dpad_hat_6353  (alias)
 *   __libcgj_batch6353_marker = "libcgj-batch6353"
 *
 * Exclusive continuum CREATE-ONLY (6351-6360: Xbox controller feature
 * stubs). Unique gj_xpad_dpad_hat_6353 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6353_marker[] = "libcgj-batch6353";

#define B6353_BIT_UP     0u
#define B6353_BIT_DOWN   1u
#define B6353_BIT_LEFT   2u
#define B6353_BIT_RIGHT  3u
#define B6353_MASK       0x0fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6353_pressed(uint32_t uV)
{
	if (uV != 0u) {
		return 1u;
	}
	return 0u;
}

static uint32_t
b6353_hat(uint32_t uUp, uint32_t uDown, uint32_t uLeft, uint32_t uRight)
{
	uint32_t uH;

	uH = 0u;
	uH |= b6353_pressed(uUp) << B6353_BIT_UP;
	uH |= b6353_pressed(uDown) << B6353_BIT_DOWN;
	uH |= b6353_pressed(uLeft) << B6353_BIT_LEFT;
	uH |= b6353_pressed(uRight) << B6353_BIT_RIGHT;
	return uH & B6353_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_dpad_hat_6353 - pack D-pad Up/Down/Left/Right into a hat nibble.
 *
 * Neutral is 0 when no directions are pressed. Pure integer encode.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_xpad_dpad_hat_6353(uint32_t uUp, uint32_t uDown, uint32_t uLeft,
    uint32_t uRight)
{
	(void)NULL;
	return b6353_hat(uUp, uDown, uLeft, uRight);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xpad_dpad_hat_6353(uint32_t uUp, uint32_t uDown,
    uint32_t uLeft, uint32_t uRight)
    __attribute__((alias("gj_xpad_dpad_hat_6353")));
