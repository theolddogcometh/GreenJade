/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12644: deckui ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deckui_ok_u_12644(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Deck UI runtime probe for the gamescope soft continuum.
 *   uint32_t __gj_deckui_ok_u_12644  (alias)
 *   __libcgj_batch12644_marker = "libcgj-batch12644"
 *
 * Exclusive continuum CREATE-ONLY (12641-12650: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12641, steamui_ok_u_12642,
 * bigpicture_ok_u_12643, deckui_ok_u_12644, cec_ok_u_12645,
 * overlay_ok_u_12646, fps_limit_ok_u_12647, hdr_ok_u_12648,
 * gamescope_soft_ready_u_12649, batch_id_12650→12650). Unique surface
 * only; no multi-def. Distinct from gj_deckui_ok_u_12444 /
 * gj_deckui_ok_u_12244 / gj_deckui_ok_u_12044. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12644_marker[] = "libcgj-batch12644";

/* Soft deckui-ok lamp: always off (not a real Deck UI probe). */
#define B12644_DECKUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12644_ok(void)
{
	return B12644_DECKUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deckui_ok_u_12644 - deckui ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Steam Deck UI
 * presence or call libc. No parent wires.
 */
uint32_t
gj_deckui_ok_u_12644(void)
{
	(void)NULL;
	return b12644_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deckui_ok_u_12644(void)
    __attribute__((alias("gj_deckui_ok_u_12644")));
