/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13120: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13120(void);
 *     - Returns the compile-time graph batch number for this TU (13120).
 *   uint32_t __gj_batch_id_13120  (alias)
 *   __libcgj_batch13120_marker = "libcgj-batch13120"
 *
 * Exclusive continuum CREATE-ONLY (13111-13120: flatpak/portal soft id
 * stubs — flatpak_ok_u_13111, xdg_portal_ok_u_13112,
 * pipewire_portal_ok_u_13113, camera_portal_ok_u_13114,
 * file_portal_ok_u_13115, secret_portal_ok_u_13116,
 * notification_portal_ok_u_13117, settings_portal_ok_u_13118,
 * portal_soft_ready_u_13119, batch_id_13120). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_12920 (batch12920), gj_batch_id_12720 (batch12720),
 * gj_batch_id_12520 (batch12520), and gj_batch_id_12320 (batch12320).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13120_marker[] = "libcgj-batch13120";

#define B13120_BATCH_ID  13120u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13120_id(void)
{
	return B13120_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13120 - report this TU's graph batch number.
 *
 * Always returns 13120.
 */
uint32_t
gj_batch_id_13120(void)
{
	(void)NULL;
	return b13120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13120(void)
    __attribute__((alias("gj_batch_id_13120")));
