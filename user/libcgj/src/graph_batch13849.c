/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13849: soft gamescope/steamui id continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_13849(void);
 *     - Returns 1 (gamescope/steamui soft id stub continuum ready).
 *       Pure-data product tag that exclusive wave 13841-13850 surfaces
 *       are present for the bar3-related path; not a hard gamescope
 *       runtime probe.
 *   uint32_t __gj_gamescope_soft_ready_u_13849  (alias)
 *   __libcgj_batch13849_marker = "libcgj-batch13849"
 *
 * Exclusive continuum CREATE-ONLY (13841-13850: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13841, steamui_ok_u_13842,
 * bigpicture_ok_u_13843, deckui_ok_u_13844, cec_ok_u_13845,
 * overlay_ok_u_13846, fps_limit_ok_u_13847, hdr_ok_u_13848,
 * gamescope_soft_ready_u_13849, batch_id_13850→13850). Unique surface
 * only; no multi-def. Ok units remain 0. Distinct from prior
 * gj_gamescope_soft_ready_u_13649 / gj_gamescope_soft_ready_u_13449 /
 * gj_gamescope_soft_ready_u_13249. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13849_marker[] = "libcgj-batch13849";

/* Soft continuum-ready lamp for gamescope/steamui id stubs wave. */
#define B13849_GAMESCOPE_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13849_soft_ready(void)
{
	return B13849_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_13849 - gamescope/steamui soft id continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive
 * wave surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_13849(void)
{
	(void)NULL;
	return b13849_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_13849(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_13849")));
