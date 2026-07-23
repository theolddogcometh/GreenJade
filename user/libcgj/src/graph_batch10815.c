/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10815: file portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_file_portal_ok_u_10815(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FileChooser portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_file_portal_ok_u_10815  (alias)
 *   __libcgj_batch10815_marker = "libcgj-batch10815"
 *
 * Exclusive continuum CREATE-ONLY (10811-10820: flatpak/portal soft
 * ok slots all→0; portal_soft_ready→1; batch_id→10820 —
 * flatpak_ok_u_10811, xdg_portal_ok_u_10812,
 * pipewire_portal_ok_u_10813, camera_portal_ok_u_10814,
 * file_portal_ok_u_10815, secret_portal_ok_u_10816,
 * notification_portal_ok_u_10817, settings_portal_ok_u_10818,
 * portal_soft_ready_u_10819, batch_id_10820). Unique surface only; no
 * multi-def. Distinct from gj_file_portal_ok_u_10115 (batch10115). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10815_marker[] = "libcgj-batch10815";

/* Soft file-portal-ok lamp: always off (not a real portal probe). */
#define B10815_FILE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10815_file_portal_ok(void)
{
	return B10815_FILE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_file_portal_ok_u_10815 - file portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe FileChooser
 * portal or call libc. No parent wires.
 */
uint32_t
gj_file_portal_ok_u_10815(void)
{
	(void)NULL;
	return b10815_file_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_file_portal_ok_u_10815(void)
    __attribute__((alias("gj_file_portal_ok_u_10815")));
