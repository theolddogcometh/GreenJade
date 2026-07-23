/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10119: portal soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_portal_soft_ready_u_10119(void);
 *     - Returns 1 (soft lamp only). Indicates the flatpak/portal soft-
 *       id stub surfaces in this continuum are present; not flatpak,
 *       xdg, pipewire, camera, file, secret, notification, or settings
 *       portal ok.
 *   uint32_t __gj_portal_soft_ready_u_10119  (alias)
 *   __libcgj_batch10119_marker = "libcgj-batch10119"
 *
 * Exclusive continuum CREATE-ONLY (10111-10120: flatpak/portal soft id
 * stubs — flatpak_ok_u_10111, xdg_portal_ok_u_10112,
 * pipewire_portal_ok_u_10113, camera_portal_ok_u_10114,
 * file_portal_ok_u_10115, secret_portal_ok_u_10116,
 * notification_portal_ok_u_10117, settings_portal_ok_u_10118,
 * portal_soft_ready_u_10119, batch_id_10120). Unique surface only; no
 * multi-def. Flatpak/xdg/pipewire/camera/file/secret/notification/
 * settings portal ok units remain 0. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10119_marker[] = "libcgj-batch10119";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B10119_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10119_soft_ready(void)
{
	return B10119_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_portal_soft_ready_u_10119 - portal soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 10111-10120 surfaces are present. Does not claim flatpak/xdg/
 * pipewire/camera/file/secret/notification/settings portal ok and
 * does not call libc. No parent wires.
 */
uint32_t
gj_portal_soft_ready_u_10119(void)
{
	(void)NULL;
	return b10119_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_portal_soft_ready_u_10119(void)
    __attribute__((alias("gj_portal_soft_ready_u_10119")));
