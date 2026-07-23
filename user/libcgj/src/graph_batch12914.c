/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12914: camera portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_camera_portal_ok_u_12914(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Camera portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_camera_portal_ok_u_12914  (alias)
 *   __libcgj_batch12914_marker = "libcgj-batch12914"
 *
 * Exclusive continuum CREATE-ONLY (12911-12920: flatpak/portal soft id
 * stubs — flatpak_ok_u_12911, xdg_portal_ok_u_12912,
 * pipewire_portal_ok_u_12913, camera_portal_ok_u_12914,
 * file_portal_ok_u_12915, secret_portal_ok_u_12916,
 * notification_portal_ok_u_12917, settings_portal_ok_u_12918,
 * portal_soft_ready_u_12919, batch_id_12920). Unique surface only; no
 * multi-def. Distinct from gj_camera_portal_ok_u_12714 (batch12714),
 * gj_camera_portal_ok_u_12514 (batch12514),
 * gj_camera_portal_ok_u_12314 (batch12314), and
 * gj_camera_portal_ok_u_12114 (batch12114). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12914_marker[] = "libcgj-batch12914";

/* Soft camera-portal-ok lamp: always off (not a real camera portal probe). */
#define B12914_CAMERA_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12914_camera_portal_ok(void)
{
	return B12914_CAMERA_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_camera_portal_ok_u_12914 - camera portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open Camera portal
 * or call libc. No parent wires.
 */
uint32_t
gj_camera_portal_ok_u_12914(void)
{
	(void)NULL;
	return b12914_camera_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_camera_portal_ok_u_12914(void)
    __attribute__((alias("gj_camera_portal_ok_u_12914")));
