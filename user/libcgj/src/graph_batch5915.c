/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5915: smoke XOR dyn gate disagree mirror.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_dyn_xor_5915(uint32_t smoke, uint32_t dyn);
 *     - Return 1 when the normalized smoke and dyn 0/1 lamps disagree
 *       (exclusive-or). Else 0. Soft pure-data disagree lamp for
 *       product gate diagnostics.
 *   uint32_t __gj_smoke_dyn_xor_5915  (alias)
 *   __libcgj_batch5915_marker = "libcgj-batch5915"
 *
 * Exclusive continuum CREATE-ONLY (5911-5920: smoke/dyn gate mirror
 * helpers for product). Unique gj_smoke_dyn_xor_5915 surface only; no
 * multi-def. Distinct from AND/OR mirrors 5913/5914. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5915_marker[] = "libcgj-batch5915";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5915_norm01(uint32_t u32V)
{
	return (u32V != 0u) ? 1u : 0u;
}

static uint32_t
b5915_xor01(uint32_t u32Smoke, uint32_t u32Dyn)
{
	return b5915_norm01(u32Smoke) ^ b5915_norm01(u32Dyn);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_dyn_xor_5915 - disagree (XOR) of smoke and dyn gate lamps.
 *
 * smoke: soft smoke-gate lamp
 * dyn:   soft dyn-gate lamp
 *
 * Returns 1 when exactly one side is nonzero, else 0. Soft pure-data
 * product mirror. No parent wires.
 */
uint32_t
gj_smoke_dyn_xor_5915(uint32_t smoke, uint32_t dyn)
{
	(void)NULL;
	return b5915_xor01(smoke, dyn);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_dyn_xor_5915(uint32_t smoke, uint32_t dyn)
    __attribute__((alias("gj_smoke_dyn_xor_5915")));
