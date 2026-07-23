/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12320: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12320(void);
 *     - Returns the compile-time graph batch number for this TU (12320).
 *   uint32_t __gj_batch_id_12320  (alias)
 *   __libcgj_batch12320_marker = "libcgj-batch12320"
 *
 * Exclusive continuum CREATE-ONLY (12311-12320: flatpak/portal soft id
 * stubs — flatpak_ok_u_12311, xdg_portal_ok_u_12312,
 * pipewire_portal_ok_u_12313, camera_portal_ok_u_12314,
 * file_portal_ok_u_12315, secret_portal_ok_u_12316,
 * notification_portal_ok_u_12317, settings_portal_ok_u_12318,
 * portal_soft_ready_u_12319, batch_id_12320). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_12120 (batch12120), gj_batch_id_12020 (batch12020), and
 * gj_batch_id_11920 (batch11920). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12320_marker[] = "libcgj-batch12320";

#define B12320_BATCH_ID  12320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12320_id(void)
{
	return B12320_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12320 - report this TU's graph batch number.
 *
 * Always returns 12320.
 */
uint32_t
gj_batch_id_12320(void)
{
	(void)NULL;
	return b12320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12320(void)
    __attribute__((alias("gj_batch_id_12320")));
