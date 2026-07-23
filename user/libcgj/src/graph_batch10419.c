/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10419: portal soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_portal_soft_ready_u_10419(void);
 *     - Returns 1 (soft lamp only). Indicates the flatpak/portal soft-
 *       id stub surfaces in this continuum are present; not flatpak,
 *       xdg, pipewire, camera, file, secret, notification, or settings
 *       portal ok.
 *   uint32_t __gj_portal_soft_ready_u_10419  (alias)
 *   __libcgj_batch10419_marker = "libcgj-batch10419"
 *
 * Exclusive continuum CREATE-ONLY (10411-10420: flatpak/portal soft id
 * stubs — flatpak_ok_u_10411, xdg_portal_ok_u_10412,
 * pipewire_portal_ok_u_10413, camera_portal_ok_u_10414,
 * file_portal_ok_u_10415, secret_portal_ok_u_10416,
 * notification_portal_ok_u_10417, settings_portal_ok_u_10418,
 * portal_soft_ready_u_10419, batch_id_10420). Unique surface only; no
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

const char __libcgj_batch10419_marker[] = "libcgj-batch10419";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B10419_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10419_soft_ready(void)
{
	return B10419_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_portal_soft_ready_u_10419 - portal soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 10411-10420 surfaces are present. Does not claim flatpak/xdg/
 * pipewire/camera/file/secret/notification/settings portal ok and
 * does not call libc. No parent wires.
 */
uint32_t
gj_portal_soft_ready_u_10419(void)
{
	(void)NULL;
	return b10419_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_portal_soft_ready_u_10419(void)
    __attribute__((alias("gj_portal_soft_ready_u_10419")));
