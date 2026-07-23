/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12511: flatpak ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_ok_u_12511(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       flatpak runtime probe for the flatpak/portal continuum.
 *   uint32_t __gj_flatpak_ok_u_12511  (alias)
 *   __libcgj_batch12511_marker = "libcgj-batch12511"
 *
 * Exclusive continuum CREATE-ONLY (12511-12520: flatpak/portal soft id
 * stubs — flatpak_ok_u_12511, xdg_portal_ok_u_12512,
 * pipewire_portal_ok_u_12513, camera_portal_ok_u_12514,
 * file_portal_ok_u_12515, secret_portal_ok_u_12516,
 * notification_portal_ok_u_12517, settings_portal_ok_u_12518,
 * portal_soft_ready_u_12519, batch_id_12520). Unique surface only; no
 * multi-def. Distinct from gj_flatpak_ok_u_12311 (batch12311),
 * gj_flatpak_ok_u_12111 (batch12111), and gj_flatpak_ok_u_12011
 * (batch12011). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12511_marker[] = "libcgj-batch12511";

/* Soft flatpak-ok lamp: always off (not a real flatpak probe). */
#define B12511_FLATPAK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12511_flatpak_ok(void)
{
	return B12511_FLATPAK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ok_u_12511 - flatpak ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe flatpak or
 * call libc. No parent wires.
 */
uint32_t
gj_flatpak_ok_u_12511(void)
{
	(void)NULL;
	return b12511_flatpak_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_ok_u_12511(void)
    __attribute__((alias("gj_flatpak_ok_u_12511")));
