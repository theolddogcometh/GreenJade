/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10115: file portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_file_portal_ok_u_10115(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FileChooser portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_file_portal_ok_u_10115  (alias)
 *   __libcgj_batch10115_marker = "libcgj-batch10115"
 *
 * Exclusive continuum CREATE-ONLY (10111-10120: flatpak/portal soft id
 * stubs — flatpak_ok_u_10111, xdg_portal_ok_u_10112,
 * pipewire_portal_ok_u_10113, camera_portal_ok_u_10114,
 * file_portal_ok_u_10115, secret_portal_ok_u_10116,
 * notification_portal_ok_u_10117, settings_portal_ok_u_10118,
 * portal_soft_ready_u_10119, batch_id_10120). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10115_marker[] = "libcgj-batch10115";

/* Soft file-portal-ok lamp: always off (not a real portal probe). */
#define B10115_FILE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10115_file_portal_ok(void)
{
	return B10115_FILE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_file_portal_ok_u_10115 - file portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe FileChooser
 * portal or call libc. No parent wires.
 */
uint32_t
gj_file_portal_ok_u_10115(void)
{
	(void)NULL;
	return b10115_file_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_file_portal_ok_u_10115(void)
    __attribute__((alias("gj_file_portal_ok_u_10115")));
