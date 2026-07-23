/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13113: pipewire portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_portal_ok_u_13113(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PipeWire/screen-cast portal probe for the flatpak/portal
 *       continuum.
 *   uint32_t __gj_pipewire_portal_ok_u_13113  (alias)
 *   __libcgj_batch13113_marker = "libcgj-batch13113"
 *
 * Exclusive continuum CREATE-ONLY (13111-13120: flatpak/portal soft id
 * stubs — flatpak_ok_u_13111, xdg_portal_ok_u_13112,
 * pipewire_portal_ok_u_13113, camera_portal_ok_u_13114,
 * file_portal_ok_u_13115, secret_portal_ok_u_13116,
 * notification_portal_ok_u_13117, settings_portal_ok_u_13118,
 * portal_soft_ready_u_13119, batch_id_13120). Unique surface only; no
 * multi-def. Distinct from gj_pipewire_portal_ok_u_12913 (batch12913),
 * gj_pipewire_portal_ok_u_12713 (batch12713), gj_pipewire_portal_ok_u_12513
 * (batch12513), and gj_pipewire_portal_ok_u_12313
 * (batch12313). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13113_marker[] = "libcgj-batch13113";

/* Soft pipewire-portal-ok lamp: always off (not a real screencast probe). */
#define B13113_PIPEWIRE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13113_pipewire_portal_ok(void)
{
	return B13113_PIPEWIRE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_portal_ok_u_13113 - pipewire portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open PipeWire or
 * screen-cast portals or call libc. No parent wires.
 */
uint32_t
gj_pipewire_portal_ok_u_13113(void)
{
	(void)NULL;
	return b13113_pipewire_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_portal_ok_u_13113(void)
    __attribute__((alias("gj_pipewire_portal_ok_u_13113")));
