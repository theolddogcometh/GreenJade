/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7986: atexit slots remaining room stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atexit_slots_ok_u_7986(uint32_t used);
 *     - Return 1 if used < 32 (room for another handler), else 0.
 *   uint32_t __gj_atexit_slots_ok_u_7986  (alias)
 *   __libcgj_batch7986_marker = "libcgj-batch7986"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Unique gj_atexit_slots_ok_u_7986 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7986_marker[] = "libcgj-batch7986";

/* Soft atexit table capacity for room checks. */
#define B7986_ATEXIT_MAX  ((uint32_t)32u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7986_slots_ok(uint32_t u32Used)
{
	return (u32Used < B7986_ATEXIT_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atexit_slots_ok_u_7986 - 1 if another atexit handler can be stored.
 *
 * used: soft registered-handler count
 *
 * Returns 1 when used < 32, else 0. Pure count gate; does not call
 * atexit(3). No parent wires.
 */
uint32_t
gj_atexit_slots_ok_u_7986(uint32_t u32Used)
{
	(void)NULL;
	return b7986_slots_ok(u32Used);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atexit_slots_ok_u_7986(uint32_t u32Used)
    __attribute__((alias("gj_atexit_slots_ok_u_7986")));
