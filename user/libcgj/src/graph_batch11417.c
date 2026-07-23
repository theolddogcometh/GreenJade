/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11417: notification portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_notification_ok_u_11417(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Notification portal probe for the flatpak portal soft continuum.
 *   uint32_t __gj_notification_ok_u_11417  (alias)
 *   __libcgj_batch11417_marker = "libcgj-batch11417"
 *
 * Exclusive continuum CREATE-ONLY (11411-11420: flatpak portal soft —
 * flatpak_ok_u_11411, portal_ok_u_11412, document_ok_u_11413,
 * filechooser_ok_u_11414, openuri_ok_u_11415, network_ok_u_11416,
 * notification_ok_u_11417, screencast_ok_u_11418,
 * portal_soft_ready_u_11419, batch_id_11420). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11417_marker[] = "libcgj-batch11417";

/* Soft notification-ok lamp: always off (not a real Notification probe). */
#define B11417_NOTIFICATION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11417_notification_ok(void)
{
	return B11417_NOTIFICATION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_notification_ok_u_11417 - notification portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Notification
 * portal or call libc. No parent wires.
 */
uint32_t
gj_notification_ok_u_11417(void)
{
	(void)NULL;
	return b11417_notification_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_notification_ok_u_11417(void)
    __attribute__((alias("gj_notification_ok_u_11417")));
