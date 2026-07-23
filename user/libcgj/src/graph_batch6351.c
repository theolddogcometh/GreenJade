/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6351: Xbox pad face-button mask pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_face_mask_6351(uint32_t a, uint32_t b,
 *                                   uint32_t x, uint32_t y);
 *     - Pack A/B/X/Y digital presses into bits 0..3 (A=bit0, B=bit1,
 *       X=bit2, Y=bit3). Each input is treated as pressed when non-zero.
 *       Higher bits of the result are always clear.
 *   uint32_t __gj_xpad_face_mask_6351  (alias)
 *   __libcgj_batch6351_marker = "libcgj-batch6351"
 *
 * Exclusive continuum CREATE-ONLY (6351-6360: Xbox controller feature
 * stubs — face_mask, sys_mask, dpad_hat, lstick_dz, rstick_dz,
 * lt_scale, rt_scale, rumble_clamp, battery_ok, continuum + batch_id).
 * Unique gj_xpad_face_mask_6351 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6351_marker[] = "libcgj-batch6351";

/* Xbox-style face button bit layout (software pad policy, not HID). */
#define B6351_BIT_A  0u
#define B6351_BIT_B  1u
#define B6351_BIT_X  2u
#define B6351_BIT_Y  3u
#define B6351_MASK   0x0fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6351_pressed(uint32_t uV)
{
	if (uV != 0u) {
		return 1u;
	}
	return 0u;
}

static uint32_t
b6351_face_mask(uint32_t uA, uint32_t uB, uint32_t uX, uint32_t uY)
{
	uint32_t uM;

	uM = 0u;
	uM |= b6351_pressed(uA) << B6351_BIT_A;
	uM |= b6351_pressed(uB) << B6351_BIT_B;
	uM |= b6351_pressed(uX) << B6351_BIT_X;
	uM |= b6351_pressed(uY) << B6351_BIT_Y;
	return uM & B6351_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_face_mask_6351 - pack A/B/X/Y into a 4-bit face mask.
 *
 * Non-zero inputs mean pressed. Pure integer pack; no HID or USB.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_xpad_face_mask_6351(uint32_t uA, uint32_t uB, uint32_t uX, uint32_t uY)
{
	(void)NULL;
	return b6351_face_mask(uA, uB, uX, uY);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xpad_face_mask_6351(uint32_t uA, uint32_t uB, uint32_t uX,
    uint32_t uY) __attribute__((alias("gj_xpad_face_mask_6351")));
