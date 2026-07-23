/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12911: flatpak ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_ok_u_12911(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       flatpak runtime probe for the flatpak/portal continuum.
 *   uint32_t __gj_flatpak_ok_u_12911  (alias)
 *   __libcgj_batch12911_marker = "libcgj-batch12911"
 *
 * Exclusive continuum CREATE-ONLY (12911-12920: flatpak/portal soft id
 * stubs — flatpak_ok_u_12911, xdg_portal_ok_u_12912,
 * pipewire_portal_ok_u_12913, camera_portal_ok_u_12914,
 * file_portal_ok_u_12915, secret_portal_ok_u_12916,
 * notification_portal_ok_u_12917, settings_portal_ok_u_12918,
 * portal_soft_ready_u_12919, batch_id_12920). Unique surface only; no
 * multi-def. Distinct from gj_flatpak_ok_u_12711 (batch12711),
 * gj_flatpak_ok_u_12511 (batch12511), gj_flatpak_ok_u_12311
 * (batch12311), and gj_flatpak_ok_u_12111 (batch12111). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12911_marker[] = "libcgj-batch12911";

/* Soft flatpak-ok lamp: always off (not a real flatpak probe). */
#define B12911_FLATPAK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12911_flatpak_ok(void)
{
	return B12911_FLATPAK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ok_u_12911 - flatpak ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe flatpak or
 * call libc. No parent wires.
 */
uint32_t
gj_flatpak_ok_u_12911(void)
{
	(void)NULL;
	return b12911_flatpak_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_ok_u_12911(void)
    __attribute__((alias("gj_flatpak_ok_u_12911")));
