/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12113: pipewire portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_portal_ok_u_12113(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PipeWire/screen-cast portal probe for the flatpak/portal
 *       continuum.
 *   uint32_t __gj_pipewire_portal_ok_u_12113  (alias)
 *   __libcgj_batch12113_marker = "libcgj-batch12113"
 *
 * Exclusive continuum CREATE-ONLY (12111-12120: flatpak/portal soft id
 * stubs — flatpak_ok_u_12111, xdg_portal_ok_u_12112,
 * pipewire_portal_ok_u_12113, camera_portal_ok_u_12114,
 * file_portal_ok_u_12115, secret_portal_ok_u_12116,
 * notification_portal_ok_u_12117, settings_portal_ok_u_12118,
 * portal_soft_ready_u_12119, batch_id_12120). Unique surface only; no
 * multi-def. Distinct from gj_pipewire_portal_ok_u_11813
 * (batch11813), gj_pipewire_portal_ok_u_11013 (batch11013), and
 * gj_pipewire_portal_ok_u_10813 (batch10813). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12113_marker[] = "libcgj-batch12113";

/* Soft pipewire-portal-ok lamp: always off (not a real portal probe). */
#define B12113_PIPEWIRE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12113_pipewire_portal_ok(void)
{
return B12113_PIPEWIRE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_portal_ok_u_12113 - pipewire portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe PipeWire
 * ScreenCast portal and does not call libc. No parent wires.
 */
uint32_t
gj_pipewire_portal_ok_u_12113(void)
{
(void)NULL;
return b12113_pipewire_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_portal_ok_u_12113(void)
    __attribute__((alias("gj_pipewire_portal_ok_u_12113")));
