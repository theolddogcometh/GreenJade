/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6358: Xbox rumble / impulse motor clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_rumble_clamp_6358(uint32_t lo, uint32_t hi);
 *     - Clamp low-frequency (left) and high-frequency (right) rumble
 *       duties each to 0..255, then pack as (hi << 8) | lo. Models the
 *       classic dual-motor Xbox pad force-feedback levels.
 *   uint32_t __gj_xpad_rumble_clamp_6358  (alias)
 *   __libcgj_batch6358_marker = "libcgj-batch6358"
 *
 * Exclusive continuum CREATE-ONLY (6351-6360: Xbox controller feature
 * stubs). Unique gj_xpad_rumble_clamp_6358 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6358_marker[] = "libcgj-batch6358";

#define B6358_DUTY_MAX  255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6358_clamp8(uint32_t uV)
{
	if (uV > B6358_DUTY_MAX) {
		return B6358_DUTY_MAX;
	}
	return uV;
}

static uint32_t
b6358_pack(uint32_t uLo, uint32_t uHi)
{
	uint32_t uL;
	uint32_t uH;

	uL = b6358_clamp8(uLo);
	uH = b6358_clamp8(uHi);
	return (uH << 8) | uL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_rumble_clamp_6358 - clamp and pack dual-motor rumble duties.
 *
 * lo: left / low-frequency motor, hi: right / high-frequency motor.
 * Result is a 16-bit packed pair in the low half of uint32_t. Pure
 * integer; no USB/HID I/O. Does not call libc. No parent wires.
 */
uint32_t
gj_xpad_rumble_clamp_6358(uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b6358_pack(uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xpad_rumble_clamp_6358(uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_xpad_rumble_clamp_6358")));
