/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13115: file portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_file_portal_ok_u_13115(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FileChooser portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_file_portal_ok_u_13115  (alias)
 *   __libcgj_batch13115_marker = "libcgj-batch13115"
 *
 * Exclusive continuum CREATE-ONLY (13111-13120: flatpak/portal soft id
 * stubs — flatpak_ok_u_13111, xdg_portal_ok_u_13112,
 * pipewire_portal_ok_u_13113, camera_portal_ok_u_13114,
 * file_portal_ok_u_13115, secret_portal_ok_u_13116,
 * notification_portal_ok_u_13117, settings_portal_ok_u_13118,
 * portal_soft_ready_u_13119, batch_id_13120). Unique surface only; no
 * multi-def. Distinct from gj_file_portal_ok_u_12915 (batch12915),
 * gj_file_portal_ok_u_12715 (batch12715), gj_file_portal_ok_u_12515
 * (batch12515), and gj_file_portal_ok_u_12315
 * (batch12315). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13115_marker[] = "libcgj-batch13115";

/* Soft file-portal-ok lamp: always off (not a real FileChooser probe). */
#define B13115_FILE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13115_file_portal_ok(void)
{
	return B13115_FILE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_file_portal_ok_u_13115 - file portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open FileChooser
 * portal or call libc. No parent wires.
 */
uint32_t
gj_file_portal_ok_u_13115(void)
{
	(void)NULL;
	return b13115_file_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_file_portal_ok_u_13115(void)
    __attribute__((alias("gj_file_portal_ok_u_13115")));
