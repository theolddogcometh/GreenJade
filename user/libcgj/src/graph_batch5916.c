/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5916: gate lamp invert mirror helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gate_mirror_inv_5916(uint32_t gate);
 *     - Invert a soft gate lamp after 0/1 normalize: nonzero → 0,
 *       zero → 1. Soft pure-data fail/pass flip for product gate
 *       mirror tables.
 *   uint32_t __gj_gate_mirror_inv_5916  (alias)
 *   __libcgj_batch5916_marker = "libcgj-batch5916"
 *
 * Exclusive continuum CREATE-ONLY (5911-5920: smoke/dyn gate mirror
 * helpers for product). Unique gj_gate_mirror_inv_5916 surface only; no
 * multi-def. Applies to either smoke or dyn after normalize. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5916_marker[] = "libcgj-batch5916";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5916_inv01(uint32_t u32Gate)
{
	return (u32Gate != 0u) ? 0u : 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gate_mirror_inv_5916 - invert a soft gate lamp to 0/1.
 *
 * gate: soft smoke or dyn gate lamp
 *
 * Returns 0 if gate != 0, else 1. Soft pure-data product mirror.
 * No parent wires.
 */
uint32_t
gj_gate_mirror_inv_5916(uint32_t gate)
{
	(void)NULL;
	return b5916_inv01(gate);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gate_mirror_inv_5916(uint32_t gate)
    __attribute__((alias("gj_gate_mirror_inv_5916")));
