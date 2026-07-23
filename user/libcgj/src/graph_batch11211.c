/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11211: flatpak ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_ok_u_11211(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       flatpak runtime probe for the flatpak/portal continuum.
 *   uint32_t __gj_flatpak_ok_u_11211  (alias)
 *   __libcgj_batch11211_marker = "libcgj-batch11211"
 *
 * Exclusive continuum CREATE-ONLY (11211-11220: flatpak/portal soft
 * ok slots all→0; portal_soft_ready→1; batch_id→11220 —
 * flatpak_ok_u_11211, xdg_portal_ok_u_11212,
 * pipewire_portal_ok_u_11213, camera_portal_ok_u_11214,
 * file_portal_ok_u_11215, secret_portal_ok_u_11216,
 * notification_portal_ok_u_11217, settings_portal_ok_u_11218,
 * portal_soft_ready_u_11219, batch_id_11220). Unique surface only; no
 * multi-def. Distinct from gj_flatpak_ok_u_11011 (batch11011). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11211_marker[] = "libcgj-batch11211";

/* Soft flatpak-ok lamp: always off (not a real flatpak probe). */
#define B11211_FLATPAK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11211_flatpak_ok(void)
{
	return B11211_FLATPAK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ok_u_11211 - flatpak ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe flatpak or
 * call libc. No parent wires.
 */
uint32_t
gj_flatpak_ok_u_11211(void)
{
	(void)NULL;
	return b11211_flatpak_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_ok_u_11211(void)
    __attribute__((alias("gj_flatpak_ok_u_11211")));
