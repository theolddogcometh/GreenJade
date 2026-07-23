/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7029: rseq flags soft validation.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_flags_ok_7029(uint32_t flags);
 *     - Return 1 if flags is 0 (register) or 1 (unregister only).
 *   uint32_t gj_rseq_is_unregister_7029(uint32_t flags);
 *     - Return 1 if flags has the unregister bit set (bit 0).
 *   uint32_t __gj_rseq_flags_ok_7029  (alias)
 *   uint32_t __gj_rseq_is_unregister_7029  (alias)
 *   __libcgj_batch7029_marker = "libcgj-batch7029"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs).
 * Unique gj_rseq_*_7029 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7029_marker[] = "libcgj-batch7029";

#define B7029_FLAG_UNREGISTER  1u
/* Soft: only bit 0 is defined for rseq(2) flags. */
#define B7029_FLAGS_MASK       1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7029_flags_ok(uint32_t u32Flags)
{
	/* Accept only zero or known bits; reject unknown high bits. */
	if ((u32Flags & ~B7029_FLAGS_MASK) != 0u) {
		return 0u;
	}
	return 1u;
}

static uint32_t
b7029_is_unregister(uint32_t u32Flags)
{
	return ((u32Flags & B7029_FLAG_UNREGISTER) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_flags_ok_7029 - soft rseq(2) flags validity check.
 *
 * flags: candidate rseq(2) flags word
 *
 * Returns 1 when only the defined bit (unregister) or none is set;
 * else 0. Soft pure-data gate; no rseq syscall. No parent wires.
 */
uint32_t
gj_rseq_flags_ok_7029(uint32_t u32Flags)
{
	(void)NULL;
	return b7029_flags_ok(u32Flags);
}

/*
 * gj_rseq_is_unregister_7029 - soft unregister intent predicate.
 *
 * flags: candidate rseq(2) flags word
 *
 * Returns 1 when bit 0 (RSEQ_FLAG_UNREGISTER) is set; else 0.
 * Does not validate unknown bits. No parent wires.
 */
uint32_t
gj_rseq_is_unregister_7029(uint32_t u32Flags)
{
	return b7029_is_unregister(u32Flags);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_rseq_flags_ok_7029(uint32_t u32Flags)
    __attribute__((alias("gj_rseq_flags_ok_7029")));

uint32_t __gj_rseq_is_unregister_7029(uint32_t u32Flags)
    __attribute__((alias("gj_rseq_is_unregister_7029")));
