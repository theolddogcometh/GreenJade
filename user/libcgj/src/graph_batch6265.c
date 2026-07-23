/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6265: fan curve point duty clamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_clamp_duty_6265(uint32_t duty_pct);
 *     - Clamp duty_pct into the soft fan-curve point duty window
 *       [0, 100] (percent). Above 100 → 100.
 *   uint32_t __gj_fc_pt_clamp_duty_6265  (alias)
 *   __libcgj_batch6265_marker = "libcgj-batch6265"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs).
 * Unique gj_fc_pt_clamp_duty_6265 surface only; no multi-def.
 * Distinct from gj_fc_pt_duty_ok_6262 and gj_fc_pt_clamp_temp_6264.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6265_marker[] = "libcgj-batch6265";

#define B6265_DUTY_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6265_clamp_duty(uint32_t uDutyPct)
{
	if (uDutyPct > B6265_DUTY_MAX) {
		return B6265_DUTY_MAX;
	}
	return uDutyPct;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_clamp_duty_6265 - clamp duty_pct into [0, 100].
 *
 * Sanitizes candidate control-point duty percents before packing or PWM
 * conversion. Pure integer clamp. Does not call libc. No parent wires.
 */
uint32_t
gj_fc_pt_clamp_duty_6265(uint32_t uDutyPct)
{
	(void)NULL;
	return b6265_clamp_duty(uDutyPct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pt_clamp_duty_6265(uint32_t uDutyPct)
    __attribute__((alias("gj_fc_pt_clamp_duty_6265")));
