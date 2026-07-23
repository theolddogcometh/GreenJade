/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7090: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7090(void);
 *     - Returns the compile-time graph batch number for this TU (7090).
 *   uint32_t __gj_batch_id_7090  (alias)
 *   __libcgj_batch7090_marker = "libcgj-batch7090"
 *
 * Exclusive continuum CREATE-ONLY (7081-7090: perf_event_open type stubs —
 * hardware, software, tracepoint, hw_cache, raw, breakpoint, type_ok,
 * type_max_id, type_errorish, batch_id_7090). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7090_marker[] = "libcgj-batch7090";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7090_id(void)
{
	return 7090u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7090 - report this TU's graph batch number.
 *
 * Always returns 7090. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7090(void)
{
	(void)NULL;
	return b7090_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7090(void)
    __attribute__((alias("gj_batch_id_7090")));
