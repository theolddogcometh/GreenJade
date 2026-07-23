/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13646: overlay ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_ok_u_13646(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamescope/steam overlay probe for the bar3-related continuum.
 *   uint32_t __gj_overlay_ok_u_13646  (alias)
 *   __libcgj_batch13646_marker = "libcgj-batch13646"
 *
 * Exclusive continuum CREATE-ONLY (13641-13650: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13641, steamui_ok_u_13642,
 * bigpicture_ok_u_13643, deckui_ok_u_13644, cec_ok_u_13645,
 * overlay_ok_u_13646, fps_limit_ok_u_13647, hdr_ok_u_13648,
 * gamescope_soft_ready_u_13649, batch_id_13650). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13646_marker[] = "libcgj-batch13646";

/* Soft overlay-ok lamp: always off (not a real overlay probe). */
#define B13646_OVERLAY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13646_ok(void)
{
	return B13646_OVERLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_ok_u_13646 - overlay ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe overlay
 * presence or call libc. No parent wires.
 */
uint32_t
gj_overlay_ok_u_13646(void)
{
	(void)NULL;
	return b13646_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_ok_u_13646(void)
    __attribute__((alias("gj_overlay_ok_u_13646")));
