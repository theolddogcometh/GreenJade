/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12313: pipewire portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_portal_ok_u_12313(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PipeWire/screen-cast portal probe for the flatpak/portal
 *       continuum.
 *   uint32_t __gj_pipewire_portal_ok_u_12313  (alias)
 *   __libcgj_batch12313_marker = "libcgj-batch12313"
 *
 * Exclusive continuum CREATE-ONLY (12311-12320: flatpak/portal soft id
 * stubs — flatpak_ok_u_12311, xdg_portal_ok_u_12312,
 * pipewire_portal_ok_u_12313, camera_portal_ok_u_12314,
 * file_portal_ok_u_12315, secret_portal_ok_u_12316,
 * notification_portal_ok_u_12317, settings_portal_ok_u_12318,
 * portal_soft_ready_u_12319, batch_id_12320). Unique surface only; no
 * multi-def. Distinct from gj_pipewire_portal_ok_u_12113
 * (batch12113), gj_pipewire_portal_ok_u_12013 (batch12013), and
 * gj_pipewire_portal_ok_u_11913 (batch11913). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12313_marker[] = "libcgj-batch12313";

/* Soft pipewire-portal-ok lamp: always off (not a real portal probe). */
#define B12313_PIPEWIRE_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12313_pipewire_portal_ok(void)
{
	return B12313_PIPEWIRE_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_portal_ok_u_12313 - pipewire portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe PipeWire
 * ScreenCast portal and does not call libc. No parent wires.
 */
uint32_t
gj_pipewire_portal_ok_u_12313(void)
{
	(void)NULL;
	return b12313_pipewire_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_portal_ok_u_12313(void)
    __attribute__((alias("gj_pipewire_portal_ok_u_12313")));
