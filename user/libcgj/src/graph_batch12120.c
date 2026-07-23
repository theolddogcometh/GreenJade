/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12120: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12120(void);
 *     - Returns the compile-time graph batch number for this TU (12120).
 *   uint32_t __gj_batch_id_12120  (alias)
 *   __libcgj_batch12120_marker = "libcgj-batch12120"
 *
 * Exclusive continuum CREATE-ONLY (12111-12120: flatpak/portal soft id
 * stubs — flatpak_ok_u_12111, xdg_portal_ok_u_12112,
 * pipewire_portal_ok_u_12113, camera_portal_ok_u_12114,
 * file_portal_ok_u_12115, secret_portal_ok_u_12116,
 * notification_portal_ok_u_12117, settings_portal_ok_u_12118,
 * portal_soft_ready_u_12119, batch_id_12120). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11820 (batch11820), gj_batch_id_11620 (batch11620), and
 * gj_batch_id_11420 (batch11420). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12120_marker[] = "libcgj-batch12120";

#define B12120_BATCH_ID  12120u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12120_id(void)
{
return B12120_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12120 - report this TU's graph batch number.
 *
 * Always returns 12120.
 */
uint32_t
gj_batch_id_12120(void)
{
(void)NULL;
return b12120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12120(void)
    __attribute__((alias("gj_batch_id_12120")));
