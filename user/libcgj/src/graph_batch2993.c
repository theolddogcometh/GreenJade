/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2993: paired static smoke gate (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_gate_pair_u(int code_a, int code_b);
 *     - Return 1 if both static smoke codes are strict green PASS
 *       (exactly 0), else 0. Soft pure-data gate for paired static
 *       smoke lamps (e.g. host + guest static smoke). Unsigned form.
 *   uint32_t __gj_smoke_gate_pair_u  (alias)
 *   __libcgj_batch2993_marker = "libcgj-batch2993"
 *
 * Milestone 3000 exclusive continuum CREATE-ONLY (2991-3000). Unique
 * gj_smoke_gate_pair_u surface only; no multi-def. Distinct from
 * gj_smoke_green_pair_p (batch2693 int form), gj_smoke_both_pass_p
 * (batch2594), gj_smoke_pass_p (batch2494), and gj_dyn_gate_pair_u
 * (batch2994). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2993_marker[] = "libcgj-batch2993";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if both codes are strict green PASS (exactly 0). */
static uint32_t
b2993_gate_pair(int nCodeA, int nCodeB)
{
	if (nCodeA != 0) {
		return 0u;
	}
	if (nCodeB != 0) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_gate_pair_u - 1u iff both static smoke codes are green.
 *
 * code_a: first static smoke result
 * code_b: second static smoke result
 *
 * Green PASS is exactly 0 for both. Pair with gj_dyn_gate_pair_u for
 * the dyn soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_gate_pair_u(int nCodeA, int nCodeB)
{
	(void)NULL;
	return b2993_gate_pair(nCodeA, nCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_gate_pair_u(int nCodeA, int nCodeB)
    __attribute__((alias("gj_smoke_gate_pair_u")));
