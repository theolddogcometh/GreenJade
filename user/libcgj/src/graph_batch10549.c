/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10549: gamescope soft aggregate ready stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_10549(void);
 *     - Returns 0 (soft aggregate soft/unprobed for the 10541-10550
 *       gamescope soft-stub wave — all→0; not a runtime hard probe).
 *   uint32_t __gj_gamescope_soft_ready_u_10549  (alias)
 *   __libcgj_batch10549_marker = "libcgj-batch10549"
 *
 * Exclusive continuum CREATE-ONLY (10541-10550: gamescope soft stubs —
 * all→0). Unique gj_gamescope_soft_ready_u_10549 surface only; no
 * multi-def. Distinct from the per-slot ok_u soft stubs (10541-10548)
 * and gj_batch_id_10550. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10549_marker[] = "libcgj-batch10549";

/* Soft gamescope aggregate unprobed (all→0 wave). */
#define B10549_GAMESCOPE_SOFT_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10549_gamescope_soft_ready(void)
{
	return B10549_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_10549 - soft gamescope aggregate ready stub.
 *
 * Always returns 0 (soft/unprobed aggregate for all→0 wave). Does not
 * call libc or hard-probe any slot. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_10549(void)
{
	(void)NULL;
	return b10549_gamescope_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_10549(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_10549")));
