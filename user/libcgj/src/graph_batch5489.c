/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5489: FSR quality-mode validity stub.
 *
 * Surface (unique symbols):
 *   int gj_fsr_mode_ok_5489(uint32_t mode);
 *     - Return 1 if mode is in the closed range [0, 4] soft FSR quality
 *       tiers (0=off/native, 1=ultra quality, 2=quality, 3=balanced,
 *       4=performance), else 0. Stub integer gate only.
 *   int __gj_fsr_mode_ok_5489  (alias)
 *   __libcgj_batch5489_marker = "libcgj-batch5489"
 *
 * Deck Top 50 / launcher / overlay / FPS / FSR exclusive CREATE-ONLY
 * wave (5481-5490). Unique gj_fsr_mode_ok_5489 surface only; no
 * multi-def. Distinct from FPS limit stubs. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5489_marker[] = "libcgj-batch5489";

/* Soft FSR quality tiers: 0..4 inclusive. */
#define B5489_FSR_MODE_MAX  4u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5489_fsr_mode_ok(uint32_t uMode)
{
	if (uMode > B5489_FSR_MODE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsr_mode_ok_5489 - soft check that FSR quality mode is plausible.
 *
 * mode: candidate FSR quality tier (0 = off/native .. 4 = performance)
 * Returns 1 when mode is in [0, 4], else 0.
 * Does not call libc. No parent wires.
 */
int
gj_fsr_mode_ok_5489(uint32_t uMode)
{
	(void)NULL;
	return b5489_fsr_mode_ok(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fsr_mode_ok_5489(uint32_t uMode)
    __attribute__((alias("gj_fsr_mode_ok_5489")));
