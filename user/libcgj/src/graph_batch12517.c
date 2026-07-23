/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12517: notification portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_notification_portal_ok_u_12517(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Notification portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_notification_portal_ok_u_12517  (alias)
 *   __libcgj_batch12517_marker = "libcgj-batch12517"
 *
 * Exclusive continuum CREATE-ONLY (12511-12520: flatpak/portal soft id
 * stubs — flatpak_ok_u_12511, xdg_portal_ok_u_12512,
 * pipewire_portal_ok_u_12513, camera_portal_ok_u_12514,
 * file_portal_ok_u_12515, secret_portal_ok_u_12516,
 * notification_portal_ok_u_12517, settings_portal_ok_u_12518,
 * portal_soft_ready_u_12519, batch_id_12520). Unique surface only; no
 * multi-def. Distinct from gj_notification_portal_ok_u_12317
 * (batch12317), gj_notification_portal_ok_u_12117 (batch12117), and
 * gj_notification_portal_ok_u_12017 (batch12017). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12517_marker[] = "libcgj-batch12517";

/* Soft notification-portal-ok lamp: always off (not a real probe). */
#define B12517_NOTIFICATION_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12517_notification_portal_ok(void)
{
	return B12517_NOTIFICATION_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_notification_portal_ok_u_12517 - notification portal ok soft unit.
 *
 * Always returns 0. Soft continuum stub; does not probe Notification
 * portal or call libc. No parent wires.
 */
uint32_t
gj_notification_portal_ok_u_12517(void)
{
	(void)NULL;
	return b12517_notification_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_notification_portal_ok_u_12517(void)
    __attribute__((alias("gj_notification_portal_ok_u_12517")));
