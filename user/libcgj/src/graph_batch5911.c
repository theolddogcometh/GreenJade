/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5911: smoke gate normalize/mirror helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_gate_mirror_5911(uint32_t smoke);
 *     - Normalize a soft smoke-gate lamp to a strict 0/1 product mirror:
 *       nonzero → 1, zero → 0. Soft pure-data only; does not run smoke
 *       tests or touch the static smoke_green_* lamps.
 *   uint32_t __gj_smoke_gate_mirror_5911  (alias)
 *   __libcgj_batch5911_marker = "libcgj-batch5911"
 *
 * Exclusive continuum CREATE-ONLY (5911-5920: smoke/dyn gate mirror
 * helpers for product). Unique gj_smoke_gate_mirror_5911 surface only;
 * no multi-def. Distinct from gj_smoke_green_5900 (batch5893). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5911_marker[] = "libcgj-batch5911";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5911_norm01(uint32_t u32V)
{
	return (u32V != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_gate_mirror_5911 - normalize smoke gate to strict 0/1.
 *
 * smoke: soft smoke-gate lamp (any nonzero is treated as PASS)
 *
 * Returns 1 if smoke != 0, else 0. Soft pure-data mirror helper for
 * product scoring. No parent wires.
 */
uint32_t
gj_smoke_gate_mirror_5911(uint32_t smoke)
{
	(void)NULL;
	return b5911_norm01(smoke);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_gate_mirror_5911(uint32_t smoke)
    __attribute__((alias("gj_smoke_gate_mirror_5911")));
