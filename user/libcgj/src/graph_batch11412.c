/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11412: portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_portal_ok_u_11412(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       xdg-desktop-portal probe for the flatpak portal soft continuum.
 *   uint32_t __gj_portal_ok_u_11412  (alias)
 *   __libcgj_batch11412_marker = "libcgj-batch11412"
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

const char __libcgj_batch11412_marker[] = "libcgj-batch11412";

/* Soft portal-ok lamp: always off (not a real portal probe). */
#define B11412_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11412_portal_ok(void)
{
	return B11412_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_portal_ok_u_11412 - portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe
 * xdg-desktop-portal or call libc. No parent wires.
 */
uint32_t
gj_portal_ok_u_11412(void)
{
	(void)NULL;
	return b11412_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_portal_ok_u_11412(void)
    __attribute__((alias("gj_portal_ok_u_11412")));
