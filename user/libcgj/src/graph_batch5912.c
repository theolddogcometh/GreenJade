/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5912: dyn gate normalize/mirror helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_gate_mirror_5912(uint32_t dyn);
 *     - Normalize a soft dyn-gate lamp to a strict 0/1 product mirror:
 *       nonzero → 1, zero → 0. Soft pure-data only; does not run dyn
 *       smoke or touch the dyn_green_* lamps.
 *   uint32_t __gj_dyn_gate_mirror_5912  (alias)
 *   __libcgj_batch5912_marker = "libcgj-batch5912"
 *
 * Exclusive continuum CREATE-ONLY (5911-5920: smoke/dyn gate mirror
 * helpers for product). Unique gj_dyn_gate_mirror_5912 surface only; no
 * multi-def. Distinct from gj_dyn_green_5900 (batch5894) and
 * gj_smoke_gate_mirror_5911. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5912_marker[] = "libcgj-batch5912";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5912_norm01(uint32_t u32V)
{
	return (u32V != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_gate_mirror_5912 - normalize dyn gate to strict 0/1.
 *
 * dyn: soft dyn-gate lamp (any nonzero is treated as soft-green)
 *
 * Returns 1 if dyn != 0, else 0. Soft pure-data mirror helper for
 * product scoring. No parent wires.
 */
uint32_t
gj_dyn_gate_mirror_5912(uint32_t dyn)
{
	(void)NULL;
	return b5912_norm01(dyn);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_gate_mirror_5912(uint32_t dyn)
    __attribute__((alias("gj_dyn_gate_mirror_5912")));
