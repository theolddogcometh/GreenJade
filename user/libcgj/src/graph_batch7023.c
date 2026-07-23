/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7023: rseq unregister flag soft constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_flag_unregister_7023(void);
 *     - Returns 1 (soft RSEQ_FLAG_UNREGISTER = 1 << 0).
 *   uint32_t __gj_rseq_flag_unregister_7023  (alias)
 *   __libcgj_batch7023_marker = "libcgj-batch7023"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs).
 * Unique gj_rseq_flag_unregister_7023 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7023_marker[] = "libcgj-batch7023";

/* Soft: RSEQ_FLAG_UNREGISTER (1 << 0) for rseq(2) flags argument. */
#define B7023_FLAG_UNREGISTER  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7023_flag_unregister(void)
{
	return B7023_FLAG_UNREGISTER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_flag_unregister_7023 - soft RSEQ_FLAG_UNREGISTER bit.
 *
 * Always returns 1. Soft pure-data product tag for the rseq(2) flags
 * bit that unregisters a previously registered TLS rseq area. Flags 0
 * registers. Does not call rseq. No parent wires.
 */
uint32_t
gj_rseq_flag_unregister_7023(void)
{
	(void)NULL;
	return b7023_flag_unregister();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rseq_flag_unregister_7023(void)
    __attribute__((alias("gj_rseq_flag_unregister_7023")));
