/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7990: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7990(void);
 *     - Returns the compile-time graph batch number for this TU (7990).
 *   uint32_t __gj_batch_id_7990  (alias)
 *   __libcgj_batch7990_marker = "libcgj-batch7990"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7990_marker[] = "libcgj-batch7990";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7990_id(void)
{
	return 7990u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7990 - report this TU's graph batch number.
 *
 * Always returns 7990. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7990(void)
{
	(void)NULL;
	return b7990_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7990(void)
    __attribute__((alias("gj_batch_id_7990")));
