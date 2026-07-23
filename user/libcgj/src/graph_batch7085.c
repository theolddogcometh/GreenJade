/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7085: perf_event_open PERF_TYPE_RAW type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_type_raw_u_7085(uint32_t pe_type);
 *     - Return 1 if pe_type is PERF_TYPE_RAW (4), else 0.
 *   uint32_t __gj_pe_type_raw_u_7085  (alias)
 *   __libcgj_batch7085_marker = "libcgj-batch7085"
 *
 * Exclusive continuum CREATE-ONLY (7081-7090: perf_event_open type stubs —
 * hardware, software, tracepoint, hw_cache, raw, breakpoint, type_ok,
 * type_max_id, type_errorish, batch_id_7090). Unique
 * gj_pe_type_raw_u_7085 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7085_marker[] = "libcgj-batch7085";

/* PERF_TYPE_RAW: raw PMU event encoding (value 4). */
#define B7085_PERF_TYPE_RAW ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7085_pe_type_raw(uint32_t u32Type)
{
	return (u32Type == B7085_PERF_TYPE_RAW) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_type_raw_u_7085 - 1 if pe_type is PERF_TYPE_RAW.
 *
 * pe_type: perf_event_attr.type soft catalog id
 *
 * Returns 1 when pe_type equals PERF_TYPE_RAW (4), else 0.
 * Pure integer compare; does not call perf_event_open. No parent wires.
 */
uint32_t
gj_pe_type_raw_u_7085(uint32_t u32PeType)
{
	(void)NULL;
	return b7085_pe_type_raw(u32PeType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_type_raw_u_7085(uint32_t u32PeType)
    __attribute__((alias("gj_pe_type_raw_u_7085")));
