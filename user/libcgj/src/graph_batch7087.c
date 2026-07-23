/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7087: perf_event_open type validity soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_type_ok_u_7087(uint32_t pe_type);
 *     - Return 1 if pe_type is a known perf type in [0, 5]
 *       (HARDWARE..BREAKPOINT), else 0.
 *   uint32_t __gj_pe_type_ok_u_7087  (alias)
 *   __libcgj_batch7087_marker = "libcgj-batch7087"
 *
 * Exclusive continuum CREATE-ONLY (7081-7090: perf_event_open type stubs —
 * hardware, software, tracepoint, hw_cache, raw, breakpoint, type_ok,
 * type_max_id, type_errorish, batch_id_7090). Unique
 * gj_pe_type_ok_u_7087 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7087_marker[] = "libcgj-batch7087";

/* Known perf types: HARDWARE=0 .. BREAKPOINT=5 (PERF_TYPE_MAX=6). */
#define B7087_PERF_TYPE_MAX_IDX ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7087_pe_type_ok(uint32_t u32Type)
{
	return (u32Type <= B7087_PERF_TYPE_MAX_IDX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_type_ok_u_7087 - 1 if pe_type is a known perf_event type.
 *
 * pe_type: perf_event_attr.type soft catalog id
 *
 * Returns 1 for types in [0, 5] (HARDWARE..BREAKPOINT), else 0.
 * Soft range check; does not call perf_event_open. No parent wires.
 */
uint32_t
gj_pe_type_ok_u_7087(uint32_t u32PeType)
{
	(void)NULL;
	return b7087_pe_type_ok(u32PeType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_type_ok_u_7087(uint32_t u32PeType)
    __attribute__((alias("gj_pe_type_ok_u_7087")));
