/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12513: pipewire portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_portal_ok_u_12513(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PipeWire/screen-cast portal probe for the flatpak/portal
 *       continuum.
 *   uint32_t __gj_pipewire_portal_ok_u_12513  (alias)
 *   __libcgj_batch12513_marker = "libcgj-batch12513"
 *
 * Exclusive continuum CREATE-ONLY (12511-12520: flatpak/portal soft id
 * stubs — flatpak_ok_u_12511, xdg_portal_ok_u_12512,
 * pipewire_portal_ok_u_12513, camera_portal_ok_u_12514,
 * file_portal_ok_u_12515, secret_portal_ok_u_12516,
 * notification_portal_ok_u_12517, settings_portal_ok_u_12518,
 * portal_soft_ready_u_12519, batch_id_12520). Unique surface only; no
 * multi-def. Distinct from gj_pipewire_portal_ok_u_12313
 * (batch12313), gj_pipewire_portal_ok_u_12113 (batch12113), and
 * gj_pipewire_portal_ok_u_12013 (batch12013). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12513_marker[] = "libcgj-batch12513";

/* Soft pipewire-portal-ok lamp: always off (not a real portal probe). */
#define B12513_PIPEWIRE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12513_pipewire_portal_ok(void)
{
	return B12513_PIPEWIRE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_portal_ok_u_12513 - pipewire portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe PipeWire
 * ScreenCast portal and does not call libc. No parent wires.
 */
uint32_t
gj_pipewire_portal_ok_u_12513(void)
{
	(void)NULL;
	return b12513_pipewire_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_portal_ok_u_12513(void)
    __attribute__((alias("gj_pipewire_portal_ok_u_12513")));
