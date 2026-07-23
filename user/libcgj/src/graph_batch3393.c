/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3393: static smoke green gate (wave 3400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_3400(int code);
 *     - Return 1 if code is strict green PASS (exactly 0), else 0.
 *       Soft pure-data gate for product smoke status lamps on the
 *       milestone 3400 continuum. Unsigned 0/1 form.
 *   uint32_t __gj_smoke_green_3400  (alias)
 *   __libcgj_batch3393_marker = "libcgj-batch3393"
 *
 * Milestone 3400 exclusive continuum CREATE-ONLY (3391-3400). Unique
 * gj_smoke_green_3400 surface only; no multi-def. Distinct from
 * gj_smoke_green_3300 (batch3293), gj_smoke_green_u (batch3095),
 * gj_smoke_pair_ok_u (batch3194), and gj_dyn_green_3400
 * (batch3394 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3393_marker[] = "libcgj-batch3393";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if code is strict PASS green. */
static uint32_t
b3393_green(int nCode)
{
	if (nCode == 0) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_3400 - 1u iff smoke code is strict green PASS.
 *
 * code: integer smoke result code (signed so non-zero rejects).
 * Green is exactly 0. Pair with gj_dyn_green_3400 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_green_3400(int nCode)
{
	(void)NULL;
	return b3393_green(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_3400(int nCode)
    __attribute__((alias("gj_smoke_green_3400")));
