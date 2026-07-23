/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7027: rseq signature match predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_sig_match_7027(uint32_t got, uint32_t expect);
 *     - Return 1 if got == expect (soft abort-signature match).
 *   uint32_t __gj_rseq_sig_match_7027  (alias)
 *   __libcgj_batch7027_marker = "libcgj-batch7027"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs).
 * Unique gj_rseq_sig_match_7027 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7027_marker[] = "libcgj-batch7027";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7027_match(uint32_t u32Got, uint32_t u32Expect)
{
	return (u32Got == u32Expect) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_sig_match_7027 - soft rseq signature equality check.
 *
 * got:    candidate 32-bit signature (e.g. from a syscall arg)
 * expect: expected RSEQ_SIG-style constant (e.g. 0x53053053)
 *
 * Returns 1 when got == expect; else 0. Soft pure-data model of the
 * kernel matching the rseq(2) sig argument against the abort IP word.
 * Does not call rseq. No parent wires.
 */
uint32_t
gj_rseq_sig_match_7027(uint32_t u32Got, uint32_t u32Expect)
{
	(void)NULL;
	return b7027_match(u32Got, u32Expect);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rseq_sig_match_7027(uint32_t u32Got, uint32_t u32Expect)
    __attribute__((alias("gj_rseq_sig_match_7027")));
