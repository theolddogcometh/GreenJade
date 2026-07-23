/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12720: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12720(void);
 *     - Returns the compile-time graph batch number for this TU (12720).
 *   uint32_t __gj_batch_id_12720  (alias)
 *   __libcgj_batch12720_marker = "libcgj-batch12720"
 *
 * Exclusive continuum CREATE-ONLY (12711-12720: flatpak/portal soft id
 * stubs — flatpak_ok_u_12711, xdg_portal_ok_u_12712,
 * pipewire_portal_ok_u_12713, camera_portal_ok_u_12714,
 * file_portal_ok_u_12715, secret_portal_ok_u_12716,
 * notification_portal_ok_u_12717, settings_portal_ok_u_12718,
 * portal_soft_ready_u_12719, batch_id_12720). Unique surfaces only; no
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

const char __libcgj_batch12720_marker[] = "libcgj-batch12720";

#define B12720_BATCH_ID  12720u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12720_id(void)
{
	return B12720_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12720 - report this TU's graph batch number.
 *
 * Always returns 12720.
 */
uint32_t
gj_batch_id_12720(void)
{
	(void)NULL;
	return b12720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12720(void)
    __attribute__((alias("gj_batch_id_12720")));
