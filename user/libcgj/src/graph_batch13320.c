/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13320: flatpak portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13320(void);
 *     - Returns the compile-time graph batch number for this TU (13320).
 *   uint32_t __gj_batch_id_13320  (alias)
 *   __libcgj_batch13320_marker = "libcgj-batch13320"
 *
 * Exclusive continuum CREATE-ONLY (13311-13320: flatpak portal soft id
 * stubs — flatpak_ok_u_13311, xdg_portal_ok_u_13312,
 * documents_ok_u_13313, file_chooser_ok_u_13314, open_uri_ok_u_13315,
 * screenshot_ok_u_13316, notification_ok_u_13317, secrets_ok_u_13318,
 * portal_soft_ready_u_13319, batch_id_13320).
 * Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_13190 (batch13190),
 * gj_batch_id_13290 (batch13290), and gj_batch_id_13300 (batch13300).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13320_marker[] = "libcgj-batch13320";

#define B13320_BATCH_ID  13320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13320_id(void)
{
	return B13320_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13320 - report this TU's graph batch number.
 *
 * Always returns 13320.
 */
uint32_t
gj_batch_id_13320(void)
{
	(void)NULL;
	return b13320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13320(void)
    __attribute__((alias("gj_batch_id_13320")));
