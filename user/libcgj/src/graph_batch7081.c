/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7081: perf_event_open PERF_TYPE_HARDWARE type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_type_hardware_u_7081(uint32_t pe_type);
 *     - Return 1 if pe_type is PERF_TYPE_HARDWARE (0), else 0.
 *   uint32_t __gj_pe_type_hardware_u_7081  (alias)
 *   __libcgj_batch7081_marker = "libcgj-batch7081"
 *
 * Exclusive continuum CREATE-ONLY (7081-7090: perf_event_open type stubs —
 * hardware, software, tracepoint, hw_cache, raw, breakpoint, type_ok,
 * type_max_id, type_errorish, batch_id_7090). Unique
 * gj_pe_type_hardware_u_7081 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7081_marker[] = "libcgj-batch7081";

/* PERF_TYPE_HARDWARE: generalized hardware CPU events (value 0). */
#define B7081_PERF_TYPE_HARDWARE ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7081_pe_type_hardware(uint32_t u32Type)
{
	return (u32Type == B7081_PERF_TYPE_HARDWARE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_type_hardware_u_7081 - 1 if pe_type is PERF_TYPE_HARDWARE.
 *
 * pe_type: perf_event_attr.type soft catalog id
 *
 * Returns 1 when pe_type equals PERF_TYPE_HARDWARE (0), else 0.
 * Pure integer compare; does not call perf_event_open. No parent wires.
 */
uint32_t
gj_pe_type_hardware_u_7081(uint32_t u32PeType)
{
	(void)NULL;
	return b7081_pe_type_hardware(u32PeType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_type_hardware_u_7081(uint32_t u32PeType)
    __attribute__((alias("gj_pe_type_hardware_u_7081")));
