/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13848: hdr ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_ok_u_13848(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDR display/runtime probe for the bar3-related continuum.
 *   uint32_t __gj_hdr_ok_u_13848  (alias)
 *   __libcgj_batch13848_marker = "libcgj-batch13848"
 *
 * Exclusive continuum CREATE-ONLY (13841-13850: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13841, steamui_ok_u_13842,
 * bigpicture_ok_u_13843, deckui_ok_u_13844, cec_ok_u_13845,
 * overlay_ok_u_13846, fps_limit_ok_u_13847, hdr_ok_u_13848,
 * gamescope_soft_ready_u_13849, batch_id_13850→13850). Unique surface
 * only; no multi-def. Distinct from gj_hdr_ok_u_13648 /
 * gj_hdr_ok_u_13448 / gj_hdr_ok_u_13248. No parent wires. No __int128.
 * No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13848_marker[] = "libcgj-batch13848";

/* Soft hdr-ok lamp: always off (not a real runtime probe). */
#define B13848_HDR_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13848_ok(void)
{
	return B13848_HDR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_ok_u_13848 - hdr ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDR display
 * presence or call libc. No parent wires.
 */
uint32_t
gj_hdr_ok_u_13848(void)
{
	(void)NULL;
	return b13848_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_ok_u_13848(void)
    __attribute__((alias("gj_hdr_ok_u_13848")));
