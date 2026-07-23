/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11020: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11020(void);
 *     - Returns the compile-time graph batch number for this TU (11020).
 *   uint32_t __gj_batch_id_11020  (alias)
 *   __libcgj_batch11020_marker = "libcgj-batch11020"
 *
 * Exclusive continuum CREATE-ONLY (11011-11020: flatpak/portal soft
 * ok slots all→0; portal_soft_ready→1; batch_id→11020 —
 * flatpak_ok_u_11011, xdg_portal_ok_u_11012,
 * pipewire_portal_ok_u_11013, camera_portal_ok_u_11014,
 * file_portal_ok_u_11015, secret_portal_ok_u_11016,
 * notification_portal_ok_u_11017, settings_portal_ok_u_11018,
 * portal_soft_ready_u_11019, batch_id_11020). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10820 (batch10820). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11020_marker[] = "libcgj-batch11020";

#define B11020_BATCH_ID  11020u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11020_id(void)
{
	return B11020_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11020 - report this TU's graph batch number.
 *
 * Always returns 11020.
 */
uint32_t
gj_batch_id_11020(void)
{
	(void)NULL;
	return b11020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11020(void)
    __attribute__((alias("gj_batch_id_11020")));
