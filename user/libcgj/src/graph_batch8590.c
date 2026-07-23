/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8590: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8590(void);
 *     - Returns the compile-time graph batch number for this TU (8590).
 *   uint32_t __gj_batch_id_8590  (alias)
 *   __libcgj_batch8590_marker = "libcgj-batch8590"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Soft pure-data catalog
 * only; no setlocale. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8590_marker[] = "libcgj-batch8590";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8590_id(void)
{
	return 8590u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8590 - report this TU's graph batch number.
 *
 * Always returns 8590. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8590(void)
{
	(void)NULL;
	return b8590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8590(void)
    __attribute__((alias("gj_batch_id_8590")));
