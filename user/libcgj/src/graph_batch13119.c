/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13119: portal soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_portal_soft_ready_u_13119(void);
 *     - Returns 1 (soft lamp only). Indicates the flatpak/portal soft-
 *       id stub surfaces in this continuum are present; not flatpak,
 *       xdg, pipewire, camera, file, secret, notification, or settings
 *       portal ok.
 *   uint32_t __gj_portal_soft_ready_u_13119  (alias)
 *   __libcgj_batch13119_marker = "libcgj-batch13119"
 *
 * Exclusive continuum CREATE-ONLY (13111-13120: flatpak/portal soft id
 * stubs — flatpak_ok_u_13111, xdg_portal_ok_u_13112,
 * pipewire_portal_ok_u_13113, camera_portal_ok_u_13114,
 * file_portal_ok_u_13115, secret_portal_ok_u_13116,
 * notification_portal_ok_u_13117, settings_portal_ok_u_13118,
 * portal_soft_ready_u_13119, batch_id_13120). Unique surface only; no
 * multi-def. Distinct from gj_portal_soft_ready_u_12919 (batch12919),
 * gj_portal_soft_ready_u_12719 (batch12719),
 * gj_portal_soft_ready_u_12519 (batch12519), and
 * gj_portal_soft_ready_u_12319 (batch12319). Flatpak/xdg/pipewire/
 * camera/file/secret/notification/settings portal ok units remain 0.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13119_marker[] = "libcgj-batch13119";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B13119_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13119_soft_ready(void)
{
	return B13119_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_portal_soft_ready_u_13119 - portal soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 13111-13120 surfaces are present. Does not claim flatpak/xdg/
 * pipewire/camera/file/secret/notification/settings portal ok and
 * does not call libc. No parent wires.
 */
uint32_t
gj_portal_soft_ready_u_13119(void)
{
	(void)NULL;
	return b13119_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_portal_soft_ready_u_13119(void)
    __attribute__((alias("gj_portal_soft_ready_u_13119")));
