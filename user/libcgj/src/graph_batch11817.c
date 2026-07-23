/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11817: notification portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_notification_portal_ok_u_11817(void);
 *     - Always returns 0. Soft stub only; intentionally not a real notification portal readiness probe.
 *   uint32_t __gj_notification_portal_ok_u_11817  (alias)
 *   __libcgj_batch11817_marker = "libcgj-batch11817"
 *
 * Exclusive continuum CREATE-ONLY (11811-11820: flatpak/portal soft id
 * stubs — flatpak_ok_u_11811, xdg_portal_ok_u_11812,
 * pipewire_portal_ok_u_11813, camera_portal_ok_u_11814,
 * file_portal_ok_u_11815, secret_portal_ok_u_11816,
 * notification_portal_ok_u_11817, settings_portal_ok_u_11818,
 * portal_soft_ready_u_11819, batch_id_11820). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11817_marker[] = "libcgj-batch11817";

/* Soft notification-portal-ok lamp: always off (not a real portal probe). */
#define B11817_NOTIFICATION_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11817_notification_portal_ok(void)
{
	return B11817_NOTIFICATION_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_notification_portal_ok_u_11817 - notification portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not talk to notification portals or call libc. No parent wires.
 */
uint32_t
gj_notification_portal_ok_u_11817(void)
{
	(void)NULL;
	return b11817_notification_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_notification_portal_ok_u_11817(void)
    __attribute__((alias("gj_notification_portal_ok_u_11817")));
