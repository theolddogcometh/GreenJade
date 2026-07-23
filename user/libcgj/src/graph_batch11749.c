/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11749: gamescope soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_11749(void);
 *     - Returns 1 (soft lamp only). Indicates the gamescope/steamui soft
 *       id stub surfaces in this continuum are present; not gamescope,
 *       steamui, bigpicture, deckui, cec, overlay, fps_limit, or hdr ok.
 *   uint32_t __gj_gamescope_soft_ready_u_11749  (alias)
 *   __libcgj_batch11749_marker = "libcgj-batch11749"
 *
 * Exclusive continuum CREATE-ONLY (11741-11750: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11741, steamui_ok_u_11742,
 * bigpicture_ok_u_11743, deckui_ok_u_11744, cec_ok_u_11745,
 * overlay_ok_u_11746, fps_limit_ok_u_11747, hdr_ok_u_11748,
 * gamescope_soft_ready_u_11749, batch_id_11750→11750). Unique surface
 * only; no multi-def. Ok units remain 0. Distinct from
 * gj_gamescope_soft_ready_u_11549 / gj_gamescope_soft_ready_u_11349 /
 * gj_gamescope_soft_ready_u_11149. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11749_marker[] = "libcgj-batch11749";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B11749_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11749_soft_ready(void)
{
	return B11749_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_11749 - gamescope soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 11741-11750 surfaces are present. Does not claim gamescope/steamui/
 * bigpicture/deckui/cec/overlay/fps_limit/hdr ok and does not call
 * libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_11749(void)
{
	(void)NULL;
	return b11749_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_11749(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_11749")));
