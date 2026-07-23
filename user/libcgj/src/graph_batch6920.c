/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6920: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6920(void);
 *     - Returns the compile-time graph batch number for this TU (6920).
 *   uint32_t __gj_batch_id_6920  (alias)
 *   __libcgj_batch6920_marker = "libcgj-batch6920"
 *
 * Exclusive continuum CREATE-ONLY (6911-6920: renameat2 flags stubs —
 * noreplace, exchange, whiteout, any, mask, conflict, valid,
 * noreplace_ok, legal_combo, batch_id_6920). Does NOT redefine gj_batch_id
 * / gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6920_marker[] = "libcgj-batch6920";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6920_id(void)
{
	return 6920u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6920 - report this TU's graph batch number.
 *
 * Always returns 6920. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6920(void)
{
	(void)NULL;
	return b6920_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6920(void)
    __attribute__((alias("gj_batch_id_6920")));
