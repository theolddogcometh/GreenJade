/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13917: notification portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_notification_portal_ok_u_13917(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Notification portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_notification_portal_ok_u_13917  (alias)
 *   __libcgj_batch13917_marker = "libcgj-batch13917"
 *
 * Exclusive continuum CREATE-ONLY (13911-13920: flatpak/portal soft id
 * stubs — flatpak_ok_u_13911, xdg_portal_ok_u_13912,
 * pipewire_portal_ok_u_13913, camera_portal_ok_u_13914,
 * file_portal_ok_u_13915, secret_portal_ok_u_13916,
 * notification_portal_ok_u_13917, settings_portal_ok_u_13918,
 * portal_soft_ready_u_13919, batch_id_13920). Unique surface only; no
 * multi-def. Distinct from gj_notification_portal_ok_u_10117
 * (batch10117). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13917_marker[] = "libcgj-batch13917";

/* Soft notification-portal-ok lamp: always off (not a real probe). */
#define B13917_NOTIFICATION_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13917_notification_portal_ok(void)
{
	return B13917_NOTIFICATION_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_notification_portal_ok_u_13917 - notification portal ok soft unit.
 *
 * Always returns 0. Soft continuum stub; does not probe Notification
 * portal or call libc. No parent wires.
 */
uint32_t
gj_notification_portal_ok_u_13917(void)
{
	(void)NULL;
	return b13917_notification_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_notification_portal_ok_u_13917(void)
    __attribute__((alias("gj_notification_portal_ok_u_13917")));
