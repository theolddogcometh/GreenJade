/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13919: portal soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_portal_soft_ready_u_13919(void);
 *     - Returns 1 (soft lamp only). Indicates the flatpak/portal soft-
 *       id stub surfaces in this continuum are present; not flatpak,
 *       xdg, pipewire, camera, file, secret, notification, or settings
 *       portal ok.
 *   uint32_t __gj_portal_soft_ready_u_13919  (alias)
 *   __libcgj_batch13919_marker = "libcgj-batch13919"
 *
 * Exclusive continuum CREATE-ONLY (13911-13920: flatpak/portal soft id
 * stubs — flatpak_ok_u_13911, xdg_portal_ok_u_13912,
 * pipewire_portal_ok_u_13913, camera_portal_ok_u_13914,
 * file_portal_ok_u_13915, secret_portal_ok_u_13916,
 * notification_portal_ok_u_13917, settings_portal_ok_u_13918,
 * portal_soft_ready_u_13919, batch_id_13920). Unique surface only; no
 * multi-def. Flatpak/xdg/pipewire/camera/file/secret/notification/
 * settings portal ok units remain 0. Distinct from
 * gj_portal_soft_ready_u_10119 (batch10119). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13919_marker[] = "libcgj-batch13919";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B13919_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13919_soft_ready(void)
{
	return B13919_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_portal_soft_ready_u_13919 - portal soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 13911-13920 surfaces are present. Does not claim flatpak/xdg/
 * pipewire/camera/file/secret/notification/settings portal ok and
 * does not call libc. No parent wires.
 */
uint32_t
gj_portal_soft_ready_u_13919(void)
{
	(void)NULL;
	return b13919_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_portal_soft_ready_u_13919(void)
    __attribute__((alias("gj_portal_soft_ready_u_13919")));
