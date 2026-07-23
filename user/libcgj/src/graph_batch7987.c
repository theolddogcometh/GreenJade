/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7987: atexit slots full stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atexit_slots_full_u_7987(uint32_t used);
 *     - Return 1 if used >= 32 (table full), else 0.
 *   uint32_t __gj_atexit_slots_full_u_7987  (alias)
 *   __libcgj_batch7987_marker = "libcgj-batch7987"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Unique gj_atexit_slots_full_u_7987 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7987_marker[] = "libcgj-batch7987";

/* Soft atexit table capacity for full checks. */
#define B7987_ATEXIT_MAX  ((uint32_t)32u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7987_slots_full(uint32_t u32Used)
{
	return (u32Used >= B7987_ATEXIT_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atexit_slots_full_u_7987 - 1 if atexit table is at capacity.
 *
 * used: soft registered-handler count
 *
 * Returns 1 when used >= 32, else 0. Inverse of slots_ok; does not call
 * atexit(3). No parent wires.
 */
uint32_t
gj_atexit_slots_full_u_7987(uint32_t u32Used)
{
	(void)NULL;
	return b7987_slots_full(u32Used);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atexit_slots_full_u_7987(uint32_t u32Used)
    __attribute__((alias("gj_atexit_slots_full_u_7987")));
