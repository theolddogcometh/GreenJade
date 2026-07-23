/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11149: gamescope soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_11149(void);
 *     - Returns 1 (soft lamp only). Indicates the gamescope/steamui soft
 *       id stub surfaces in this continuum are present; not gamescope,
 *       steamui, bigpicture, deckui, cec, overlay, fps_limit, or hdr ok.
 *   uint32_t __gj_gamescope_soft_ready_u_11149  (alias)
 *   __libcgj_batch11149_marker = "libcgj-batch11149"
 *
 * Exclusive continuum CREATE-ONLY (11141-11150: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11141, steamui_ok_u_11142,
 * bigpicture_ok_u_11143, deckui_ok_u_11144, cec_ok_u_11145,
 * overlay_ok_u_11146, fps_limit_ok_u_11147, hdr_ok_u_11148,
 * gamescope_soft_ready_u_11149, batch_id_11150). Unique surface only;
 * no multi-def. Ok units remain 0. Distinct from
 * gj_gamescope_soft_ready_u_10949 / gj_gamescope_soft_ready_u_10749 /
 * gj_gamescope_soft_ready_u_10049. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11149_marker[] = "libcgj-batch11149";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B11149_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11149_soft_ready(void)
{
	return B11149_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_11149 - gamescope soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 11141-11150 surfaces are present. Does not claim gamescope/steamui/
 * bigpicture/deckui/cec/overlay/fps_limit/hdr ok and does not call
 * libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_11149(void)
{
	(void)NULL;
	return b11149_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_11149(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_11149")));
