/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6730: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6730(void);
 *     - Returns the compile-time graph batch number for this TU (6730).
 *   uint32_t __gj_batch_id_6730  (alias)
 *   __libcgj_batch6730_marker = "libcgj-batch6730"
 *
 * Exclusive continuum CREATE-ONLY (6721-6730: clone flags stubs —
 * vm, fs, files, sighand, thread, sysvsem, settls, parent_settid,
 * threadish, batch_id_6730). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6730_marker[] = "libcgj-batch6730";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6730_id(void)
{
	return 6730u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6730 - report this TU's graph batch number.
 *
 * Always returns 6730. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6730(void)
{
	(void)NULL;
	return b6730_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6730(void)
    __attribute__((alias("gj_batch_id_6730")));
