/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13920: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13920(void);
 *     - Returns the compile-time graph batch number for this TU (13920).
 *   uint32_t __gj_batch_id_13920  (alias)
 *   __libcgj_batch13920_marker = "libcgj-batch13920"
 *
 * Exclusive continuum CREATE-ONLY (13911-13920: flatpak/portal soft id
 * stubs — flatpak_ok_u_13911, xdg_portal_ok_u_13912,
 * pipewire_portal_ok_u_13913, camera_portal_ok_u_13914,
 * file_portal_ok_u_13915, secret_portal_ok_u_13916,
 * notification_portal_ok_u_13917, settings_portal_ok_u_13918,
 * portal_soft_ready_u_13919, batch_id_13920). Unique surfaces only; no
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

const char __libcgj_batch13920_marker[] = "libcgj-batch13920";

#define B13920_BATCH_ID  13920u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13920_id(void)
{
	return B13920_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13920 - report this TU's graph batch number.
 *
 * Always returns 13920.
 */
uint32_t
gj_batch_id_13920(void)
{
	(void)NULL;
	return b13920_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13920(void)
    __attribute__((alias("gj_batch_id_13920")));
