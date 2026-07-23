/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10046: overlay ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_ok_u_10046(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamescope/steam overlay probe for the bar3-related continuum.
 *   uint32_t __gj_overlay_ok_u_10046  (alias)
 *   __libcgj_batch10046_marker = "libcgj-batch10046"
 *
 * Exclusive continuum CREATE-ONLY (10041-10050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_10041, steamui_ok_u_10042,
 * bigpicture_ok_u_10043, deckui_ok_u_10044, cec_ok_u_10045,
 * overlay_ok_u_10046, fps_limit_ok_u_10047, hdr_ok_u_10048,
 * gamescope_soft_ready_u_10049, batch_id_10050). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10046_marker[] = "libcgj-batch10046";

/* Soft overlay-ok lamp: always off (not a real overlay probe). */
#define B10046_OVERLAY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10046_ok(void)
{
	return B10046_OVERLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_ok_u_10046 - overlay ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe overlay
 * presence or call libc. No parent wires.
 */
uint32_t
gj_overlay_ok_u_10046(void)
{
	(void)NULL;
	return b10046_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_ok_u_10046(void)
    __attribute__((alias("gj_overlay_ok_u_10046")));
