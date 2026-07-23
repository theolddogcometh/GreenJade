/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10345: cec ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cec_ok_u_10345(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDMI-CEC runtime probe for the bar3-related continuum.
 *   uint32_t __gj_cec_ok_u_10345  (alias)
 *   __libcgj_batch10345_marker = "libcgj-batch10345"
 *
 * Exclusive continuum CREATE-ONLY (10341-10350: gamescope/steamui soft
 * id stubs — gamescope_ok_u_10341, steamui_ok_u_10342,
 * bigpicture_ok_u_10343, deckui_ok_u_10344, cec_ok_u_10345,
 * overlay_ok_u_10346, fps_limit_ok_u_10347, hdr_ok_u_10348,
 * gamescope_soft_ready_u_10349, batch_id_10350). Unique surface only;
 * no multi-def. Distinct from gj_cec_ok_u_10045 (batch10045).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10345_marker[] = "libcgj-batch10345";

/* Soft cec-ok lamp: always off (not a real CEC probe). */
#define B10345_CEC_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10345_ok(void)
{
	return B10345_CEC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cec_ok_u_10345 - cec ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDMI-CEC
 * devices or call libc. No parent wires.
 */
uint32_t
gj_cec_ok_u_10345(void)
{
	(void)NULL;
	return b10345_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cec_ok_u_10345(void)
    __attribute__((alias("gj_cec_ok_u_10345")));
