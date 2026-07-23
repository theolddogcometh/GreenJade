/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12520: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12520(void);
 *     - Returns the compile-time graph batch number for this TU (12520).
 *   uint32_t __gj_batch_id_12520  (alias)
 *   __libcgj_batch12520_marker = "libcgj-batch12520"
 *
 * Exclusive continuum CREATE-ONLY (12511-12520: flatpak/portal soft id
 * stubs — flatpak_ok_u_12511, xdg_portal_ok_u_12512,
 * pipewire_portal_ok_u_12513, camera_portal_ok_u_12514,
 * file_portal_ok_u_12515, secret_portal_ok_u_12516,
 * notification_portal_ok_u_12517, settings_portal_ok_u_12518,
 * portal_soft_ready_u_12519, batch_id_12520). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_12320 (batch12320), gj_batch_id_12120 (batch12120), and
 * gj_batch_id_12020 (batch12020). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12520_marker[] = "libcgj-batch12520";

#define B12520_BATCH_ID  12520u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12520_id(void)
{
	return B12520_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12520 - report this TU's graph batch number.
 *
 * Always returns 12520.
 */
uint32_t
gj_batch_id_12520(void)
{
	(void)NULL;
	return b12520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12520(void)
    __attribute__((alias("gj_batch_id_12520")));
