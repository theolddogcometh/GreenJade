/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12519: portal soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_portal_soft_ready_u_12519(void);
 *     - Returns 1 (soft lamp only). Indicates the flatpak/portal soft-
 *       id stub surfaces in this continuum are present; not flatpak,
 *       xdg, pipewire, camera, file, secret, notification, or settings
 *       portal ok.
 *   uint32_t __gj_portal_soft_ready_u_12519  (alias)
 *   __libcgj_batch12519_marker = "libcgj-batch12519"
 *
 * Exclusive continuum CREATE-ONLY (12511-12520: flatpak/portal soft id
 * stubs — flatpak_ok_u_12511, xdg_portal_ok_u_12512,
 * pipewire_portal_ok_u_12513, camera_portal_ok_u_12514,
 * file_portal_ok_u_12515, secret_portal_ok_u_12516,
 * notification_portal_ok_u_12517, settings_portal_ok_u_12518,
 * portal_soft_ready_u_12519, batch_id_12520). Unique surface only; no
 * multi-def. Flatpak/xdg/pipewire/camera/file/secret/notification/
 * settings portal ok units remain 0. Distinct from
 * gj_portal_soft_ready_u_12319 (batch12319),
 * gj_portal_soft_ready_u_12119 (batch12119), and
 * gj_portal_soft_ready_u_12019 (batch12019). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12519_marker[] = "libcgj-batch12519";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B12519_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12519_soft_ready(void)
{
	return B12519_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_portal_soft_ready_u_12519 - portal soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 12511-12520 surfaces are present. Does not claim flatpak/xdg/
 * pipewire/camera/file/secret/notification/settings portal ok and
 * does not call libc. No parent wires.
 */
uint32_t
gj_portal_soft_ready_u_12519(void)
{
	(void)NULL;
	return b12519_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_portal_soft_ready_u_12519(void)
    __attribute__((alias("gj_portal_soft_ready_u_12519")));
