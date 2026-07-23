/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12112: xdg portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xdg_portal_ok_u_12112(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       XDG desktop portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_xdg_portal_ok_u_12112  (alias)
 *   __libcgj_batch12112_marker = "libcgj-batch12112"
 *
 * Exclusive continuum CREATE-ONLY (12111-12120: flatpak/portal soft id
 * stubs — flatpak_ok_u_12111, xdg_portal_ok_u_12112,
 * pipewire_portal_ok_u_12113, camera_portal_ok_u_12114,
 * file_portal_ok_u_12115, secret_portal_ok_u_12116,
 * notification_portal_ok_u_12117, settings_portal_ok_u_12118,
 * portal_soft_ready_u_12119, batch_id_12120). Unique surface only; no
 * multi-def. Distinct from gj_xdg_portal_ok_u_11812 (batch11812),
 * gj_xdg_portal_ok_u_11012 (batch11012), and gj_xdg_portal_ok_u_10812
 * (batch10812). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12112_marker[] = "libcgj-batch12112";

/* Soft xdg-portal-ok lamp: always off (not a real portal probe). */
#define B12112_XDG_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12112_xdg_portal_ok(void)
{
return B12112_XDG_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_portal_ok_u_12112 - xdg portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe XDG desktop
 * portal or call libc. No parent wires.
 */
uint32_t
gj_xdg_portal_ok_u_12112(void)
{
(void)NULL;
return b12112_xdg_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xdg_portal_ok_u_12112(void)
    __attribute__((alias("gj_xdg_portal_ok_u_12112")));
