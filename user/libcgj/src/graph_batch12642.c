/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12642: steamui ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamui_ok_u_12642(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam UI runtime probe for the gamescope soft continuum.
 *   uint32_t __gj_steamui_ok_u_12642  (alias)
 *   __libcgj_batch12642_marker = "libcgj-batch12642"
 *
 * Exclusive continuum CREATE-ONLY (12641-12650: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12641, steamui_ok_u_12642,
 * bigpicture_ok_u_12643, deckui_ok_u_12644, cec_ok_u_12645,
 * overlay_ok_u_12646, fps_limit_ok_u_12647, hdr_ok_u_12648,
 * gamescope_soft_ready_u_12649, batch_id_12650→12650). Unique surface
 * only; no multi-def. Distinct from gj_steamui_ok_u_12442 /
 * gj_steamui_ok_u_12242 / gj_steamui_ok_u_12042. No parent wires.
 * No __int128. No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12642_marker[] = "libcgj-batch12642";

/* Soft steamui-ok lamp: always off (not a real UI probe). */
#define B12642_STEAMUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12642_ok(void)
{
	return B12642_STEAMUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamui_ok_u_12642 - steamui ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Steam UI
 * presence or call libc. No parent wires.
 */
uint32_t
gj_steamui_ok_u_12642(void)
{
	(void)NULL;
	return b12642_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamui_ok_u_12642(void)
    __attribute__((alias("gj_steamui_ok_u_12642")));
