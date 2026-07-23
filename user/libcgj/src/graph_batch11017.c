/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11017: notification portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_notification_portal_ok_u_11017(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Notification portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_notification_portal_ok_u_11017  (alias)
 *   __libcgj_batch11017_marker = "libcgj-batch11017"
 *
 * Exclusive continuum CREATE-ONLY (11011-11020: flatpak/portal soft
 * ok slots all→0; portal_soft_ready→1; batch_id→11020 —
 * flatpak_ok_u_11011, xdg_portal_ok_u_11012,
 * pipewire_portal_ok_u_11013, camera_portal_ok_u_11014,
 * file_portal_ok_u_11015, secret_portal_ok_u_11016,
 * notification_portal_ok_u_11017, settings_portal_ok_u_11018,
 * portal_soft_ready_u_11019, batch_id_11020). Unique surface only; no
 * multi-def. Distinct from gj_notification_portal_ok_u_10817
 * (batch10817). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11017_marker[] = "libcgj-batch11017";

/* Soft notification-portal-ok lamp: always off (not a real probe). */
#define B11017_NOTIFICATION_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11017_notification_portal_ok(void)
{
	return B11017_NOTIFICATION_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_notification_portal_ok_u_11017 - notification portal ok soft unit.
 *
 * Always returns 0. Soft continuum stub; does not probe Notification
 * portal or call libc. No parent wires.
 */
uint32_t
gj_notification_portal_ok_u_11017(void)
{
	(void)NULL;
	return b11017_notification_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_notification_portal_ok_u_11017(void)
    __attribute__((alias("gj_notification_portal_ok_u_11017")));
