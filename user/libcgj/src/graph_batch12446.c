/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12446: overlay ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_ok_u_12446(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamescope/steam overlay probe for the gamescope soft continuum.
 *   uint32_t __gj_overlay_ok_u_12446  (alias)
 *   __libcgj_batch12446_marker = "libcgj-batch12446"
 *
 * Exclusive continuum CREATE-ONLY (12441-12450: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12441, steamui_ok_u_12442,
 * bigpicture_ok_u_12443, deckui_ok_u_12444, cec_ok_u_12445,
 * overlay_ok_u_12446, fps_limit_ok_u_12447, hdr_ok_u_12448,
 * gamescope_soft_ready_u_12449, batch_id_12450→12450). Unique surface
 * only; no multi-def. Distinct from gj_overlay_ok_u_12246 /
 * gj_overlay_ok_u_12046 / gj_overlay_ok_u_11946. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12446_marker[] = "libcgj-batch12446";

/* Soft overlay-ok lamp: always off (not a real overlay probe). */
#define B12446_OVERLAY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12446_ok(void)
{
	return B12446_OVERLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_ok_u_12446 - overlay ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe overlay
 * presence or call libc. No parent wires.
 */
uint32_t
gj_overlay_ok_u_12446(void)
{
	(void)NULL;
	return b12446_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_ok_u_12446(void)
    __attribute__((alias("gj_overlay_ok_u_12446")));
