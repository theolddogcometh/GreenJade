/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13314: FileChooser portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_file_chooser_ok_u_13314(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FileChooser portal probe for the flatpak portal continuum.
 *   uint32_t __gj_file_chooser_ok_u_13314  (alias)
 *   __libcgj_batch13314_marker = "libcgj-batch13314"
 *
 * Exclusive continuum CREATE-ONLY (13311-13320: flatpak portal soft id
 * stubs — flatpak_ok_u_13311, xdg_portal_ok_u_13312,
 * documents_ok_u_13313, file_chooser_ok_u_13314, open_uri_ok_u_13315,
 * screenshot_ok_u_13316, notification_ok_u_13317, secrets_ok_u_13318,
 * portal_soft_ready_u_13319, batch_id_13320).
 * Unique surface only; no multi-def. Distinct from sibling flatpak
 * portal soft stubs in this wave. No parent wires. No __int128. No
 * FileChooser portal implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13314_marker[] = "libcgj-batch13314";

/* Soft FileChooser-ok lamp: always off (not a real chooser probe). */
#define B13314_FILE_CHOOSER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13314_file_chooser_ok(void)
{
	return B13314_FILE_CHOOSER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_file_chooser_ok_u_13314 - FileChooser portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect the
 * FileChooser portal or call libc. No parent wires.
 */
uint32_t
gj_file_chooser_ok_u_13314(void)
{
	(void)NULL;
	return b13314_file_chooser_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_file_chooser_ok_u_13314(void)
    __attribute__((alias("gj_file_chooser_ok_u_13314")));
