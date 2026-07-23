/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3096: dyn smoke soft-green gate (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_u(int code);
 *     - Return 1 if dyn-smoke code is soft-green: code is in [0, 2]
 *       (PASS, soft-warn, soft-skip). Negatives and codes > 2 fail.
 *       Soft pure-data gate for single cgj_dyn_smoke results. Unsigned
 *       0/1 form.
 *   uint32_t __gj_dyn_green_u  (alias)
 *   __libcgj_batch3096_marker = "libcgj-batch3096"
 *
 * Milestone 3100 exclusive continuum CREATE-ONLY (3091-3100). Unique
 * gj_dyn_green_u surface only; no multi-def. Distinct from
 * gj_dyn_green_pair_p (batch2694 pair int), gj_dyn_gate_pair_u
 * (batch2994 pair uint), gj_dyn_pass_p (batch2496), and
 * gj_smoke_green_u (batch3095 exact 0). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3096_marker[] = "libcgj-batch3096";

/* Inclusive upper bound of dyn-smoke soft-green band. */
#define B3096_DYN_GREEN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if dyn code is in soft-green band [0, B3096_DYN_GREEN_MAX]. */
static uint32_t
b3096_green_u(int nCode)
{
	if (nCode < 0) {
		return 0u;
	}
	if (nCode > B3096_DYN_GREEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_u - 1u iff dyn-smoke code is soft-green.
 *
 * code: dyn-smoke result
 *
 * Soft-green band is [0, 2]. Distinct from static smoke green (exact 0)
 * and paired dyn gates. Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_u(int nCode)
{
	(void)NULL;
	return b3096_green_u(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_u(int nCode)
    __attribute__((alias("gj_dyn_green_u")));
