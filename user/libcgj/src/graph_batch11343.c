/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11343: bigpicture ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_11343(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Big Picture mode probe for the gamescope/steamui soft continuum.
 *   uint32_t __gj_bigpicture_ok_u_11343  (alias)
 *   __libcgj_batch11343_marker = "libcgj-batch11343"
 *
 * Exclusive continuum CREATE-ONLY (11341-11350: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11341, steamui_ok_u_11342,
 * bigpicture_ok_u_11343, deckui_ok_u_11344, cec_ok_u_11345,
 * overlay_ok_u_11346, fps_limit_ok_u_11347, hdr_ok_u_11348,
 * gamescope_soft_ready_u_11349, batch_id_11350→11350). Unique surface
 * only; no multi-def. Distinct from gj_bigpicture_ok_u_11143 /
 * gj_bigpicture_ok_u_10943 / gj_bigpicture_ok_u_10043. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11343_marker[] = "libcgj-batch11343";

/* Soft bigpicture-ok lamp: always off (not a real runtime probe). */
#define B11343_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11343_bigpicture_ok(void)
{
	return B11343_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_11343 - bigpicture ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe bigpicture
 * mode or call libc. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_11343(void)
{
	(void)NULL;
	return b11343_bigpicture_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_11343(void)
    __attribute__((alias("gj_bigpicture_ok_u_11343")));
