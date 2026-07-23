/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11420: flatpak portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11420(void);
 *     - Returns the compile-time graph batch number for this TU (11420).
 *   uint32_t __gj_batch_id_11420  (alias)
 *   __libcgj_batch11420_marker = "libcgj-batch11420"
 *
 * Exclusive continuum CREATE-ONLY (11411-11420: flatpak portal soft —
 * flatpak_ok_u_11411, portal_ok_u_11412, document_ok_u_11413,
 * filechooser_ok_u_11414, openuri_ok_u_11415, network_ok_u_11416,
 * notification_ok_u_11417, screencast_ok_u_11418,
 * portal_soft_ready_u_11419, batch_id_11420). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11420_marker[] = "libcgj-batch11420";

#define B11420_BATCH_ID  11420u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11420_id(void)
{
	return B11420_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11420 - report this TU's graph batch number.
 *
 * Always returns 11420.
 */
uint32_t
gj_batch_id_11420(void)
{
	(void)NULL;
	return b11420_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11420(void)
    __attribute__((alias("gj_batch_id_11420")));
