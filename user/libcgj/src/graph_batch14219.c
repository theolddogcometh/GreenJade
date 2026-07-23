/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14219: portal soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_portal_soft_ready_u_14219(void);
 *     - Returns 1 (soft lamp only). Indicates the flatpak/portal soft-
 *       id stub surfaces in this continuum are present; not flatpak,
 *       xdg, pipewire, camera, file, secret, notification, or settings
 *       portal ok.
 *   uint32_t __gj_portal_soft_ready_u_14219  (alias)
 *   __libcgj_batch14219_marker = "libcgj-batch14219"
 *
 * Exclusive continuum CREATE-ONLY (14211-14220: flatpak/portal soft id
 * stubs — flatpak_ok_u_14211, xdg_portal_ok_u_14212,
 * pipewire_portal_ok_u_14213, camera_portal_ok_u_14214,
 * file_portal_ok_u_14215, secret_portal_ok_u_14216,
 * notification_portal_ok_u_14217, settings_portal_ok_u_14218,
 * portal_soft_ready_u_14219, batch_id_14220). Unique surface only; no
 * multi-def. Flatpak/xdg/pipewire/camera/file/secret/notification/
 * settings portal ok units remain 0. Distinct from
 * gj_portal_soft_ready_u_13919 (batch13919). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14219_marker[] = "libcgj-batch14219";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B14219_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14219_soft_ready(void)
{
	return B14219_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_portal_soft_ready_u_14219 - portal soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 14211-14220 surfaces are present. Does not claim flatpak/xdg/
 * pipewire/camera/file/secret/notification/settings portal ok and
 * does not call libc. No parent wires.
 */
uint32_t
gj_portal_soft_ready_u_14219(void)
{
	(void)NULL;
	return b14219_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_portal_soft_ready_u_14219(void)
    __attribute__((alias("gj_portal_soft_ready_u_14219")));
