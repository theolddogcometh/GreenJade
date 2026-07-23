/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14220: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14220(void);
 *     - Returns the compile-time graph batch number for this TU (14220).
 *   uint32_t __gj_batch_id_14220  (alias)
 *   __libcgj_batch14220_marker = "libcgj-batch14220"
 *
 * Exclusive continuum CREATE-ONLY (14211-14220: flatpak/portal soft id
 * stubs — flatpak_ok_u_14211, xdg_portal_ok_u_14212,
 * pipewire_portal_ok_u_14213, camera_portal_ok_u_14214,
 * file_portal_ok_u_14215, secret_portal_ok_u_14216,
 * notification_portal_ok_u_14217, settings_portal_ok_u_14218,
 * portal_soft_ready_u_14219, batch_id_14220). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_13920 (batch13920). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14220_marker[] = "libcgj-batch14220";

#define B14220_BATCH_ID  14220u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14220_id(void)
{
	return B14220_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14220 - report this TU's graph batch number.
 *
 * Always returns 14220.
 */
uint32_t
gj_batch_id_14220(void)
{
	(void)NULL;
	return b14220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14220(void)
    __attribute__((alias("gj_batch_id_14220")));
