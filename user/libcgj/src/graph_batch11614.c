/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11614: filechooser portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_filechooser_ok_u_11614(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FileChooser portal probe for the flatpak portal soft continuum.
 *   uint32_t __gj_filechooser_ok_u_11614  (alias)
 *   __libcgj_batch11614_marker = "libcgj-batch11614"
 *
 * Exclusive continuum CREATE-ONLY (11611-11620: flatpak portal soft —
 * flatpak_ok_u_11611, portal_ok_u_11612, document_ok_u_11613,
 * filechooser_ok_u_11614, openuri_ok_u_11615, network_ok_u_11616,
 * notification_ok_u_11617, screencast_ok_u_11618,
 * portal_soft_ready_u_11619→1, batch_id_11620→11620; others 0).
 * Unique surface only; no multi-def. Distinct from
 * gj_filechooser_ok_u_11414 (batch11414). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11614_marker[] = "libcgj-batch11614";

/* Soft filechooser-ok lamp: always off (not a real FileChooser probe). */
#define B11614_FILECHOOSER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11614_filechooser_ok(void)
{
	return B11614_FILECHOOSER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_filechooser_ok_u_11614 - filechooser portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe the FileChooser
 * portal or call libc. No parent wires.
 */
uint32_t
gj_filechooser_ok_u_11614(void)
{
	(void)NULL;
	return b11614_filechooser_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_filechooser_ok_u_11614(void)
    __attribute__((alias("gj_filechooser_ok_u_11614")));
