/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13114: camera portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_camera_portal_ok_u_13114(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Camera portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_camera_portal_ok_u_13114  (alias)
 *   __libcgj_batch13114_marker = "libcgj-batch13114"
 *
 * Exclusive continuum CREATE-ONLY (13111-13120: flatpak/portal soft id
 * stubs — flatpak_ok_u_13111, xdg_portal_ok_u_13112,
 * pipewire_portal_ok_u_13113, camera_portal_ok_u_13114,
 * file_portal_ok_u_13115, secret_portal_ok_u_13116,
 * notification_portal_ok_u_13117, settings_portal_ok_u_13118,
 * portal_soft_ready_u_13119, batch_id_13120). Unique surface only; no
 * multi-def. Distinct from gj_camera_portal_ok_u_12914 (batch12914),
 * gj_camera_portal_ok_u_12714 (batch12714), gj_camera_portal_ok_u_12514
 * (batch12514), and gj_camera_portal_ok_u_12314
 * (batch12314). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13114_marker[] = "libcgj-batch13114";

/* Soft camera-portal-ok lamp: always off (not a real camera portal probe). */
#define B13114_CAMERA_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13114_camera_portal_ok(void)
{
	return B13114_CAMERA_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_camera_portal_ok_u_13114 - camera portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open Camera portal
 * or call libc. No parent wires.
 */
uint32_t
gj_camera_portal_ok_u_13114(void)
{
	(void)NULL;
	return b13114_camera_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_camera_portal_ok_u_13114(void)
    __attribute__((alias("gj_camera_portal_ok_u_13114")));
