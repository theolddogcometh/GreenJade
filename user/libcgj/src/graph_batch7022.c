/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7022: rseq registration length minimum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_len_min_7022(void);
 *     - Returns 32 (soft original sizeof(struct rseq) ABI length).
 *   uint32_t __gj_rseq_len_min_7022  (alias)
 *   __libcgj_batch7022_marker = "libcgj-batch7022"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs).
 * Unique gj_rseq_len_min_7022 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7022_marker[] = "libcgj-batch7022";

/* Soft: original kernel ABI rseq structure size (cpu_id_start..flags). */
#define B7022_LEN_MIN  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7022_len_min(void)
{
	return B7022_LEN_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_len_min_7022 - minimum rseq_len for registration.
 *
 * Always returns 32. Soft pure-data layout tag for the original
 * sizeof(struct rseq) passed as rseq_len to rseq(2). Does not call
 * rseq. No parent wires.
 */
uint32_t
gj_rseq_len_min_7022(void)
{
	(void)NULL;
	return b7022_len_min();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rseq_len_min_7022(void)
    __attribute__((alias("gj_rseq_len_min_7022")));
