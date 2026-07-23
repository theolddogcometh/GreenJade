/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13843: bigpicture ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_13843(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Big Picture mode probe for the bar3-related continuum.
 *   uint32_t __gj_bigpicture_ok_u_13843  (alias)
 *   __libcgj_batch13843_marker = "libcgj-batch13843"
 *
 * Exclusive continuum CREATE-ONLY (13841-13850: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13841, steamui_ok_u_13842,
 * bigpicture_ok_u_13843, deckui_ok_u_13844, cec_ok_u_13845,
 * overlay_ok_u_13846, fps_limit_ok_u_13847, hdr_ok_u_13848,
 * gamescope_soft_ready_u_13849, batch_id_13850→13850). Unique surface
 * only; no multi-def. Distinct from gj_bigpicture_ok_u_13643 /
 * gj_bigpicture_ok_u_13443 / gj_bigpicture_ok_u_13243. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13843_marker[] = "libcgj-batch13843";

/* Soft bigpicture-ok lamp: always off (not a real runtime probe). */
#define B13843_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13843_ok(void)
{
	return B13843_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_13843 - bigpicture ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Big Picture
 * mode presence or call libc. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_13843(void)
{
	(void)NULL;
	return b13843_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_13843(void)
    __attribute__((alias("gj_bigpicture_ok_u_13843")));
