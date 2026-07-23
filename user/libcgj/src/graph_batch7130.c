/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7130: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7130(void);
 *     - Returns the compile-time graph batch number for this TU (7130).
 *   uint32_t __gj_batch_id_7130  (alias)
 *   __libcgj_batch7130_marker = "libcgj-batch7130"
 *
 * Exclusive continuum CREATE-ONLY (7121-7130: timer_create clock stubs —
 * realtime, monotonic, process_cputime, thread_cputime, monotonic_raw,
 * boottime, timer_abstime, sigev_none, clk_ok, batch_id_7130). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7130_marker[] = "libcgj-batch7130";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7130_id(void)
{
	return 7130u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7130 - report this TU's graph batch number.
 *
 * Always returns 7130. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7130(void)
{
	(void)NULL;
	return b7130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7130(void)
    __attribute__((alias("gj_batch_id_7130")));
