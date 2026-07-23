/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7088: perf_event_open PERF_TYPE_MAX id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_type_max_id_u_7088(void);
 *     - Returns the soft PERF_TYPE_MAX catalog id (6). Bound sentinel
 *       only; not a valid attr.type for open.
 *   uint32_t __gj_pe_type_max_id_u_7088  (alias)
 *   __libcgj_batch7088_marker = "libcgj-batch7088"
 *
 * Exclusive continuum CREATE-ONLY (7081-7090: perf_event_open type stubs —
 * hardware, software, tracepoint, hw_cache, raw, breakpoint, type_ok,
 * type_max_id, type_errorish, batch_id_7090). Unique
 * gj_pe_type_max_id_u_7088 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7088_marker[] = "libcgj-batch7088";

/* PERF_TYPE_MAX: exclusive upper bound for known types (value 6). */
#define B7088_PERF_TYPE_MAX ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7088_pe_type_max_id(void)
{
	return B7088_PERF_TYPE_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_type_max_id_u_7088 - soft PERF_TYPE_MAX catalog id.
 *
 * Always returns 6 (PERF_TYPE_MAX). Catalog sentinel for range checks;
 * does not call perf_event_open. No parent wires.
 */
uint32_t
gj_pe_type_max_id_u_7088(void)
{
	(void)NULL;
	return b7088_pe_type_max_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_type_max_id_u_7088(void)
    __attribute__((alias("gj_pe_type_max_id_u_7088")));
