/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11620: flatpak portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11620(void);
 *     - Returns the compile-time graph batch number for this TU (11620).
 *   uint32_t __gj_batch_id_11620  (alias)
 *   __libcgj_batch11620_marker = "libcgj-batch11620"
 *
 * Exclusive continuum CREATE-ONLY (11611-11620: flatpak portal soft —
 * flatpak_ok_u_11611, portal_ok_u_11612, document_ok_u_11613,
 * filechooser_ok_u_11614, openuri_ok_u_11615, network_ok_u_11616,
 * notification_ok_u_11617, screencast_ok_u_11618,
 * portal_soft_ready_u_11619→1, batch_id_11620→11620; others 0).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_11420 (batch11420) and gj_batch_id_11220
 * (batch11220). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11620_marker[] = "libcgj-batch11620";

#define B11620_BATCH_ID  11620u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11620_id(void)
{
	return B11620_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11620 - report this TU's graph batch number.
 *
 * Always returns 11620. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11620(void)
{
	(void)NULL;
	return b11620_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11620(void)
    __attribute__((alias("gj_batch_id_11620")));
