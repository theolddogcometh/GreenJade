/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11611: flatpak ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_ok_u_11611(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       flatpak/portal probe for the flatpak portal soft continuum.
 *   uint32_t __gj_flatpak_ok_u_11611  (alias)
 *   __libcgj_batch11611_marker = "libcgj-batch11611"
 *
 * Exclusive continuum CREATE-ONLY (11611-11620: flatpak portal soft —
 * flatpak_ok_u_11611, portal_ok_u_11612, document_ok_u_11613,
 * filechooser_ok_u_11614, openuri_ok_u_11615, network_ok_u_11616,
 * notification_ok_u_11617, screencast_ok_u_11618,
 * portal_soft_ready_u_11619→1, batch_id_11620→11620; others 0).
 * Unique surface only; no multi-def. Distinct from
 * gj_flatpak_ok_u_11411 (batch11411) and gj_flatpak_ok_u_11211
 * (batch11211). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11611_marker[] = "libcgj-batch11611";

/* Soft flatpak-ok lamp: always off (not a real flatpak probe). */
#define B11611_FLATPAK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11611_flatpak_ok(void)
{
	return B11611_FLATPAK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ok_u_11611 - flatpak ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe flatpak or
 * call libc. No parent wires.
 */
uint32_t
gj_flatpak_ok_u_11611(void)
{
	(void)NULL;
	return b11611_flatpak_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_ok_u_11611(void)
    __attribute__((alias("gj_flatpak_ok_u_11611")));
