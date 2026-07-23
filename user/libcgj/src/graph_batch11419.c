/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11419: flatpak portal soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_portal_soft_ready_u_11419(void);
 *     - Returns 1 (soft lamp only). Indicates the flatpak portal soft-id
 *       stub surfaces in this continuum are present; not flatpak, portal,
 *       document, filechooser, openuri, network, notification, or
 *       screencast ok.
 *   uint32_t __gj_portal_soft_ready_u_11419  (alias)
 *   __libcgj_batch11419_marker = "libcgj-batch11419"
 *
 * Exclusive continuum CREATE-ONLY (11411-11420: flatpak portal soft —
 * flatpak_ok_u_11411, portal_ok_u_11412, document_ok_u_11413,
 * filechooser_ok_u_11414, openuri_ok_u_11415, network_ok_u_11416,
 * notification_ok_u_11417, screencast_ok_u_11418,
 * portal_soft_ready_u_11419, batch_id_11420). Unique surface only; no
 * multi-def. Flatpak/portal/document/filechooser/openuri/network/
 * notification/screencast ok units remain 0. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11419_marker[] = "libcgj-batch11419";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B11419_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11419_soft_ready(void)
{
	return B11419_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_portal_soft_ready_u_11419 - flatpak portal soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 11411-11420 surfaces are present. Does not claim flatpak/portal/
 * document/filechooser/openuri/network/notification/screencast ok and
 * does not call libc. No parent wires.
 */
uint32_t
gj_portal_soft_ready_u_11419(void)
{
	(void)NULL;
	return b11419_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_portal_soft_ready_u_11419(void)
    __attribute__((alias("gj_portal_soft_ready_u_11419")));
