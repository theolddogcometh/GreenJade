/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8560: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8560(void);
 *     - Returns the compile-time graph batch number for this TU (8560).
 *   uint32_t __gj_batch_id_8560  (alias)
 *   __libcgj_batch8560_marker = "libcgj-batch8560"
 *
 * Exclusive continuum CREATE-ONLY (8551-8560: regex/class id stubs —
 * empty_ok, dot_id, star_id, plus_id, qmark_id, anchor_start_id,
 * anchor_end_id, class_ok, escape_ok, batch_id_8560). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No real regex engine. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8560_marker[] = "libcgj-batch8560";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8560_id(void)
{
	return 8560u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8560 - report this TU's graph batch number.
 *
 * Always returns 8560. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8560(void)
{
	(void)NULL;
	return b8560_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8560(void)
    __attribute__((alias("gj_batch_id_8560")));
