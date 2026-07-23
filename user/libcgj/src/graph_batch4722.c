/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4722: Q32 turns to degrees (fp-free).
 *
 * Surface (unique symbols):
 *   uint32_t gj_turns_to_deg_u(uint32_t turns_q32);
 *     - Map Q32 turns (one circle = 2^32) to integer degrees in
 *       [0, 359] via floor(turns * 360 / 2^32).
 *   uint32_t __gj_turns_to_deg_u  (alias)
 *   __libcgj_batch4722_marker = "libcgj-batch4722"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730). Inverse shape of
 * gj_deg_to_turns_u (batch4721) — unique gj_turns_to_deg_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4722_marker[] = "libcgj-batch4722";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * turns_q32 → degrees: floor(turns * 360 / 2^32).
 * Product turns*360 fits in uint64_t (max ~ 2^32 * 360).
 */
static uint32_t
b4722_turns_to_deg(uint32_t u32Turns)
{
	return (uint32_t)(((uint64_t)u32Turns * 360ull) >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_turns_to_deg_u - convert Q32 turns to integer degrees.
 *
 * turns_q32: angle as fraction of a circle in Q32 (2^32 = 360°)
 *
 * Returns floor degree in [0, 359]. 0x40000000 → 90, 0x80000000 → 180,
 * 0xffffffff → 359. Self-contained; no parent wires. No __int128.
 */
uint32_t
gj_turns_to_deg_u(uint32_t u32TurnsQ32)
{
	(void)NULL;
	return b4722_turns_to_deg(u32TurnsQ32);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_turns_to_deg_u(uint32_t u32TurnsQ32)
    __attribute__((alias("gj_turns_to_deg_u")));
