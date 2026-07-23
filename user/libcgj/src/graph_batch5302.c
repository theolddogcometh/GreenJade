/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5302: conventional 144 Hz display refresh rate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_refresh_144_u(void);
 *     - Return the conventional 144 Hz gaming display refresh rate.
 *   uint32_t __gj_mode_refresh_144_u  (alias)
 *   __libcgj_batch5302_marker = "libcgj-batch5302"
 *
 * Exclusive continuum CREATE-ONLY (5301-5310: display mode unique —
 * mode_refresh_60_u, mode_refresh_144_u, mode_is_hd_u, mode_is_fhd_u,
 * mode_is_qhd_u, mode_is_uhd_u, mode_pixels_u, mode_aspect_num_u,
 * mode_aspect_den_u, batch_id_5310). Unique gj_mode_refresh_144_u
 * surface only; no multi-def. Distinct from gj_mode_refresh_60_u
 * (batch5301). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5302_marker[] = "libcgj-batch5302";

/* Conventional 144 Hz progressive display refresh (Hz). */
#define B5302_REFRESH_144  144u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5302_refresh_144(void)
{
	return B5302_REFRESH_144;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_refresh_144_u - conventional 144 Hz display refresh rate.
 *
 * Always returns 144. Soft continuum constant for high-refresh mode
 * tables; does not program hardware. Self-contained; no parent wires.
 */
uint32_t
gj_mode_refresh_144_u(void)
{
	(void)NULL;
	return b5302_refresh_144();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_refresh_144_u(void)
    __attribute__((alias("gj_mode_refresh_144_u")));
