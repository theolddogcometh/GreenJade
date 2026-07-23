/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12915: file portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_file_portal_ok_u_12915(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FileChooser portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_file_portal_ok_u_12915  (alias)
 *   __libcgj_batch12915_marker = "libcgj-batch12915"
 *
 * Exclusive continuum CREATE-ONLY (12911-12920: flatpak/portal soft id
 * stubs — flatpak_ok_u_12911, xdg_portal_ok_u_12912,
 * pipewire_portal_ok_u_12913, camera_portal_ok_u_12914,
 * file_portal_ok_u_12915, secret_portal_ok_u_12916,
 * notification_portal_ok_u_12917, settings_portal_ok_u_12918,
 * portal_soft_ready_u_12919, batch_id_12920). Unique surface only; no
 * multi-def. Distinct from gj_file_portal_ok_u_12715 (batch12715),
 * gj_file_portal_ok_u_12515 (batch12515), gj_file_portal_ok_u_12315
 * (batch12315), and gj_file_portal_ok_u_12115 (batch12115). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12915_marker[] = "libcgj-batch12915";

/* Soft file-portal-ok lamp: always off (not a real FileChooser probe). */
#define B12915_FILE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12915_file_portal_ok(void)
{
	return B12915_FILE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_file_portal_ok_u_12915 - file portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open FileChooser
 * portal or call libc. No parent wires.
 */
uint32_t
gj_file_portal_ok_u_12915(void)
{
	(void)NULL;
	return b12915_file_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_file_portal_ok_u_12915(void)
    __attribute__((alias("gj_file_portal_ok_u_12915")));
