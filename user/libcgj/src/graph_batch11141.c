/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11141: gamescope ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_ok_u_11141(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamescope runtime probe for the gamescope/steamui soft continuum.
 *   uint32_t __gj_gamescope_ok_u_11141  (alias)
 *   __libcgj_batch11141_marker = "libcgj-batch11141"
 *
 * Exclusive continuum CREATE-ONLY (11141-11150: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11141, steamui_ok_u_11142,
 * bigpicture_ok_u_11143, deckui_ok_u_11144, cec_ok_u_11145,
 * overlay_ok_u_11146, fps_limit_ok_u_11147, hdr_ok_u_11148,
 * gamescope_soft_ready_u_11149, batch_id_11150). Unique surface only;
 * no multi-def. Distinct from gj_gamescope_ok_u_10941 / gj_gamescope_ok_u_10741 / gj_gamescope_ok_u_10041. No parent wires. No __int128.
 * No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11141_marker[] = "libcgj-batch11141";

/* Soft gamescope-ok lamp: always off (not a real runtime probe). */
#define B11141_GAMESCOPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11141_gamescope_ok(void)
{
	return B11141_GAMESCOPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_ok_u_11141 - gamescope ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe gamescope
 * presence or call libc. No parent wires.
 */
uint32_t
gj_gamescope_ok_u_11141(void)
{
	(void)NULL;
	return b11141_gamescope_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_ok_u_11141(void)
    __attribute__((alias("gj_gamescope_ok_u_11141")));
