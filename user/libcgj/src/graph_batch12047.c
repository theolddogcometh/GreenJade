/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12047: fps_limit ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fps_limit_ok_u_12047(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FPS limiter configuration probe for the gamescope soft continuum.
 *   uint32_t __gj_fps_limit_ok_u_12047  (alias)
 *   __libcgj_batch12047_marker = "libcgj-batch12047"
 *
 * Exclusive continuum CREATE-ONLY (12041-12050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12041, steamui_ok_u_12042,
 * bigpicture_ok_u_12043, deckui_ok_u_12044, cec_ok_u_12045,
 * overlay_ok_u_12046, fps_limit_ok_u_12047, hdr_ok_u_12048,
 * gamescope_soft_ready_u_12049, batch_id_12050). Unique surface only;
 * no multi-def. Distinct from gj_fps_limit_ok_u_11947 (batch11947),
 * gj_fps_limit_ok_u_11747, and prior *047 fps_limit soft waves. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12047_marker[] = "libcgj-batch12047";

/* Soft fps-limit-ok lamp: always off (not a real limiter probe). */
#define B12047_FPS_LIMIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12047_ok(void)
{
	return B12047_FPS_LIMIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_limit_ok_u_12047 - fps_limit ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe FPS limiter
 * configuration or call libc. No parent wires.
 */
uint32_t
gj_fps_limit_ok_u_12047(void)
{
	(void)NULL;
	return b12047_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fps_limit_ok_u_12047(void)
    __attribute__((alias("gj_fps_limit_ok_u_12047")));
