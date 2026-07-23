/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12115: file portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_file_portal_ok_u_12115(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FileChooser portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_file_portal_ok_u_12115  (alias)
 *   __libcgj_batch12115_marker = "libcgj-batch12115"
 *
 * Exclusive continuum CREATE-ONLY (12111-12120: flatpak/portal soft id
 * stubs — flatpak_ok_u_12111, xdg_portal_ok_u_12112,
 * pipewire_portal_ok_u_12113, camera_portal_ok_u_12114,
 * file_portal_ok_u_12115, secret_portal_ok_u_12116,
 * notification_portal_ok_u_12117, settings_portal_ok_u_12118,
 * portal_soft_ready_u_12119, batch_id_12120). Unique surface only; no
 * multi-def. Distinct from gj_file_portal_ok_u_11815 (batch11815),
 * gj_file_portal_ok_u_11015 (batch11015), and gj_file_portal_ok_u_10815
 * (batch10815). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12115_marker[] = "libcgj-batch12115";

/* Soft file-portal-ok lamp: always off (not a real portal probe). */
#define B12115_FILE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12115_file_portal_ok(void)
{
return B12115_FILE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_file_portal_ok_u_12115 - file portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe FileChooser
 * portal or call libc. No parent wires.
 */
uint32_t
gj_file_portal_ok_u_12115(void)
{
(void)NULL;
return b12115_file_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_file_portal_ok_u_12115(void)
    __attribute__((alias("gj_file_portal_ok_u_12115")));
