/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4044: dyn smoke soft-green gate (wave 4050).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4050(int code);
 *     - Return 1 if dyn-smoke code is soft-green: code is in [0, 2]
 *       (PASS, soft-warn, soft-skip). Negatives and codes > 2 fail.
 *       Soft pure-data gate for single cgj_dyn_smoke results on the
 *       milestone 4050 continuum. Unsigned 0/1 form.
 *   uint32_t __gj_dyn_green_4050  (alias)
 *   __libcgj_batch4044_marker = "libcgj-batch4044"
 *
 * Milestone 4050 exclusive continuum CREATE-ONLY (4041-4050). Unique
 * gj_dyn_green_4050 surface only; no multi-def. Distinct from
 * gj_dyn_green_3950 (batch3944), gj_dyn_green_3900 (batch3894),
 * gj_dyn_green_3700 (batch3694), and gj_smoke_green_4050
 * (batch4043 exact 0). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4044_marker[] = "libcgj-batch4044";

/* Inclusive upper bound of dyn-smoke soft-green band. */
#define B4044_DYN_GREEN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if dyn code is in soft-green band [0, B4044_DYN_GREEN_MAX]. */
static uint32_t
b4044_green(int nCode)
{
	if (nCode < 0) {
		return 0u;
	}
	if (nCode > B4044_DYN_GREEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4050 - 1u iff dyn-smoke code is soft-green.
 *
 * code: dyn-smoke result
 *
 * Soft-green band is [0, 2]. Distinct from static smoke green (exact 0)
 * and paired dyn gates. Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_4050(int nCode)
{
	(void)NULL;
	return b4044_green(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4050(int nCode)
    __attribute__((alias("gj_dyn_green_4050")));
