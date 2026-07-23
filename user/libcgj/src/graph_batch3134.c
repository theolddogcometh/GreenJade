/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3134: milliseconds to whole seconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_ms_to_s_u(uint64_t ms);
 *     - Return ms / 1000 (floor division toward zero).
 *   uint64_t __gj_time_ms_to_s_u  (alias)
 *   __libcgj_batch3134_marker = "libcgj-batch3134"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140). Distinct from
 * gj_ms_to_secs (batch485) and gj_ms_to_sec_floor (batch1974) —
 * unique gj_time_ms_to_s_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3134_marker[] = "libcgj-batch3134";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3134_ms_to_s(uint64_t u64Ms)
{
	return u64Ms / 1000ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_ms_to_s_u - convert milliseconds to whole seconds (floor).
 *
 * u64Ms: milliseconds
 *
 * Integer divide by 1000; remainder discarded.
 */
uint64_t
gj_time_ms_to_s_u(uint64_t u64Ms)
{
	(void)NULL;
	return b3134_ms_to_s(u64Ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_ms_to_s_u(uint64_t u64Ms)
    __attribute__((alias("gj_time_ms_to_s_u")));
