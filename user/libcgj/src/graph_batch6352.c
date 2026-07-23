/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6352: Xbox pad system-button mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_sys_mask_6352(uint32_t view, uint32_t menu,
 *                                  uint32_t guide, uint32_t share);
 *     - Pack View/Menu/Guide/Share digital presses into bits 0..3
 *       (View=bit0, Menu=bit1, Guide=bit2, Share=bit3). Each input is
 *       pressed when non-zero. Higher bits clear.
 *   uint32_t __gj_xpad_sys_mask_6352  (alias)
 *   __libcgj_batch6352_marker = "libcgj-batch6352"
 *
 * Exclusive continuum CREATE-ONLY (6351-6360: Xbox controller feature
 * stubs). Unique gj_xpad_sys_mask_6352 surface only; no multi-def.
 * Distinct from gj_xpad_face_mask_6351. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6352_marker[] = "libcgj-batch6352";

/* System / chrome button layout for Xbox One / Series-style pads. */
#define B6352_BIT_VIEW   0u
#define B6352_BIT_MENU   1u
#define B6352_BIT_GUIDE  2u
#define B6352_BIT_SHARE  3u
#define B6352_MASK       0x0fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6352_pressed(uint32_t uV)
{
	if (uV != 0u) {
		return 1u;
	}
	return 0u;
}

static uint32_t
b6352_sys_mask(uint32_t uView, uint32_t uMenu, uint32_t uGuide,
    uint32_t uShare)
{
	uint32_t uM;

	uM = 0u;
	uM |= b6352_pressed(uView) << B6352_BIT_VIEW;
	uM |= b6352_pressed(uMenu) << B6352_BIT_MENU;
	uM |= b6352_pressed(uGuide) << B6352_BIT_GUIDE;
	uM |= b6352_pressed(uShare) << B6352_BIT_SHARE;
	return uM & B6352_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_sys_mask_6352 - pack View/Menu/Guide/Share into a 4-bit mask.
 *
 * Share is Series-era; zero share when unavailable. Pure integer pack.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_xpad_sys_mask_6352(uint32_t uView, uint32_t uMenu, uint32_t uGuide,
    uint32_t uShare)
{
	(void)NULL;
	return b6352_sys_mask(uView, uMenu, uGuide, uShare);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xpad_sys_mask_6352(uint32_t uView, uint32_t uMenu,
    uint32_t uGuide, uint32_t uShare)
    __attribute__((alias("gj_xpad_sys_mask_6352")));
