/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11748: hdr ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_ok_u_11748(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDR display/runtime probe for the gamescope/steamui soft
 *       continuum.
 *   uint32_t __gj_hdr_ok_u_11748  (alias)
 *   __libcgj_batch11748_marker = "libcgj-batch11748"
 *
 * Exclusive continuum CREATE-ONLY (11741-11750: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11741, steamui_ok_u_11742,
 * bigpicture_ok_u_11743, deckui_ok_u_11744, cec_ok_u_11745,
 * overlay_ok_u_11746, fps_limit_ok_u_11747, hdr_ok_u_11748,
 * gamescope_soft_ready_u_11749, batch_id_11750→11750). Unique surface
 * only; no multi-def. Distinct from gj_hdr_ok_u_11548 /
 * gj_hdr_ok_u_11348 / gj_hdr_ok_u_11148. No parent wires. No __int128.
 * No gamescope/steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11748_marker[] = "libcgj-batch11748";

/* Soft hdr-ok lamp: always off (not a real runtime probe). */
#define B11748_HDR_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11748_hdr_ok(void)
{
	return B11748_HDR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_ok_u_11748 - hdr ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe hdr display
 * capability or call libc. No parent wires.
 */
uint32_t
gj_hdr_ok_u_11748(void)
{
	(void)NULL;
	return b11748_hdr_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_ok_u_11748(void)
    __attribute__((alias("gj_hdr_ok_u_11748")));
