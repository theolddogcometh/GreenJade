/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14020: flatpak/portal soft path + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14020(void);
 *     - Returns the compile-time graph batch number for this TU (14020).
 *   uint32_t __gj_batch_id_14020  (alias)
 *   __libcgj_batch14020_marker = "libcgj-batch14020"
 *
 * Exclusive continuum CREATE-ONLY (14011-14020: flatpak/portal soft
 * path deepen — flatpak_portal_soft_14011, flatpak_portal_doc_soft_14012,
 * flatpak_portal_file_soft_14013, flatpak_portal_openuri_soft_14014,
 * flatpak_portal_inhibit_soft_14015, flatpak_portal_notif_soft_14016,
 * flatpak_portal_screencast_soft_14017, flatpak_portal_secret_soft_14018,
 * flatpak_portal_ready_soft_14019, batch_id_14020). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14020_marker[] = "libcgj-batch14020";

#define B14020_BATCH_ID  14020u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14020_id(void)
{
	return B14020_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14020 - report this TU's graph batch number.
 *
 * Always returns 14020.
 */
uint32_t
gj_batch_id_14020(void)
{
	(void)NULL;
	return b14020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14020(void)
    __attribute__((alias("gj_batch_id_14020")));
