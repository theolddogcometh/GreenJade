/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12912: xdg portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xdg_portal_ok_u_12912(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       XDG desktop portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_xdg_portal_ok_u_12912  (alias)
 *   __libcgj_batch12912_marker = "libcgj-batch12912"
 *
 * Exclusive continuum CREATE-ONLY (12911-12920: flatpak/portal soft id
 * stubs — flatpak_ok_u_12911, xdg_portal_ok_u_12912,
 * pipewire_portal_ok_u_12913, camera_portal_ok_u_12914,
 * file_portal_ok_u_12915, secret_portal_ok_u_12916,
 * notification_portal_ok_u_12917, settings_portal_ok_u_12918,
 * portal_soft_ready_u_12919, batch_id_12920). Unique surface only; no
 * multi-def. Distinct from gj_xdg_portal_ok_u_12712 (batch12712),
 * gj_xdg_portal_ok_u_12512 (batch12512), gj_xdg_portal_ok_u_12312
 * (batch12312), and gj_xdg_portal_ok_u_12112 (batch12112). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12912_marker[] = "libcgj-batch12912";

/* Soft xdg-portal-ok lamp: always off (not a real portal probe). */
#define B12912_XDG_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12912_xdg_portal_ok(void)
{
	return B12912_XDG_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_portal_ok_u_12912 - xdg portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not talk to xdg-desktop-
 * portal or call libc. No parent wires.
 */
uint32_t
gj_xdg_portal_ok_u_12912(void)
{
	(void)NULL;
	return b12912_xdg_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xdg_portal_ok_u_12912(void)
    __attribute__((alias("gj_xdg_portal_ok_u_12912")));
