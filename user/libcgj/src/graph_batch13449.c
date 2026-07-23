/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13449: gamescope soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_13449(void);
 *     - Returns 1 (soft compile-time product tag: gamescope soft
 *       continuum wave 13441-13450 is present/ready; not a runtime hard
 *       probe of any gamescope/steamui stack).
 *   uint32_t __gj_gamescope_soft_ready_u_13449  (alias)
 *   __libcgj_batch13449_marker = "libcgj-batch13449"
 *
 * Exclusive continuum CREATE-ONLY (13441-13450: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→13450 —
 * gamescope_ok_u_13441, steamui_ok_u_13442, bigpicture_ok_u_13443,
 * deckui_ok_u_13444, cec_ok_u_13445, overlay_ok_u_13446,
 * fps_limit_ok_u_13447, hdr_ok_u_13448, gamescope_soft_ready_u_13449,
 * batch_id_13450). Unique gj_gamescope_soft_ready_u_13449 surface
 * only; no multi-def. Distinct from per-slot ok_u soft stubs
 * (13441-13448, all→0), gj_batch_id_13450, and prior
 * gj_gamescope_soft_ready_u_10049 (batch10049). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13449_marker[] = "libcgj-batch13449";

/* Soft gamescope continuum ready lamp (wave present). */
#define B13449_GAMESCOPE_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13449_gamescope_soft_ready(void)
{
	return B13449_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_13449 - soft gamescope continuum ready tag.
 *
 * Always returns 1 (gamescope soft wave ready/present). Does not call
 * libc or hard-probe any gamescope/steamui slot. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_13449(void)
{
	(void)NULL;
	return b13449_gamescope_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_13449(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_13449")));
