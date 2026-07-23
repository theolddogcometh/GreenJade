/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9597: dyn smoke soft gate (wave 9600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_9600(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 9600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_9600  (alias)
 *   __libcgj_batch9597_marker = "libcgj-batch9597"
 *
 * Milestone 9600 exclusive continuum CREATE-ONLY (9591-9600). Unique
 * gj_dyn_soft_9600 surface only; no multi-def. Distinct from
 * gj_dyn_soft_9500 (batch9497), gj_dyn_soft_9400 (batch9397),
 * gj_dyn_soft_9300 (batch9297), and gj_smoke_soft_9600
 * (batch9596 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9597_marker[] = "libcgj-batch9597";

/* Dyn soft lamp for wave 9600 (always soft-ready). */
#define B9597_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9597_soft(void)
{
	return B9597_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_9600 - report dyn soft lamp for wave 9600.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch9596) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_9600(void)
{
	(void)NULL;
	return b9597_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_9600(void)
    __attribute__((alias("gj_dyn_soft_9600")));
