/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12046: overlay ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_ok_u_12046(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamescope/steam overlay probe for the gamescope soft continuum.
 *   uint32_t __gj_overlay_ok_u_12046  (alias)
 *   __libcgj_batch12046_marker = "libcgj-batch12046"
 *
 * Exclusive continuum CREATE-ONLY (12041-12050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12041, steamui_ok_u_12042,
 * bigpicture_ok_u_12043, deckui_ok_u_12044, cec_ok_u_12045,
 * overlay_ok_u_12046, fps_limit_ok_u_12047, hdr_ok_u_12048,
 * gamescope_soft_ready_u_12049, batch_id_12050). Unique surface only;
 * no multi-def. Distinct from gj_overlay_ok_u_11946 (batch11946),
 * gj_overlay_ok_u_11746, and prior *046 overlay soft waves. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12046_marker[] = "libcgj-batch12046";

/* Soft overlay-ok lamp: always off (not a real overlay probe). */
#define B12046_OVERLAY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12046_ok(void)
{
	return B12046_OVERLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_ok_u_12046 - overlay ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe overlay
 * presence or call libc. No parent wires.
 */
uint32_t
gj_overlay_ok_u_12046(void)
{
	(void)NULL;
	return b12046_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_ok_u_12046(void)
    __attribute__((alias("gj_overlay_ok_u_12046")));
