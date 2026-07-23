/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13720: flatpak portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13720(void);
 *     - Returns the compile-time graph batch number for this TU (13720).
 *   uint32_t __gj_batch_id_13720  (alias)
 *   __libcgj_batch13720_marker = "libcgj-batch13720"
 *
 * Exclusive continuum CREATE-ONLY (13711-13720: flatpak portal soft id
 * stubs — flatpak_ok_u_13711, xdg_portal_ok_u_13712,
 * documents_ok_u_13713, file_chooser_ok_u_13714, open_uri_ok_u_13715,
 * screenshot_ok_u_13716, notification_ok_u_13717, secrets_ok_u_13718,
 * portal_soft_ready_u_13719, batch_id_13720→13720).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13520 / gj_batch_id_13320. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13720_marker[] = "libcgj-batch13720";

#define B13720_BATCH_ID  13720u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13720_id(void)
{
	return B13720_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13720 - report this TU's graph batch number.
 *
 * Always returns 13720. Closes the 13711-13720 flatpak portal soft wave.
 */
uint32_t
gj_batch_id_13720(void)
{
	(void)NULL;
	return b13720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13720(void)
    __attribute__((alias("gj_batch_id_13720")));
