/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12249: soft gamescope/steamui id continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_12249(void);
 *     - Returns 1 (gamescope/steamui soft id stub continuum ready).
 *       Pure-data product tag that exclusive wave 12241-12250 surfaces
 *       are present for the bar3-related path; not a hard gamescope
 *       runtime probe. Ok units remain 0.
 *   uint32_t __gj_gamescope_soft_ready_u_12249  (alias)
 *   __libcgj_batch12249_marker = "libcgj-batch12249"
 *
 * Exclusive continuum CREATE-ONLY (12241-12250: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12241, steamui_ok_u_12242,
 * bigpicture_ok_u_12243, deckui_ok_u_12244, cec_ok_u_12245,
 * overlay_ok_u_12246, fps_limit_ok_u_12247, hdr_ok_u_12248,
 * gamescope_soft_ready_u_12249, batch_id_12250→12250). Unique surface
 * only; no multi-def. Ok units remain 0. Distinct from
 * gj_gamescope_soft_ready_u_11949 / gj_gamescope_soft_ready_u_11749 /
 * gj_gamescope_soft_ready_u_11549. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12249_marker[] = "libcgj-batch12249";

/* Soft continuum-ready lamp for gamescope/steamui id stubs wave. */
#define B12249_GAMESCOPE_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12249_soft_ready(void)
{
	return B12249_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_12249 - gamescope/steamui soft id continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive
 * wave surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_12249(void)
{
	(void)NULL;
	return b12249_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_12249(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_12249")));
