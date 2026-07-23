/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5913: smoke AND dyn gate mirror helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_dyn_and_5913(uint32_t smoke, uint32_t dyn);
 *     - Return 1 only when both soft smoke and dyn gate lamps are
 *       nonzero (strict product dual-green). Else 0. Soft pure-data;
 *       does not call smoke_green_* / dyn_green_* symbols.
 *   uint32_t __gj_smoke_dyn_and_5913  (alias)
 *   __libcgj_batch5913_marker = "libcgj-batch5913"
 *
 * Exclusive continuum CREATE-ONLY (5911-5920: smoke/dyn gate mirror
 * helpers for product). Unique gj_smoke_dyn_and_5913 surface only; no
 * multi-def. Distinct from single-side mirrors 5911/5912. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5913_marker[] = "libcgj-batch5913";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5913_and01(uint32_t u32Smoke, uint32_t u32Dyn)
{
	if (u32Smoke != 0u && u32Dyn != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_dyn_and_5913 - dual-green AND of smoke and dyn gate lamps.
 *
 * smoke: soft smoke-gate lamp
 * dyn:   soft dyn-gate lamp
 *
 * Returns 1 when both are nonzero, else 0. Soft pure-data product
 * mirror. No parent wires.
 */
uint32_t
gj_smoke_dyn_and_5913(uint32_t smoke, uint32_t dyn)
{
	(void)NULL;
	return b5913_and01(smoke, dyn);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_dyn_and_5913(uint32_t smoke, uint32_t dyn)
    __attribute__((alias("gj_smoke_dyn_and_5913")));
