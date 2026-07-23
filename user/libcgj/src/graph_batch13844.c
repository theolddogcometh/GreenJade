/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13844: deckui ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deckui_ok_u_13844(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Deck UI runtime probe for the bar3-related continuum.
 *   uint32_t __gj_deckui_ok_u_13844  (alias)
 *   __libcgj_batch13844_marker = "libcgj-batch13844"
 *
 * Exclusive continuum CREATE-ONLY (13841-13850: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13841, steamui_ok_u_13842,
 * bigpicture_ok_u_13843, deckui_ok_u_13844, cec_ok_u_13845,
 * overlay_ok_u_13846, fps_limit_ok_u_13847, hdr_ok_u_13848,
 * gamescope_soft_ready_u_13849, batch_id_13850→13850). Unique surface
 * only; no multi-def. Distinct from gj_deckui_ok_u_13644 /
 * gj_deckui_ok_u_13444 / gj_deckui_ok_u_13244. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13844_marker[] = "libcgj-batch13844";

/* Soft deckui-ok lamp: always off (not a real runtime probe). */
#define B13844_DECKUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13844_ok(void)
{
	return B13844_DECKUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deckui_ok_u_13844 - deckui ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Steam Deck UI
 * presence or call libc. No parent wires.
 */
uint32_t
gj_deckui_ok_u_13844(void)
{
	(void)NULL;
	return b13844_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deckui_ok_u_13844(void)
    __attribute__((alias("gj_deckui_ok_u_13844")));
