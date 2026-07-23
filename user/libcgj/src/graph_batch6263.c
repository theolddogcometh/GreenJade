/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6263: fan curve full point soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_ok_6263(uint32_t temp_c, uint32_t duty_pct);
 *     - Return 1 if both temp_c is in [0, 120] and duty_pct is in
 *       [0, 100], else 0. Soft pure-data full control-point validity
 *       check; not a live hwmon sample.
 *   uint32_t __gj_fc_pt_ok_6263  (alias)
 *   __libcgj_batch6263_marker = "libcgj-batch6263"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs).
 * Unique gj_fc_pt_ok_6263 surface only; no multi-def. Distinct from
 * gj_fc_pt_temp_ok_6261 and gj_fc_pt_duty_ok_6262 (does not call them;
 * no parent wires). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6263_marker[] = "libcgj-batch6263";

#define B6263_TEMP_MAX  120u
#define B6263_DUTY_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6263_pt_ok(uint32_t uTempC, uint32_t uDutyPct)
{
	if (uTempC > B6263_TEMP_MAX) {
		return 0u;
	}
	if (uDutyPct > B6263_DUTY_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_ok_6263 - 1 if (temp_c, duty_pct) is a soft-valid curve point.
 *
 * Joint validity for a single fan curve control point. Self-contained
 * bounds (does not call sibling batch symbols). No parent wires.
 */
uint32_t
gj_fc_pt_ok_6263(uint32_t uTempC, uint32_t uDutyPct)
{
	(void)NULL;
	return b6263_pt_ok(uTempC, uDutyPct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pt_ok_6263(uint32_t uTempC, uint32_t uDutyPct)
    __attribute__((alias("gj_fc_pt_ok_6263")));
