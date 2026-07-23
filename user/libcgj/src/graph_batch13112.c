/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13112: xdg portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xdg_portal_ok_u_13112(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       XDG desktop portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_xdg_portal_ok_u_13112  (alias)
 *   __libcgj_batch13112_marker = "libcgj-batch13112"
 *
 * Exclusive continuum CREATE-ONLY (13111-13120: flatpak/portal soft id
 * stubs — flatpak_ok_u_13111, xdg_portal_ok_u_13112,
 * pipewire_portal_ok_u_13113, camera_portal_ok_u_13114,
 * file_portal_ok_u_13115, secret_portal_ok_u_13116,
 * notification_portal_ok_u_13117, settings_portal_ok_u_13118,
 * portal_soft_ready_u_13119, batch_id_13120). Unique surface only; no
 * multi-def. Distinct from gj_xdg_portal_ok_u_12912 (batch12912),
 * gj_xdg_portal_ok_u_12712 (batch12712), gj_xdg_portal_ok_u_12512
 * (batch12512), and gj_xdg_portal_ok_u_12312
 * (batch12312). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13112_marker[] = "libcgj-batch13112";

/* Soft xdg-portal-ok lamp: always off (not a real portal probe). */
#define B13112_XDG_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13112_xdg_portal_ok(void)
{
	return B13112_XDG_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_portal_ok_u_13112 - xdg portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not talk to xdg-desktop-
 * portal or call libc. No parent wires.
 */
uint32_t
gj_xdg_portal_ok_u_13112(void)
{
	(void)NULL;
	return b13112_xdg_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xdg_portal_ok_u_13112(void)
    __attribute__((alias("gj_xdg_portal_ok_u_13112")));
