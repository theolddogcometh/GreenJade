/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13045: cec ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cec_ok_u_13045(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDMI-CEC runtime probe for the gamescope soft continuum.
 *   uint32_t __gj_cec_ok_u_13045  (alias)
 *   __libcgj_batch13045_marker = "libcgj-batch13045"
 *
 * Exclusive continuum CREATE-ONLY (13041-13050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13041, steamui_ok_u_13042,
 * bigpicture_ok_u_13043, deckui_ok_u_13044, cec_ok_u_13045,
 * overlay_ok_u_13046, fps_limit_ok_u_13047, hdr_ok_u_13048,
 * gamescope_soft_ready_u_13049, batch_id_13050→13050). Unique surface
 * only; no multi-def. Distinct from gj_cec_ok_u_12845 /
 * gj_cec_ok_u_12645 / gj_cec_ok_u_12445. No parent wires. No __int128.
 * No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13045_marker[] = "libcgj-batch13045";

/* Soft cec-ok lamp: always off (not a real CEC probe). */
#define B13045_CEC_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13045_ok(void)
{
	return B13045_CEC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cec_ok_u_13045 - cec ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDMI-CEC
 * devices or call libc. No parent wires.
 */
uint32_t
gj_cec_ok_u_13045(void)
{
	(void)NULL;
	return b13045_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cec_ok_u_13045(void)
    __attribute__((alias("gj_cec_ok_u_13045")));
