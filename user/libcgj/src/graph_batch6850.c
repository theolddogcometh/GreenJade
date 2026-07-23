/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6850: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6850(void);
 *     - Returns the compile-time graph batch number for this TU (6850).
 *   uint32_t __gj_batch_id_6850  (alias)
 *   __libcgj_batch6850_marker = "libcgj-batch6850"
 *
 * Exclusive continuum CREATE-ONLY (6841-6850: close_range flags stubs —
 * unshare, cloexec, has_unshare, has_cloexec, known_mask, has_any,
 * flags_ok, plain, both, batch_id_6850). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6850_marker[] = "libcgj-batch6850";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6850_id(void)
{
	return 6850u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6850 - report this TU's graph batch number.
 *
 * Always returns 6850. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6850(void)
{
	(void)NULL;
	return b6850_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6850(void)
    __attribute__((alias("gj_batch_id_6850")));
