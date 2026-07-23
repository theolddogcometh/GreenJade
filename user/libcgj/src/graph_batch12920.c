/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12920: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12920(void);
 *     - Returns the compile-time graph batch number for this TU (12920).
 *   uint32_t __gj_batch_id_12920  (alias)
 *   __libcgj_batch12920_marker = "libcgj-batch12920"
 *
 * Exclusive continuum CREATE-ONLY (12911-12920: flatpak/portal soft id
 * stubs — flatpak_ok_u_12911, xdg_portal_ok_u_12912,
 * pipewire_portal_ok_u_12913, camera_portal_ok_u_12914,
 * file_portal_ok_u_12915, secret_portal_ok_u_12916,
 * notification_portal_ok_u_12917, settings_portal_ok_u_12918,
 * portal_soft_ready_u_12919, batch_id_12920). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_12720 (batch12720), gj_batch_id_12520 (batch12520),
 * gj_batch_id_12320 (batch12320), and gj_batch_id_12120 (batch12120).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12920_marker[] = "libcgj-batch12920";

#define B12920_BATCH_ID  12920u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12920_id(void)
{
	return B12920_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12920 - report this TU's graph batch number.
 *
 * Always returns 12920.
 */
uint32_t
gj_batch_id_12920(void)
{
	(void)NULL;
	return b12920_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12920(void)
    __attribute__((alias("gj_batch_id_12920")));
