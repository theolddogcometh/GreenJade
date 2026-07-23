/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12111: flatpak ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_ok_u_12111(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       flatpak runtime probe for the flatpak/portal continuum.
 *   uint32_t __gj_flatpak_ok_u_12111  (alias)
 *   __libcgj_batch12111_marker = "libcgj-batch12111"
 *
 * Exclusive continuum CREATE-ONLY (12111-12120: flatpak/portal soft id
 * stubs — flatpak_ok_u_12111, xdg_portal_ok_u_12112,
 * pipewire_portal_ok_u_12113, camera_portal_ok_u_12114,
 * file_portal_ok_u_12115, secret_portal_ok_u_12116,
 * notification_portal_ok_u_12117, settings_portal_ok_u_12118,
 * portal_soft_ready_u_12119, batch_id_12120). Unique surface only; no
 * multi-def. Distinct from gj_flatpak_ok_u_11811 (batch11811),
 * gj_flatpak_ok_u_11611 (batch11611), and gj_flatpak_ok_u_11411
 * (batch11411). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12111_marker[] = "libcgj-batch12111";

/* Soft flatpak-ok lamp: always off (not a real flatpak probe). */
#define B12111_FLATPAK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12111_flatpak_ok(void)
{
return B12111_FLATPAK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ok_u_12111 - flatpak ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe flatpak or
 * call libc. No parent wires.
 */
uint32_t
gj_flatpak_ok_u_12111(void)
{
(void)NULL;
return b12111_flatpak_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_ok_u_12111(void)
    __attribute__((alias("gj_flatpak_ok_u_12111")));
