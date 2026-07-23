/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1365: Welford state size / init hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_welford_init_hint(void);
 *     — Byte-size hint for an integer Welford triple (n, mean, m2)
 *       laid out as three uint64_t/int64_t fields (24 bytes). Use to
 *       size caller-owned storage before online_var / Welford updates.
 *   uint32_t __gj_welford_init_hint  (alias)
 *   __libcgj_batch1365_marker = "libcgj-batch1365"
 *
 * Distinct from gj_welford_init (batch309, double) and
 * gj_online_var_init (batch896). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1365_marker[] = "libcgj-batch1365";

/* n (u64) + mean (i64) + m2 (i64) */
#define B1365_STATE_BYTES 24u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_welford_init_hint — recommended byte size for integer Welford state.
 *
 * Always returns 24 (three 8-byte fields). Pure constant; no inputs.
 */
uint32_t
gj_welford_init_hint(void)
{
	return B1365_STATE_BYTES;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_welford_init_hint(void)
    __attribute__((alias("gj_welford_init_hint")));
