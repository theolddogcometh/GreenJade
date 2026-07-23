/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3443: static smoke green gate (wave 3450).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_3450(int code);
 *     - Return 1 if code is strict green PASS (exactly 0), else 0.
 *       Soft pure-data gate for product smoke status lamps on the
 *       milestone 3450 continuum. Unsigned 0/1 form.
 *   uint32_t __gj_smoke_green_3450  (alias)
 *   __libcgj_batch3443_marker = "libcgj-batch3443"
 *
 * Milestone 3450 exclusive continuum CREATE-ONLY (3441-3450). Unique
 * gj_smoke_green_3450 surface only; no multi-def. Distinct from
 * gj_smoke_green_3400 (batch3393), gj_smoke_green_3300 (batch3293),
 * gj_smoke_green_u (batch3095), and gj_dyn_green_3450
 * (batch3444 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3443_marker[] = "libcgj-batch3443";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if code is strict PASS green. */
static uint32_t
b3443_green(int nCode)
{
	if (nCode == 0) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_3450 - 1u iff smoke code is strict green PASS.
 *
 * code: integer smoke result code (signed so non-zero rejects).
 * Green is exactly 0. Pair with gj_dyn_green_3450 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_green_3450(int nCode)
{
	(void)NULL;
	return b3443_green(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_3450(int nCode)
    __attribute__((alias("gj_smoke_green_3450")));
