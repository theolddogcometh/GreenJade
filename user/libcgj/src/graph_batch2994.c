/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2994: paired dyn smoke soft-gate (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_gate_pair_u(int code_a, int code_b);
 *     - Return 1 if both dyn-smoke codes are soft-green: each code is
 *       in [0, 2] (PASS, soft-warn, soft-skip). Negatives and codes > 2
 *       fail. Soft pure-data gate for paired cgj_dyn_smoke results.
 *       Unsigned form.
 *   uint32_t __gj_dyn_gate_pair_u  (alias)
 *   __libcgj_batch2994_marker = "libcgj-batch2994"
 *
 * Milestone 3000 exclusive continuum CREATE-ONLY (2991-3000). Unique
 * gj_dyn_gate_pair_u surface only; no multi-def. Distinct from
 * gj_dyn_green_pair_p (batch2694 int form), gj_smoke_gate_pair_u
 * (batch2993 exact 0 pair), gj_dyn_pass_p (batch2496), and
 * gj_dyn_smoke_ok_p (batch2396). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2994_marker[] = "libcgj-batch2994";

/* Inclusive upper bound of dyn-smoke soft-green band. */
#define B2994_DYN_GREEN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if one dyn code is in soft-green band [0, B2994_DYN_GREEN_MAX]. */
static uint32_t
b2994_one_green(int nCode)
{
	if (nCode < 0) {
		return 0u;
	}
	if (nCode > B2994_DYN_GREEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* 1u if both dyn codes are soft-green. */
static uint32_t
b2994_gate_pair(int nCodeA, int nCodeB)
{
	if (b2994_one_green(nCodeA) == 0u) {
		return 0u;
	}
	if (b2994_one_green(nCodeB) == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_gate_pair_u - 1u iff both dyn-smoke codes are soft-green.
 *
 * code_a: first dyn-smoke result
 * code_b: second dyn-smoke result
 *
 * Soft-green band is [0, 2] for each code. Distinct from static smoke
 * gate pair (exact 0) and single-code dyn pass. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_dyn_gate_pair_u(int nCodeA, int nCodeB)
{
	(void)NULL;
	return b2994_gate_pair(nCodeA, nCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_gate_pair_u(int nCodeA, int nCodeB)
    __attribute__((alias("gj_dyn_gate_pair_u")));
