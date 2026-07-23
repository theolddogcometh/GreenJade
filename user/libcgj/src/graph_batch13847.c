/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13847: fps_limit ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fps_limit_ok_u_13847(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FPS limiter configuration probe for the bar3-related continuum.
 *   uint32_t __gj_fps_limit_ok_u_13847  (alias)
 *   __libcgj_batch13847_marker = "libcgj-batch13847"
 *
 * Exclusive continuum CREATE-ONLY (13841-13850: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13841, steamui_ok_u_13842,
 * bigpicture_ok_u_13843, deckui_ok_u_13844, cec_ok_u_13845,
 * overlay_ok_u_13846, fps_limit_ok_u_13847, hdr_ok_u_13848,
 * gamescope_soft_ready_u_13849, batch_id_13850→13850). Unique surface
 * only; no multi-def. Distinct from gj_fps_limit_ok_u_13647 /
 * gj_fps_limit_ok_u_13447 / gj_fps_limit_ok_u_13247. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13847_marker[] = "libcgj-batch13847";

/* Soft fps_limit-ok lamp: always off (not a real runtime probe). */
#define B13847_FPS_LIMIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13847_ok(void)
{
	return B13847_FPS_LIMIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_limit_ok_u_13847 - fps_limit ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe FPS limiter
 * configuration or call libc. No parent wires.
 */
uint32_t
gj_fps_limit_ok_u_13847(void)
{
	(void)NULL;
	return b13847_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fps_limit_ok_u_13847(void)
    __attribute__((alias("gj_fps_limit_ok_u_13847")));
