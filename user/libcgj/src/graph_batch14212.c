/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14212: xdg portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xdg_portal_ok_u_14212(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       XDG desktop portal probe for the flatpak/portal continuum.
 *   uint32_t __gj_xdg_portal_ok_u_14212  (alias)
 *   __libcgj_batch14212_marker = "libcgj-batch14212"
 *
 * Exclusive continuum CREATE-ONLY (14211-14220: flatpak/portal soft id
 * stubs — flatpak_ok_u_14211, xdg_portal_ok_u_14212,
 * pipewire_portal_ok_u_14213, camera_portal_ok_u_14214,
 * file_portal_ok_u_14215, secret_portal_ok_u_14216,
 * notification_portal_ok_u_14217, settings_portal_ok_u_14218,
 * portal_soft_ready_u_14219, batch_id_14220). Unique surface only; no
 * multi-def. Distinct from gj_xdg_portal_ok_u_13912 (batch13912). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14212_marker[] = "libcgj-batch14212";

/* Soft xdg-portal-ok lamp: always off (not a real portal probe). */
#define B14212_XDG_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14212_xdg_portal_ok(void)
{
	return B14212_XDG_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_portal_ok_u_14212 - xdg portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not talk to xdg-desktop-
 * portal or call libc. No parent wires.
 */
uint32_t
gj_xdg_portal_ok_u_14212(void)
{
	(void)NULL;
	return b14212_xdg_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xdg_portal_ok_u_14212(void)
    __attribute__((alias("gj_xdg_portal_ok_u_14212")));
