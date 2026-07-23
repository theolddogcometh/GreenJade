/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12117: notification portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_notification_portal_ok_u_12117(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Notification portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_notification_portal_ok_u_12117  (alias)
 *   __libcgj_batch12117_marker = "libcgj-batch12117"
 *
 * Exclusive continuum CREATE-ONLY (12111-12120: flatpak/portal soft id
 * stubs — flatpak_ok_u_12111, xdg_portal_ok_u_12112,
 * pipewire_portal_ok_u_12113, camera_portal_ok_u_12114,
 * file_portal_ok_u_12115, secret_portal_ok_u_12116,
 * notification_portal_ok_u_12117, settings_portal_ok_u_12118,
 * portal_soft_ready_u_12119, batch_id_12120). Unique surface only; no
 * multi-def. Distinct from gj_notification_portal_ok_u_11817
 * (batch11817), gj_notification_portal_ok_u_11017 (batch11017), and
 * gj_notification_portal_ok_u_10817 (batch10817). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12117_marker[] = "libcgj-batch12117";

/* Soft notification-portal-ok lamp: always off (not a real probe). */
#define B12117_NOTIFICATION_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12117_notification_portal_ok(void)
{
return B12117_NOTIFICATION_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_notification_portal_ok_u_12117 - notification portal ok soft unit.
 *
 * Always returns 0. Soft continuum stub; does not probe Notification
 * portal or call libc. No parent wires.
 */
uint32_t
gj_notification_portal_ok_u_12117(void)
{
(void)NULL;
return b12117_notification_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_notification_portal_ok_u_12117(void)
    __attribute__((alias("gj_notification_portal_ok_u_12117")));
