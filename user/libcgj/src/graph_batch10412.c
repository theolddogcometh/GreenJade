/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10412: xdg portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xdg_portal_ok_u_10412(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       XDG desktop portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_xdg_portal_ok_u_10412  (alias)
 *   __libcgj_batch10412_marker = "libcgj-batch10412"
 *
 * Exclusive continuum CREATE-ONLY (10411-10420: flatpak/portal soft id
 * stubs — flatpak_ok_u_10411, xdg_portal_ok_u_10412,
 * pipewire_portal_ok_u_10413, camera_portal_ok_u_10414,
 * file_portal_ok_u_10415, secret_portal_ok_u_10416,
 * notification_portal_ok_u_10417, settings_portal_ok_u_10418,
 * portal_soft_ready_u_10419, batch_id_10420). Unique surface only; no
 * multi-def. Distinct from gj_xdg_portal_ok_u_10112 (batch10112). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10412_marker[] = "libcgj-batch10412";

/* Soft xdg-portal-ok lamp: always off (not a real portal probe). */
#define B10412_XDG_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10412_xdg_portal_ok(void)
{
	return B10412_XDG_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_portal_ok_u_10412 - xdg portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe xdg-desktop-
 * portal or call libc. No parent wires.
 */
uint32_t
gj_xdg_portal_ok_u_10412(void)
{
	(void)NULL;
	return b10412_xdg_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xdg_portal_ok_u_10412(void)
    __attribute__((alias("gj_xdg_portal_ok_u_10412")));
