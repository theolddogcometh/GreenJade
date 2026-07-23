/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11820: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11820(void);
 *     - Returns the compile-time graph batch number for this TU (11820).
 *   uint32_t __gj_batch_id_11820  (alias)
 *   __libcgj_batch11820_marker = "libcgj-batch11820"
 *
 * Exclusive continuum CREATE-ONLY (11811-11820: flatpak/portal soft id
 * stubs — flatpak_ok_u_11811, xdg_portal_ok_u_11812,
 * pipewire_portal_ok_u_11813, camera_portal_ok_u_11814,
 * file_portal_ok_u_11815, secret_portal_ok_u_11816,
 * notification_portal_ok_u_11817, settings_portal_ok_u_11818,
 * portal_soft_ready_u_11819, batch_id_11820). Unique surfaces only; no
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

const char __libcgj_batch11820_marker[] = "libcgj-batch11820";

#define B11820_BATCH_ID  11820u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11820_id(void)
{
	return B11820_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11820 - report this TU's graph batch number.
 *
 * Always returns 11820.
 */
uint32_t
gj_batch_id_11820(void)
{
	(void)NULL;
	return b11820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11820(void)
    __attribute__((alias("gj_batch_id_11820")));
