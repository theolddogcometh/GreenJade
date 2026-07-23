/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10497: dyn smoke soft gate (wave 10500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_10500(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 10500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_10500  (alias)
 *   __libcgj_batch10497_marker = "libcgj-batch10497"
 *
 * Milestone 10500 exclusive continuum CREATE-ONLY (10491-10500). Unique
 * gj_dyn_soft_10500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_10400 (batch10397), gj_dyn_soft_10300 (batch10297),
 * gj_dyn_soft_10200 (batch10197), and gj_smoke_soft_10500
 * (batch10496 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10497_marker[] = "libcgj-batch10497";

/* Dyn soft lamp for wave 10500 (always soft-ready). */
#define B10497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10497_soft(void)
{
return B10497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_10500 - report dyn soft lamp for wave 10500.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch10496) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_10500(void)
{
(void)NULL;
return b10497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_10500(void)
    __attribute__((alias("gj_dyn_soft_10500")));
