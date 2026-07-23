/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13911: flatpak ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_ok_u_13911(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       flatpak runtime probe for the flatpak/portal continuum.
 *   uint32_t __gj_flatpak_ok_u_13911  (alias)
 *   __libcgj_batch13911_marker = "libcgj-batch13911"
 *
 * Exclusive continuum CREATE-ONLY (13911-13920: flatpak/portal soft id
 * stubs — flatpak_ok_u_13911, xdg_portal_ok_u_13912,
 * pipewire_portal_ok_u_13913, camera_portal_ok_u_13914,
 * file_portal_ok_u_13915, secret_portal_ok_u_13916,
 * notification_portal_ok_u_13917, settings_portal_ok_u_13918,
 * portal_soft_ready_u_13919, batch_id_13920). Unique surface only; no
 * multi-def. Distinct from gj_flatpak_ok_u_10111 (batch10111). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13911_marker[] = "libcgj-batch13911";

/* Soft flatpak-ok lamp: always off (not a real flatpak probe). */
#define B13911_FLATPAK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13911_flatpak_ok(void)
{
	return B13911_FLATPAK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ok_u_13911 - flatpak ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe flatpak or
 * call libc. No parent wires.
 */
uint32_t
gj_flatpak_ok_u_13911(void)
{
	(void)NULL;
	return b13911_flatpak_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_ok_u_13911(void)
    __attribute__((alias("gj_flatpak_ok_u_13911")));
