/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7089: perf_event_open type errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_type_errorish_u_7089(uint32_t pe_type);
 *     - Return 1 if pe_type is unknown/invalid (not in [0, 5]), else 0.
 *   uint32_t __gj_pe_type_errorish_u_7089  (alias)
 *   __libcgj_batch7089_marker = "libcgj-batch7089"
 *
 * Exclusive continuum CREATE-ONLY (7081-7090: perf_event_open type stubs —
 * hardware, software, tracepoint, hw_cache, raw, breakpoint, type_ok,
 * type_max_id, type_errorish, batch_id_7090). Unique
 * gj_pe_type_errorish_u_7089 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7089_marker[] = "libcgj-batch7089";

/* Known perf types: HARDWARE=0 .. BREAKPOINT=5. */
#define B7089_PERF_TYPE_MAX_IDX ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7089_pe_type_errorish(uint32_t u32Type)
{
	return (u32Type > B7089_PERF_TYPE_MAX_IDX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_type_errorish_u_7089 - 1 if pe_type is unknown/invalid.
 *
 * pe_type: perf_event_attr.type soft catalog id
 *
 * Returns 1 when pe_type is outside the known set [0, 5], including
 * PERF_TYPE_MAX (6) and future/unknown values. Soft inverse of type_ok;
 * does not call perf_event_open. No parent wires.
 */
uint32_t
gj_pe_type_errorish_u_7089(uint32_t u32PeType)
{
	(void)NULL;
	return b7089_pe_type_errorish(u32PeType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_type_errorish_u_7089(uint32_t u32PeType)
    __attribute__((alias("gj_pe_type_errorish_u_7089")));
