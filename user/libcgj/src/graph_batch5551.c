/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5551: controller face-button glyph codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ctrl_glyph_code_5551(uint32_t btn_id, uint32_t style);
 *     - Map a face-button id (0..15) and style nibble (0..7) to a packed
 *       glyph code: (style << 8) | base_glyph. Unknown btn → 0.
 *   uint32_t gj_ctrl_glyph_base_5551(uint32_t btn_id);
 *     - Base glyph index only (A/B/X/Y/LB/RB/... table). btn_id > 15 → 0.
 *   uint32_t gj_batch_id_5551(void);
 *   uint32_t __gj_ctrl_glyph_code_5551 / __gj_ctrl_glyph_base_5551
 *   uint32_t __gj_batch_id_5551  (aliases)
 *   __libcgj_batch5551_marker = "libcgj-batch5551"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs). Unique
 * *_5551 surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5551_marker[] = "libcgj-batch5551";

#define B5551_BATCH_ID  5551u
#define B5551_BTN_MAX   16u
#define B5551_STYLE_MAX 8u

/*
 * Base glyph indices for common face / shoulder controls.
 * Values are stable table codes, not OS scancodes.
 */
static const uint8_t s_b5551_base[B5551_BTN_MAX] = {
	0x41u, /* 0  A / Cross  */
	0x42u, /* 1  B / Circle */
	0x58u, /* 2  X / Square */
	0x59u, /* 3  Y / Triangle */
	0x4cu, /* 4  LB / L1 */
	0x52u, /* 5  RB / R1 */
	0x4cu, /* 6  LT / L2 (base L, style disambiguates) */
	0x52u, /* 7  RT / R2 */
	0x53u, /* 8  Start / Options */
	0x42u, /* 9  Back / Share (base B-like mark) */
	0x4cu, /* 10 L3 */
	0x52u, /* 11 R3 */
	0x44u, /* 12 D-pad Up */
	0x44u, /* 13 D-pad Down */
	0x44u, /* 14 D-pad Left */
	0x44u  /* 15 D-pad Right */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5551_base(uint32_t u32Btn)
{
	if (u32Btn >= B5551_BTN_MAX) {
		return 0u;
	}
	return (uint32_t)s_b5551_base[u32Btn];
}

static uint32_t
b5551_code(uint32_t u32Btn, uint32_t u32Style)
{
	uint32_t u32Base;

	u32Base = b5551_base(u32Btn);
	if (u32Base == 0u) {
		return 0u;
	}
	if (u32Style >= B5551_STYLE_MAX) {
		u32Style = 0u;
	}
	return (u32Style << 8) | u32Base;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctrl_glyph_base_5551 - face-button base glyph index.
 *
 * btn_id: 0..15 (controller face/shoulder/dpad slots); else 0.
 */
uint32_t
gj_ctrl_glyph_base_5551(uint32_t btn_id)
{
	(void)NULL;
	return b5551_base(btn_id);
}

/*
 * gj_ctrl_glyph_code_5551 - packed style|glyph for UI chrome.
 *
 * btn_id: 0..15
 * style:  0..7 (clamped to 0 when out of range)
 */
uint32_t
gj_ctrl_glyph_code_5551(uint32_t btn_id, uint32_t style)
{
	return b5551_code(btn_id, style);
}

uint32_t
gj_batch_id_5551(void)
{
	return B5551_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_ctrl_glyph_base_5551(uint32_t btn_id)
    __attribute__((alias("gj_ctrl_glyph_base_5551")));

uint32_t __gj_ctrl_glyph_code_5551(uint32_t btn_id, uint32_t style)
    __attribute__((alias("gj_ctrl_glyph_code_5551")));

uint32_t __gj_batch_id_5551(void)
    __attribute__((alias("gj_batch_id_5551")));
