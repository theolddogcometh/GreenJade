/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11346: overlay ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_ok_u_11346(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamescope/steam overlay probe for the gamescope/steamui soft
 *       continuum.
 *   uint32_t __gj_overlay_ok_u_11346  (alias)
 *   __libcgj_batch11346_marker = "libcgj-batch11346"
 *
 * Exclusive continuum CREATE-ONLY (11341-11350: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11341, steamui_ok_u_11342,
 * bigpicture_ok_u_11343, deckui_ok_u_11344, cec_ok_u_11345,
 * overlay_ok_u_11346, fps_limit_ok_u_11347, hdr_ok_u_11348,
 * gamescope_soft_ready_u_11349, batch_id_11350→11350). Unique surface
 * only; no multi-def. Distinct from gj_overlay_ok_u_11146 /
 * gj_overlay_ok_u_10946 / gj_overlay_ok_u_10046. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11346_marker[] = "libcgj-batch11346";

/* Soft overlay-ok lamp: always off (not a real runtime probe). */
#define B11346_OVERLAY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11346_overlay_ok(void)
{
	return B11346_OVERLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_ok_u_11346 - overlay ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe overlay
 * presence or call libc. No parent wires.
 */
uint32_t
gj_overlay_ok_u_11346(void)
{
	(void)NULL;
	return b11346_overlay_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_ok_u_11346(void)
    __attribute__((alias("gj_overlay_ok_u_11346")));
