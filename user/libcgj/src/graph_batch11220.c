/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11220: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11220(void);
 *     - Returns the compile-time graph batch number for this TU (11220).
 *   uint32_t __gj_batch_id_11220  (alias)
 *   __libcgj_batch11220_marker = "libcgj-batch11220"
 *
 * Exclusive continuum CREATE-ONLY (11211-11220: flatpak/portal soft
 * ok slots all→0; portal_soft_ready→1; batch_id→11220 —
 * flatpak_ok_u_11211, xdg_portal_ok_u_11212,
 * pipewire_portal_ok_u_11213, camera_portal_ok_u_11214,
 * file_portal_ok_u_11215, secret_portal_ok_u_11216,
 * notification_portal_ok_u_11217, settings_portal_ok_u_11218,
 * portal_soft_ready_u_11219, batch_id_11220). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11020 (batch11020). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11220_marker[] = "libcgj-batch11220";

#define B11220_BATCH_ID  11220u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11220_id(void)
{
	return B11220_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11220 - report this TU's graph batch number.
 *
 * Always returns 11220.
 */
uint32_t
gj_batch_id_11220(void)
{
	(void)NULL;
	return b11220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11220(void)
    __attribute__((alias("gj_batch_id_11220")));
