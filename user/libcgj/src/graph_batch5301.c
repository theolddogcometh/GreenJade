/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5301: conventional 60 Hz display refresh rate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_refresh_60_u(void);
 *     - Return the conventional 60 Hz display refresh rate (soft tag).
 *   uint32_t __gj_mode_refresh_60_u  (alias)
 *   __libcgj_batch5301_marker = "libcgj-batch5301"
 *
 * Exclusive continuum CREATE-ONLY (5301-5310: display mode unique —
 * mode_refresh_60_u, mode_refresh_144_u, mode_is_hd_u, mode_is_fhd_u,
 * mode_is_qhd_u, mode_is_uhd_u, mode_pixels_u, mode_aspect_num_u,
 * mode_aspect_den_u, batch_id_5310). Unique gj_mode_refresh_60_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5301_marker[] = "libcgj-batch5301";

/* Conventional 60 Hz progressive display refresh (Hz). */
#define B5301_REFRESH_60  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5301_refresh_60(void)
{
	return B5301_REFRESH_60;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_refresh_60_u - conventional 60 Hz display refresh rate.
 *
 * Always returns 60. Soft continuum constant for mode tables; does not
 * program hardware. Self-contained; no parent wires.
 */
uint32_t
gj_mode_refresh_60_u(void)
{
	(void)NULL;
	return b5301_refresh_60();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_refresh_60_u(void)
    __attribute__((alias("gj_mode_refresh_60_u")));
