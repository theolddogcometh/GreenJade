/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13512: xdg-desktop-portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xdg_portal_ok_u_13512(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       xdg-desktop-portal daemon probe for the flatpak portal continuum.
 *   uint32_t __gj_xdg_portal_ok_u_13512  (alias)
 *   __libcgj_batch13512_marker = "libcgj-batch13512"
 *
 * Exclusive continuum CREATE-ONLY (13511-13520: flatpak portal soft id
 * stubs — flatpak_ok_u_13511, xdg_portal_ok_u_13512,
 * documents_ok_u_13513, file_chooser_ok_u_13514, open_uri_ok_u_13515,
 * screenshot_ok_u_13516, notification_ok_u_13517, secrets_ok_u_13518,
 * portal_soft_ready_u_13519, batch_id_13520→13520).
 * Unique surface only; no multi-def. Distinct from sibling flatpak
 * portal soft stubs in this wave and prior gj_xdg_portal_ok_u_13312 /
 * gj_xdg_portal_ok_u_13112. No parent wires. No __int128. No portal
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13512_marker[] = "libcgj-batch13512";

/* Soft xdg-portal-ok lamp: always off (not a real portal probe). */
#define B13512_XDG_PORTAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13512_xdg_portal_ok(void)
{
	return B13512_XDG_PORTAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_portal_ok_u_13512 - xdg-desktop-portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect
 * xdg-desktop-portal or call libc. No parent wires.
 */
uint32_t
gj_xdg_portal_ok_u_13512(void)
{
	(void)NULL;
	return b13512_xdg_portal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xdg_portal_ok_u_13512(void)
    __attribute__((alias("gj_xdg_portal_ok_u_13512")));
