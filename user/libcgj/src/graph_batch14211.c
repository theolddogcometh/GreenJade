/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14211: flatpak ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_ok_u_14211(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       flatpak runtime probe for the flatpak/portal continuum.
 *   uint32_t __gj_flatpak_ok_u_14211  (alias)
 *   __libcgj_batch14211_marker = "libcgj-batch14211"
 *
 * Exclusive continuum CREATE-ONLY (14211-14220: flatpak/portal soft id
 * stubs — flatpak_ok_u_14211, xdg_portal_ok_u_14212,
 * pipewire_portal_ok_u_14213, camera_portal_ok_u_14214,
 * file_portal_ok_u_14215, secret_portal_ok_u_14216,
 * notification_portal_ok_u_14217, settings_portal_ok_u_14218,
 * portal_soft_ready_u_14219, batch_id_14220). Unique surface only; no
 * multi-def. Distinct from gj_flatpak_ok_u_13911 (batch13911). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14211_marker[] = "libcgj-batch14211";

/* Soft flatpak-ok lamp: always off (not a real flatpak probe). */
#define B14211_FLATPAK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14211_flatpak_ok(void)
{
	return B14211_FLATPAK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ok_u_14211 - flatpak ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe flatpak or
 * call libc. No parent wires.
 */
uint32_t
gj_flatpak_ok_u_14211(void)
{
	(void)NULL;
	return b14211_flatpak_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_ok_u_14211(void)
    __attribute__((alias("gj_flatpak_ok_u_14211")));
