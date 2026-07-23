/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10420: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10420(void);
 *     - Returns the compile-time graph batch number for this TU (10420).
 *   uint32_t __gj_batch_id_10420  (alias)
 *   __libcgj_batch10420_marker = "libcgj-batch10420"
 *
 * Exclusive continuum CREATE-ONLY (10411-10420: flatpak/portal soft id
 * stubs — flatpak_ok_u_10411, xdg_portal_ok_u_10412,
 * pipewire_portal_ok_u_10413, camera_portal_ok_u_10414,
 * file_portal_ok_u_10415, secret_portal_ok_u_10416,
 * notification_portal_ok_u_10417, settings_portal_ok_u_10418,
 * portal_soft_ready_u_10419, batch_id_10420). Unique surfaces only; no
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

const char __libcgj_batch10420_marker[] = "libcgj-batch10420";

#define B10420_BATCH_ID  10420u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10420_id(void)
{
	return B10420_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10420 - report this TU's graph batch number.
 *
 * Always returns 10420.
 */
uint32_t
gj_batch_id_10420(void)
{
	(void)NULL;
	return b10420_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10420(void)
    __attribute__((alias("gj_batch_id_10420")));
