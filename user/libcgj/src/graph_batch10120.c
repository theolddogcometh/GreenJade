/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10120: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10120(void);
 *     - Returns the compile-time graph batch number for this TU (10120).
 *   uint32_t __gj_batch_id_10120  (alias)
 *   __libcgj_batch10120_marker = "libcgj-batch10120"
 *
 * Exclusive continuum CREATE-ONLY (10111-10120: flatpak/portal soft id
 * stubs — flatpak_ok_u_10111, xdg_portal_ok_u_10112,
 * pipewire_portal_ok_u_10113, camera_portal_ok_u_10114,
 * file_portal_ok_u_10115, secret_portal_ok_u_10116,
 * notification_portal_ok_u_10117, settings_portal_ok_u_10118,
 * portal_soft_ready_u_10119, batch_id_10120). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10120_marker[] = "libcgj-batch10120";

#define B10120_BATCH_ID  10120u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10120_id(void)
{
	return B10120_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10120 - report this TU's graph batch number.
 *
 * Always returns 10120.
 */
uint32_t
gj_batch_id_10120(void)
{
	(void)NULL;
	return b10120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10120(void)
    __attribute__((alias("gj_batch_id_10120")));
