/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13520: flatpak portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13520(void);
 *     - Returns the compile-time graph batch number for this TU (13520).
 *   uint32_t __gj_batch_id_13520  (alias)
 *   __libcgj_batch13520_marker = "libcgj-batch13520"
 *
 * Exclusive continuum CREATE-ONLY (13511-13520: flatpak portal soft id
 * stubs — flatpak_ok_u_13511, xdg_portal_ok_u_13512,
 * documents_ok_u_13513, file_chooser_ok_u_13514, open_uri_ok_u_13515,
 * screenshot_ok_u_13516, notification_ok_u_13517, secrets_ok_u_13518,
 * portal_soft_ready_u_13519, batch_id_13520→13520).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13320 / gj_batch_id_13120. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13520_marker[] = "libcgj-batch13520";

#define B13520_BATCH_ID  13520u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13520_id(void)
{
	return B13520_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13520 - report this TU's graph batch number.
 *
 * Always returns 13520.
 */
uint32_t
gj_batch_id_13520(void)
{
	(void)NULL;
	return b13520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13520(void)
    __attribute__((alias("gj_batch_id_13520")));
