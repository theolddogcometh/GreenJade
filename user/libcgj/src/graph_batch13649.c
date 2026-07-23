/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13649: soft gamescope/steamui id continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_13649(void);
 *     - Returns 1 (gamescope/steamui soft id stub continuum ready).
 *       Pure-data product tag that exclusive wave 13641-13650 surfaces
 *       are present for the bar3-related path; not a hard gamescope
 *       runtime probe.
 *   uint32_t __gj_gamescope_soft_ready_u_13649  (alias)
 *   __libcgj_batch13649_marker = "libcgj-batch13649"
 *
 * Exclusive continuum CREATE-ONLY (13641-13650: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13641, steamui_ok_u_13642,
 * bigpicture_ok_u_13643, deckui_ok_u_13644, cec_ok_u_13645,
 * overlay_ok_u_13646, fps_limit_ok_u_13647, hdr_ok_u_13648,
 * gamescope_soft_ready_u_13649, batch_id_13650). Unique surface only;
 * no multi-def. Ok units remain 0. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13649_marker[] = "libcgj-batch13649";

/* Soft continuum-ready lamp for gamescope/steamui id stubs wave. */
#define B13649_GAMESCOPE_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13649_soft_ready(void)
{
	return B13649_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_13649 - gamescope/steamui soft id continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive
 * wave surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_13649(void)
{
	(void)NULL;
	return b13649_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_13649(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_13649")));
