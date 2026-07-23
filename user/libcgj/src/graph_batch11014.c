/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11014: camera portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_camera_portal_ok_u_11014(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Camera portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_camera_portal_ok_u_11014  (alias)
 *   __libcgj_batch11014_marker = "libcgj-batch11014"
 *
 * Exclusive continuum CREATE-ONLY (11011-11020: flatpak/portal soft
 * ok slots all→0; portal_soft_ready→1; batch_id→11020 —
 * flatpak_ok_u_11011, xdg_portal_ok_u_11012,
 * pipewire_portal_ok_u_11013, camera_portal_ok_u_11014,
 * file_portal_ok_u_11015, secret_portal_ok_u_11016,
 * notification_portal_ok_u_11017, settings_portal_ok_u_11018,
 * portal_soft_ready_u_11019, batch_id_11020). Unique surface only; no
 * multi-def. Distinct from gj_camera_portal_ok_u_10814 (batch10814).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11014_marker[] = "libcgj-batch11014";

/* Soft camera-portal-ok lamp: always off (not a real portal probe). */
#define B11014_CAMERA_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11014_camera_portal_ok(void)
{
	return B11014_CAMERA_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_camera_portal_ok_u_11014 - camera portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Camera portal
 * or call libc. No parent wires.
 */
uint32_t
gj_camera_portal_ok_u_11014(void)
{
	(void)NULL;
	return b11014_camera_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_camera_portal_ok_u_11014(void)
    __attribute__((alias("gj_camera_portal_ok_u_11014")));
