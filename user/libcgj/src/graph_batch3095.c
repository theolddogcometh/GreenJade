/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3095: static smoke green gate (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_u(int code);
 *     - Return 1 if code is strict green PASS (exactly 0), else 0.
 *       Soft pure-data gate for product smoke status lamps. Unsigned
 *       0/1 form.
 *   uint32_t __gj_smoke_green_u  (alias)
 *   __libcgj_batch3095_marker = "libcgj-batch3095"
 *
 * Milestone 3100 exclusive continuum CREATE-ONLY (3091-3100). Unique
 * gj_smoke_green_u surface only; no multi-def. Distinct from
 * gj_smoke_green_p (batch2394 int form), gj_smoke_gate_pair_u
 * (batch2993 pair), gj_smoke_pass_p (batch2494), and gj_dyn_green_u
 * (batch3096 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3095_marker[] = "libcgj-batch3095";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if code is strict PASS green. */
static uint32_t
b3095_green_u(int nCode)
{
	if (nCode == 0) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_u - 1u iff smoke code is strict green PASS.
 *
 * code: integer smoke result code (signed so non-zero rejects).
 * Green is exactly 0. Pair with gj_dyn_green_u for the dyn soft-band
 * counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_green_u(int nCode)
{
	(void)NULL;
	return b3095_green_u(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_u(int nCode)
    __attribute__((alias("gj_smoke_green_u")));
