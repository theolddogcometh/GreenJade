/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5490: FSR sharpness permille stub + wave id.
 *
 * Surface (unique symbols):
 *   int gj_fsr_sharp_ok_5490(uint32_t sharp_pm);
 *     - Return 1 if sharp_pm is in [0, 1000] (FSR sharpness in
 *       permille; 0 = softest, 1000 = sharpest), else 0.
 *   uint32_t gj_batch_id_5490(void);
 *     - Returns the compile-time graph batch number for this TU (5490).
 *   int __gj_fsr_sharp_ok_5490  (alias)
 *   uint32_t __gj_batch_id_5490  (alias)
 *   __libcgj_batch5490_marker = "libcgj-batch5490"
 *
 * Deck Top 50 / launcher / overlay / FPS / FSR exclusive CREATE-ONLY
 * wave (5481-5490). Unique surfaces only; no multi-def. Distinct from
 * gj_fsr_mode_ok_5489 and prior gj_batch_id_* symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5490_marker[] = "libcgj-batch5490";

#define B5490_SHARP_MAX  1000u
#define B5490_BATCH_ID   5490u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5490_sharp_ok(uint32_t uSharpPm)
{
	if (uSharpPm > B5490_SHARP_MAX) {
		return 0;
	}
	return 1;
}

static uint32_t
b5490_id(void)
{
	return B5490_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsr_sharp_ok_5490 - soft check that FSR sharpness is in range.
 *
 * sharp_pm: FSR sharpness in permille (0..1000)
 * Returns 1 when sharp_pm is in [0, 1000], else 0.
 * Does not call libc. No parent wires.
 */
int
gj_fsr_sharp_ok_5490(uint32_t uSharpPm)
{
	(void)NULL;
	return b5490_sharp_ok(uSharpPm);
}

/*
 * gj_batch_id_5490 - report this TU's graph batch number.
 *
 * Always returns 5490 (wave tail / FSR sharpness stub batch).
 */
uint32_t
gj_batch_id_5490(void)
{
	return b5490_id();
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_fsr_sharp_ok_5490(uint32_t uSharpPm)
    __attribute__((alias("gj_fsr_sharp_ok_5490")));

uint32_t __gj_batch_id_5490(void)
    __attribute__((alias("gj_batch_id_5490")));
