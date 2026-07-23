/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7830: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7830(void);
 *     - Returns the compile-time graph batch number for this TU (7830).
 *   uint32_t __gj_batch_id_7830  (alias)
 *   __libcgj_batch7830_marker = "libcgj-batch7830"
 *
 * Exclusive continuum CREATE-ONLY (7821-7830: wordexp flags stubs —
 * dooffs_id, append_id, nocmd_id, has_dooffs, has_append, has_nocmd,
 * flags_ok, flags_pack, flags_errorish, batch_id_7830).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7830_marker[] = "libcgj-batch7830";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7830_id(void)
{
	return 7830u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7830 - report this TU's graph batch number.
 *
 * Always returns 7830. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7830(void)
{
	(void)NULL;
	return b7830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7830(void)
    __attribute__((alias("gj_batch_id_7830")));
