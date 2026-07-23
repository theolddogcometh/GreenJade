/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12713: pipewire portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_portal_ok_u_12713(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PipeWire/screen-cast portal probe for the flatpak/portal
 *       continuum.
 *   uint32_t __gj_pipewire_portal_ok_u_12713  (alias)
 *   __libcgj_batch12713_marker = "libcgj-batch12713"
 *
 * Exclusive continuum CREATE-ONLY (12711-12720: flatpak/portal soft id
 * stubs — flatpak_ok_u_12711, xdg_portal_ok_u_12712,
 * pipewire_portal_ok_u_12713, camera_portal_ok_u_12714,
 * file_portal_ok_u_12715, secret_portal_ok_u_12716,
 * notification_portal_ok_u_12717, settings_portal_ok_u_12718,
 * portal_soft_ready_u_12719, batch_id_12720). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12713_marker[] = "libcgj-batch12713";

/* Soft pipewire-portal-ok lamp: always off (not a real screencast probe). */
#define B12713_PIPEWIRE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12713_pipewire_portal_ok(void)
{
	return B12713_PIPEWIRE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_portal_ok_u_12713 - pipewire portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open PipeWire or
 * screen-cast portals or call libc. No parent wires.
 */
uint32_t
gj_pipewire_portal_ok_u_12713(void)
{
	(void)NULL;
	return b12713_pipewire_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_portal_ok_u_12713(void)
    __attribute__((alias("gj_pipewire_portal_ok_u_12713")));
