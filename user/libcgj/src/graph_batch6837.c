/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6837: pidfd_open flags OR merge.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pidfd_open_flags_or_6837(uint32_t a, uint32_t b);
 *     - Return a | b (soft merge of two pidfd_open flags masks).
 *   uint32_t __gj_pidfd_open_flags_or_6837  (alias)
 *   __libcgj_batch6837_marker = "libcgj-batch6837"
 *
 * Exclusive continuum CREATE-ONLY (6831-6840: pidfd open flags stubs).
 * Unique gj_pidfd_open_flags_or_6837 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6837_marker[] = "libcgj-batch6837";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6837_or(uint32_t u32A, uint32_t u32B)
{
	return u32A | u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pidfd_open_flags_or_6837 - bitwise OR of two pidfd_open flags masks.
 *
 * a, b: soft pidfd_open flags bitmasks
 *
 * Returns a | b. Soft pure-data merge (e.g. want = nonblock | thread
 * bookkeeping); no pidfd_open.
 */
uint32_t
gj_pidfd_open_flags_or_6837(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b6837_or(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pidfd_open_flags_or_6837(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_pidfd_open_flags_or_6837")));
