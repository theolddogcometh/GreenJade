/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7989: atexit remaining handler slots stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atexit_remaining_u_7989(uint32_t used);
 *     - Return max-used when used <= 32, else 0.
 *   uint32_t __gj_atexit_remaining_u_7989  (alias)
 *   __libcgj_batch7989_marker = "libcgj-batch7989"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Unique gj_atexit_remaining_u_7989 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7989_marker[] = "libcgj-batch7989";

/* Soft atexit table capacity for remaining-slot math. */
#define B7989_ATEXIT_MAX  ((uint32_t)32u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7989_remaining(uint32_t u32Used)
{
	if (u32Used > B7989_ATEXIT_MAX)
		return 0u;
	return B7989_ATEXIT_MAX - u32Used;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atexit_remaining_u_7989 - soft free-slot count in atexit table.
 *
 * used: soft registered-handler count
 *
 * Returns 32-used when used <= 32, else 0 (overflow clamp). Pure
 * integer subtract; does not call atexit(3). No parent wires.
 */
uint32_t
gj_atexit_remaining_u_7989(uint32_t u32Used)
{
	(void)NULL;
	return b7989_remaining(u32Used);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atexit_remaining_u_7989(uint32_t u32Used)
    __attribute__((alias("gj_atexit_remaining_u_7989")));
