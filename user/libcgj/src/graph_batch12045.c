/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12045: cec ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cec_ok_u_12045(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDMI-CEC runtime probe for the gamescope soft continuum.
 *   uint32_t __gj_cec_ok_u_12045  (alias)
 *   __libcgj_batch12045_marker = "libcgj-batch12045"
 *
 * Exclusive continuum CREATE-ONLY (12041-12050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12041, steamui_ok_u_12042,
 * bigpicture_ok_u_12043, deckui_ok_u_12044, cec_ok_u_12045,
 * overlay_ok_u_12046, fps_limit_ok_u_12047, hdr_ok_u_12048,
 * gamescope_soft_ready_u_12049, batch_id_12050). Unique surface only;
 * no multi-def. Distinct from gj_cec_ok_u_11945 (batch11945),
 * gj_cec_ok_u_11745, and prior *045 cec soft waves. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12045_marker[] = "libcgj-batch12045";

/* Soft cec-ok lamp: always off (not a real CEC probe). */
#define B12045_CEC_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12045_ok(void)
{
	return B12045_CEC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cec_ok_u_12045 - cec ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDMI-CEC
 * devices or call libc. No parent wires.
 */
uint32_t
gj_cec_ok_u_12045(void)
{
	(void)NULL;
	return b12045_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cec_ok_u_12045(void)
    __attribute__((alias("gj_cec_ok_u_12045")));
