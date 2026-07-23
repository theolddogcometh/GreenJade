/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11746: overlay ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_ok_u_11746(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamescope/steam overlay runtime probe for the gamescope/steamui
 *       soft continuum.
 *   uint32_t __gj_overlay_ok_u_11746  (alias)
 *   __libcgj_batch11746_marker = "libcgj-batch11746"
 *
 * Exclusive continuum CREATE-ONLY (11741-11750: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11741, steamui_ok_u_11742,
 * bigpicture_ok_u_11743, deckui_ok_u_11744, cec_ok_u_11745,
 * overlay_ok_u_11746, fps_limit_ok_u_11747, hdr_ok_u_11748,
 * gamescope_soft_ready_u_11749, batch_id_11750→11750). Unique surface
 * only; no multi-def. Distinct from gj_overlay_ok_u_11546 /
 * gj_overlay_ok_u_11346 / gj_overlay_ok_u_11146. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11746_marker[] = "libcgj-batch11746";

/* Soft overlay-ok lamp: always off (not a real runtime probe). */
#define B11746_OVERLAY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11746_overlay_ok(void)
{
	return B11746_OVERLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_ok_u_11746 - overlay ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe overlay
 * presence or call libc. No parent wires.
 */
uint32_t
gj_overlay_ok_u_11746(void)
{
	(void)NULL;
	return b11746_overlay_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_ok_u_11746(void)
    __attribute__((alias("gj_overlay_ok_u_11746")));
