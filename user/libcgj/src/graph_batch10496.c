/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10496: static smoke soft gate (wave 10500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_10500(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 10500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_10500  (alias)
 *   __libcgj_batch10496_marker = "libcgj-batch10496"
 *
 * Milestone 10500 exclusive continuum CREATE-ONLY (10491-10500). Unique
 * gj_smoke_soft_10500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_10400 (batch10396), gj_smoke_soft_10300 (batch10296),
 * gj_smoke_soft_10200 (batch10196), and gj_dyn_soft_10500
 * (batch10497 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10496_marker[] = "libcgj-batch10496";

/* Smoke soft lamp for wave 10500 (always PASS). */
#define B10496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10496_soft(void)
{
return B10496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_10500 - report smoke soft lamp for wave 10500.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_10500 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_10500(void)
{
(void)NULL;
return b10496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_10500(void)
    __attribute__((alias("gj_smoke_soft_10500")));
