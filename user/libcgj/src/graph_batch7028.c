/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7028: rseq_len validation soft predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_len_ok_7028(uint32_t len);
 *     - Return 1 if len >= 32 and len is a multiple of 4 (soft ABI ok).
 *   uint32_t __gj_rseq_len_ok_7028  (alias)
 *   __libcgj_batch7028_marker = "libcgj-batch7028"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs).
 * Unique gj_rseq_len_ok_7028 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7028_marker[] = "libcgj-batch7028";

/* Soft minimum matches original sizeof(struct rseq). */
#define B7028_LEN_MIN  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7028_len_ok(uint32_t u32Len)
{
	if (u32Len < B7028_LEN_MIN) {
		return 0u;
	}
	/* Soft: length is a multiple of 4 (uint32 field granularity). */
	if ((u32Len & 3u) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_len_ok_7028 - soft rseq_len ABI validity check.
 *
 * len: candidate rseq_len for rseq(2)
 *
 * Returns 1 when len >= 32 and len % 4 == 0; else 0. Soft pure-data
 * model of a minimal rseq structure length gate. Does not call rseq.
 * No parent wires.
 */
uint32_t
gj_rseq_len_ok_7028(uint32_t u32Len)
{
	(void)NULL;
	return b7028_len_ok(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rseq_len_ok_7028(uint32_t u32Len)
    __attribute__((alias("gj_rseq_len_ok_7028")));
