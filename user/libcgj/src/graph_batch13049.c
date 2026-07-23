/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13049: soft gamescope/steamui id continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_13049(void);
 *     - Returns 1 (gamescope/steamui soft id stub continuum ready).
 *       Pure-data product tag that exclusive wave 13041-13050 surfaces
 *       are present for the gamescope soft path; not a hard gamescope
 *       runtime probe. Ok units remain 0.
 *   uint32_t __gj_gamescope_soft_ready_u_13049  (alias)
 *   __libcgj_batch13049_marker = "libcgj-batch13049"
 *
 * Exclusive continuum CREATE-ONLY (13041-13050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13041, steamui_ok_u_13042,
 * bigpicture_ok_u_13043, deckui_ok_u_13044, cec_ok_u_13045,
 * overlay_ok_u_13046, fps_limit_ok_u_13047, hdr_ok_u_13048,
 * gamescope_soft_ready_u_13049, batch_id_13050→13050). Unique surface
 * only; no multi-def. Ok units remain 0. Distinct from
 * gj_gamescope_soft_ready_u_12849 / gj_gamescope_soft_ready_u_12649 /
 * gj_gamescope_soft_ready_u_12449. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13049_marker[] = "libcgj-batch13049";

/* Soft continuum-ready lamp for gamescope/steamui id stubs wave. */
#define B13049_GAMESCOPE_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13049_soft_ready(void)
{
	return B13049_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_13049 - gamescope/steamui soft id continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive
 * wave surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_13049(void)
{
	(void)NULL;
	return b13049_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_13049(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_13049")));
