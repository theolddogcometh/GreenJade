/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10615: file portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_file_portal_ok_u_10615(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FileChooser portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_file_portal_ok_u_10615  (alias)
 *   __libcgj_batch10615_marker = "libcgj-batch10615"
 *
 * Exclusive continuum CREATE-ONLY (10611-10620: flatpak/portal soft id
 * stubs — flatpak_ok_u_10611, xdg_portal_ok_u_10612,
 * pipewire_portal_ok_u_10613, camera_portal_ok_u_10614,
 * file_portal_ok_u_10615, secret_portal_ok_u_10616,
 * notification_portal_ok_u_10617, settings_portal_ok_u_10618,
 * portal_soft_ready_u_10619, batch_id_10620). Unique surface only; no
 * multi-def. Distinct from gj_file_portal_ok_u_10115 (batch10115). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10615_marker[] = "libcgj-batch10615";

/* Soft file-portal-ok lamp: always off (not a real portal probe). */
#define B10615_FILE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10615_file_portal_ok(void)
{
	return B10615_FILE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_file_portal_ok_u_10615 - file portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe FileChooser
 * portal or call libc. No parent wires.
 */
uint32_t
gj_file_portal_ok_u_10615(void)
{
	(void)NULL;
	return b10615_file_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_file_portal_ok_u_10615(void)
    __attribute__((alias("gj_file_portal_ok_u_10615")));
