/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11949: soft gamescope/steamui id continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_11949(void);
 *     - Returns 1 (gamescope/steamui soft id stub continuum ready).
 *       Pure-data product tag that exclusive wave 11941-11950 surfaces
 *       are present for the bar3-related path; not a hard gamescope
 *       runtime probe.
 *   uint32_t __gj_gamescope_soft_ready_u_11949  (alias)
 *   __libcgj_batch11949_marker = "libcgj-batch11949"
 *
 * Exclusive continuum CREATE-ONLY (11941-11950: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11941, steamui_ok_u_11942,
 * bigpicture_ok_u_11943, deckui_ok_u_11944, cec_ok_u_11945,
 * overlay_ok_u_11946, fps_limit_ok_u_11947, hdr_ok_u_11948,
 * gamescope_soft_ready_u_11949, batch_id_11950). Unique surface only;
 * no multi-def. Ok units remain 0. Distinct from
 * gj_gamescope_soft_ready_u_10049. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11949_marker[] = "libcgj-batch11949";

/* Soft continuum-ready lamp for gamescope/steamui id stubs wave. */
#define B11949_GAMESCOPE_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11949_soft_ready(void)
{
	return B11949_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_11949 - gamescope/steamui soft id continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive
 * wave surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_11949(void)
{
	(void)NULL;
	return b11949_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_11949(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_11949")));
