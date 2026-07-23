/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11943: bigpicture ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_11943(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Big Picture mode probe for the bar3-related continuum.
 *   uint32_t __gj_bigpicture_ok_u_11943  (alias)
 *   __libcgj_batch11943_marker = "libcgj-batch11943"
 *
 * Exclusive continuum CREATE-ONLY (11941-11950: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11941, steamui_ok_u_11942,
 * bigpicture_ok_u_11943, deckui_ok_u_11944, cec_ok_u_11945,
 * overlay_ok_u_11946, fps_limit_ok_u_11947, hdr_ok_u_11948,
 * gamescope_soft_ready_u_11949, batch_id_11950). Unique surface only;
 * no multi-def. Distinct from gj_*_1004x wave (10041-10050). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11943_marker[] = "libcgj-batch11943";

/* Soft bigpicture-ok lamp: always off (not a real mode probe). */
#define B11943_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11943_ok(void)
{
	return B11943_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_11943 - bigpicture ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Big Picture
 * mode or call libc. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_11943(void)
{
	(void)NULL;
	return b11943_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_11943(void)
    __attribute__((alias("gj_bigpicture_ok_u_11943")));
