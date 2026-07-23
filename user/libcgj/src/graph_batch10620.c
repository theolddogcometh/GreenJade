/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10620: flatpak/portal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10620(void);
 *     - Returns the compile-time graph batch number for this TU (10620).
 *   uint32_t __gj_batch_id_10620  (alias)
 *   __libcgj_batch10620_marker = "libcgj-batch10620"
 *
 * Exclusive continuum CREATE-ONLY (10611-10620: flatpak/portal soft id
 * stubs — flatpak_ok_u_10611, xdg_portal_ok_u_10612,
 * pipewire_portal_ok_u_10613, camera_portal_ok_u_10614,
 * file_portal_ok_u_10615, secret_portal_ok_u_10616,
 * notification_portal_ok_u_10617, settings_portal_ok_u_10618,
 * portal_soft_ready_u_10619, batch_id_10620). Unique surfaces only; no
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

const char __libcgj_batch10620_marker[] = "libcgj-batch10620";

#define B10620_BATCH_ID  10620u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10620_id(void)
{
	return B10620_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10620 - report this TU's graph batch number.
 *
 * Always returns 10620.
 */
uint32_t
gj_batch_id_10620(void)
{
	(void)NULL;
	return b10620_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10620(void)
    __attribute__((alias("gj_batch_id_10620")));
