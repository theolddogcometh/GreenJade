/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11811: flatpak ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_ok_u_11811(void);
 *     - Always returns 0. Soft stub only; intentionally not a real flatpak runtime probe for the flatpak/portal continuum.
 *   uint32_t __gj_flatpak_ok_u_11811  (alias)
 *   __libcgj_batch11811_marker = "libcgj-batch11811"
 *
 * Exclusive continuum CREATE-ONLY (11811-11820: flatpak/portal soft id
 * stubs — flatpak_ok_u_11811, xdg_portal_ok_u_11812,
 * pipewire_portal_ok_u_11813, camera_portal_ok_u_11814,
 * file_portal_ok_u_11815, secret_portal_ok_u_11816,
 * notification_portal_ok_u_11817, settings_portal_ok_u_11818,
 * portal_soft_ready_u_11819, batch_id_11820). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11811_marker[] = "libcgj-batch11811";

/* Soft flatpak-ok lamp: always off (not a real flatpak probe). */
#define B11811_FLATPAK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11811_flatpak_ok(void)
{
	return B11811_FLATPAK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ok_u_11811 - flatpak ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe flatpak or call libc. No parent wires.
 */
uint32_t
gj_flatpak_ok_u_11811(void)
{
	(void)NULL;
	return b11811_flatpak_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_ok_u_11811(void)
    __attribute__((alias("gj_flatpak_ok_u_11811")));
