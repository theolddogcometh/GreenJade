/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12913: pipewire portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_portal_ok_u_12913(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PipeWire/screen-cast portal probe for the flatpak/portal
 *       continuum.
 *   uint32_t __gj_pipewire_portal_ok_u_12913  (alias)
 *   __libcgj_batch12913_marker = "libcgj-batch12913"
 *
 * Exclusive continuum CREATE-ONLY (12911-12920: flatpak/portal soft id
 * stubs — flatpak_ok_u_12911, xdg_portal_ok_u_12912,
 * pipewire_portal_ok_u_12913, camera_portal_ok_u_12914,
 * file_portal_ok_u_12915, secret_portal_ok_u_12916,
 * notification_portal_ok_u_12917, settings_portal_ok_u_12918,
 * portal_soft_ready_u_12919, batch_id_12920). Unique surface only; no
 * multi-def. Distinct from gj_pipewire_portal_ok_u_12713 (batch12713),
 * gj_pipewire_portal_ok_u_12513 (batch12513),
 * gj_pipewire_portal_ok_u_12313 (batch12313), and
 * gj_pipewire_portal_ok_u_12113 (batch12113). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12913_marker[] = "libcgj-batch12913";

/* Soft pipewire-portal-ok lamp: always off (not a real screencast probe). */
#define B12913_PIPEWIRE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12913_pipewire_portal_ok(void)
{
	return B12913_PIPEWIRE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_portal_ok_u_12913 - pipewire portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open PipeWire or
 * screen-cast portals or call libc. No parent wires.
 */
uint32_t
gj_pipewire_portal_ok_u_12913(void)
{
	(void)NULL;
	return b12913_pipewire_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_portal_ok_u_12913(void)
    __attribute__((alias("gj_pipewire_portal_ok_u_12913")));
