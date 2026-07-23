/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12244: deckui ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deckui_ok_u_12244(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Deck UI runtime probe for the bar3-related continuum.
 *   uint32_t __gj_deckui_ok_u_12244  (alias)
 *   __libcgj_batch12244_marker = "libcgj-batch12244"
 *
 * Exclusive continuum CREATE-ONLY (12241-12250: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12241, steamui_ok_u_12242,
 * bigpicture_ok_u_12243, deckui_ok_u_12244, cec_ok_u_12245,
 * overlay_ok_u_12246, fps_limit_ok_u_12247, hdr_ok_u_12248,
 * gamescope_soft_ready_u_12249, batch_id_12250→12250). Unique surface
 * only; no multi-def. Distinct from gj_deckui_ok_u_11944 /
 * gj_deckui_ok_u_11744 / gj_deckui_ok_u_11544. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12244_marker[] = "libcgj-batch12244";

/* Soft deckui-ok lamp: always off (not a real Deck UI probe). */
#define B12244_DECKUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12244_ok(void)
{
	return B12244_DECKUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deckui_ok_u_12244 - deckui ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Steam Deck UI
 * presence or call libc. No parent wires.
 */
uint32_t
gj_deckui_ok_u_12244(void)
{
	(void)NULL;
	return b12244_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deckui_ok_u_12244(void)
    __attribute__((alias("gj_deckui_ok_u_12244")));
