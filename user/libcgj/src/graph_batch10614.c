/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10614: camera portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_camera_portal_ok_u_10614(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Camera portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_camera_portal_ok_u_10614  (alias)
 *   __libcgj_batch10614_marker = "libcgj-batch10614"
 *
 * Exclusive continuum CREATE-ONLY (10611-10620: flatpak/portal soft id
 * stubs — flatpak_ok_u_10611, xdg_portal_ok_u_10612,
 * pipewire_portal_ok_u_10613, camera_portal_ok_u_10614,
 * file_portal_ok_u_10615, secret_portal_ok_u_10616,
 * notification_portal_ok_u_10617, settings_portal_ok_u_10618,
 * portal_soft_ready_u_10619, batch_id_10620). Unique surface only; no
 * multi-def. Distinct from gj_camera_portal_ok_u_10114 (batch10114).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10614_marker[] = "libcgj-batch10614";

/* Soft camera-portal-ok lamp: always off (not a real portal probe). */
#define B10614_CAMERA_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10614_camera_portal_ok(void)
{
	return B10614_CAMERA_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_camera_portal_ok_u_10614 - camera portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Camera portal
 * or call libc. No parent wires.
 */
uint32_t
gj_camera_portal_ok_u_10614(void)
{
	(void)NULL;
	return b10614_camera_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_camera_portal_ok_u_10614(void)
    __attribute__((alias("gj_camera_portal_ok_u_10614")));
