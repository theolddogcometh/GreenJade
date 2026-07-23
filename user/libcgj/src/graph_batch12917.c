/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12917: notification portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_notification_portal_ok_u_12917(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Notification portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_notification_portal_ok_u_12917  (alias)
 *   __libcgj_batch12917_marker = "libcgj-batch12917"
 *
 * Exclusive continuum CREATE-ONLY (12911-12920: flatpak/portal soft id
 * stubs — flatpak_ok_u_12911, xdg_portal_ok_u_12912,
 * pipewire_portal_ok_u_12913, camera_portal_ok_u_12914,
 * file_portal_ok_u_12915, secret_portal_ok_u_12916,
 * notification_portal_ok_u_12917, settings_portal_ok_u_12918,
 * portal_soft_ready_u_12919, batch_id_12920). Unique surface only; no
 * multi-def. Distinct from gj_notification_portal_ok_u_12717
 * (batch12717), gj_notification_portal_ok_u_12517 (batch12517),
 * gj_notification_portal_ok_u_12317 (batch12317), and
 * gj_notification_portal_ok_u_12117 (batch12117). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12917_marker[] = "libcgj-batch12917";

/* Soft notification-portal-ok lamp: always off (not a real notify probe). */
#define B12917_NOTIFICATION_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12917_notification_portal_ok(void)
{
	return B12917_NOTIFICATION_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_notification_portal_ok_u_12917 - notification portal ok soft unit.
 *
 * Always returns 0. Soft continuum stub; does not open Notification
 * portal or call libc. No parent wires.
 */
uint32_t
gj_notification_portal_ok_u_12917(void)
{
	(void)NULL;
	return b12917_notification_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_notification_portal_ok_u_12917(void)
    __attribute__((alias("gj_notification_portal_ok_u_12917")));
