/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12243: bigpicture ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_12243(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Big Picture mode probe for the bar3-related continuum.
 *   uint32_t __gj_bigpicture_ok_u_12243  (alias)
 *   __libcgj_batch12243_marker = "libcgj-batch12243"
 *
 * Exclusive continuum CREATE-ONLY (12241-12250: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12241, steamui_ok_u_12242,
 * bigpicture_ok_u_12243, deckui_ok_u_12244, cec_ok_u_12245,
 * overlay_ok_u_12246, fps_limit_ok_u_12247, hdr_ok_u_12248,
 * gamescope_soft_ready_u_12249, batch_id_12250→12250). Unique surface
 * only; no multi-def. Distinct from gj_bigpicture_ok_u_11943 /
 * gj_bigpicture_ok_u_11743 / gj_bigpicture_ok_u_11543. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12243_marker[] = "libcgj-batch12243";

/* Soft bigpicture-ok lamp: always off (not a real mode probe). */
#define B12243_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12243_ok(void)
{
	return B12243_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_12243 - bigpicture ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Big Picture
 * mode or call libc. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_12243(void)
{
	(void)NULL;
	return b12243_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_12243(void)
    __attribute__((alias("gj_bigpicture_ok_u_12243")));
