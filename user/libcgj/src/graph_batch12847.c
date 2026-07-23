/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12847: fps_limit ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fps_limit_ok_u_12847(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FPS limiter configuration probe for the gamescope soft continuum.
 *   uint32_t __gj_fps_limit_ok_u_12847  (alias)
 *   __libcgj_batch12847_marker = "libcgj-batch12847"
 *
 * Exclusive continuum CREATE-ONLY (12841-12850: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12841, steamui_ok_u_12842,
 * bigpicture_ok_u_12843, deckui_ok_u_12844, cec_ok_u_12845,
 * overlay_ok_u_12846, fps_limit_ok_u_12847, hdr_ok_u_12848,
 * gamescope_soft_ready_u_12849, batch_id_12850→12850). Unique surface
 * only; no multi-def. Distinct from gj_fps_limit_ok_u_12447 /
 * gj_fps_limit_ok_u_12247 / gj_fps_limit_ok_u_12147. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12847_marker[] = "libcgj-batch12847";

/* Soft fps-limit-ok lamp: always off (not a real limiter probe). */
#define B12847_FPS_LIMIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12847_ok(void)
{
	return B12847_FPS_LIMIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_limit_ok_u_12847 - fps_limit ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe FPS limiter
 * configuration or call libc. No parent wires.
 */
uint32_t
gj_fps_limit_ok_u_12847(void)
{
	(void)NULL;
	return b12847_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fps_limit_ok_u_12847(void)
    __attribute__((alias("gj_fps_limit_ok_u_12847")));
