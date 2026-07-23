/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10820: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10820(void);
 *     - Returns the compile-time graph batch number for this TU (10820).
 *   uint32_t __gj_batch_id_10820  (alias)
 *   __libcgj_batch10820_marker = "libcgj-batch10820"
 *
 * Exclusive continuum CREATE-ONLY (10811-10820: flatpak/portal soft
 * ok slots all→0; portal_soft_ready→1; batch_id→10820 —
 * flatpak_ok_u_10811, xdg_portal_ok_u_10812,
 * pipewire_portal_ok_u_10813, camera_portal_ok_u_10814,
 * file_portal_ok_u_10815, secret_portal_ok_u_10816,
 * notification_portal_ok_u_10817, settings_portal_ok_u_10818,
 * portal_soft_ready_u_10819, batch_id_10820). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10120 (batch10120). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10820_marker[] = "libcgj-batch10820";

#define B10820_BATCH_ID  10820u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10820_id(void)
{
	return B10820_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10820 - report this TU's graph batch number.
 *
 * Always returns 10820.
 */
uint32_t
gj_batch_id_10820(void)
{
	(void)NULL;
	return b10820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10820(void)
    __attribute__((alias("gj_batch_id_10820")));
