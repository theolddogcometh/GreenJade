/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7850: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7850(void);
 *     - Returns the compile-time graph batch number for this TU (7850).
 *   uint32_t __gj_batch_id_7850  (alias)
 *   __libcgj_batch7850_marker = "libcgj-batch7850"
 *
 * Exclusive continuum CREATE-ONLY (7841-7850: regexec eflags stubs —
 * notbol_id, noteol_id, startend_id, has_notbol, has_noteol,
 * has_startend, eflags_ok, eflags_pack, eflags_errorish, batch_id_7850).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7850_marker[] = "libcgj-batch7850";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7850_id(void)
{
	return 7850u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7850 - report this TU's graph batch number.
 *
 * Always returns 7850. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7850(void)
{
	(void)NULL;
	return b7850_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7850(void)
    __attribute__((alias("gj_batch_id_7850")));
