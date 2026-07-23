/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6264: fan curve point temperature clamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_clamp_temp_6264(uint32_t temp_c);
 *     - Clamp temp_c into the soft fan-curve point temperature window
 *       [0, 120] (°C-style). Below 0 is impossible for uint32_t; above
 *       120 → 120.
 *   uint32_t __gj_fc_pt_clamp_temp_6264  (alias)
 *   __libcgj_batch6264_marker = "libcgj-batch6264"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs).
 * Unique gj_fc_pt_clamp_temp_6264 surface only; no multi-def.
 * Distinct from gj_fc_pt_temp_ok_6261 (predicate vs clamp).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6264_marker[] = "libcgj-batch6264";

#define B6264_TEMP_MAX  120u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6264_clamp_temp(uint32_t uTempC)
{
	if (uTempC > B6264_TEMP_MAX) {
		return B6264_TEMP_MAX;
	}
	return uTempC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_clamp_temp_6264 - clamp temp_c into [0, 120].
 *
 * Sanitizes candidate control-point temperatures before packing or
 * interpolation. Pure integer clamp. Does not call libc. No parent wires.
 */
uint32_t
gj_fc_pt_clamp_temp_6264(uint32_t uTempC)
{
	(void)NULL;
	return b6264_clamp_temp(uTempC);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pt_clamp_temp_6264(uint32_t uTempC)
    __attribute__((alias("gj_fc_pt_clamp_temp_6264")));
