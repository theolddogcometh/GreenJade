/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4838: false-sharing risk predicate (same line).
 *
 * Surface (unique symbols):
 *   int gj_false_sharing_risk_u(uintptr_t a, uintptr_t b);
 *     - Return 1 when a and b fall in the same 64-byte cache line,
 *       else 0. Distinct addresses on one line risk false sharing.
 *   int __gj_false_sharing_risk_u  (alias)
 *   __libcgj_batch4838_marker = "libcgj-batch4838"
 *
 * Exclusive continuum CREATE-ONLY (4831-4840: cacheline_size_u,
 * cacheline_align_u, cacheline_index_u, cacheline_offset_u,
 * cacheline_span_u, prefetch_hint_read_u, prefetch_hint_write_u,
 * false_sharing_risk_u, pad_to_line_u, batch_id_4840). Unique
 * gj_false_sharing_risk_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4838_marker[] = "libcgj-batch4838";

/* Assumed CPU cache-line width (x86_64 / common modern default). */
#define B4838_CACHELINE  64u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4838_false_sharing_risk(uintptr_t uA, uintptr_t uB)
{
	uintptr_t uMask = (uintptr_t)B4838_CACHELINE - 1u;

	return ((uA & ~uMask) == (uB & ~uMask)) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_false_sharing_risk_u - true when a and b share a cache line.
 *
 * a, b: addresses as integers (may be equal).
 * Returns 1 if (a & ~63) == (b & ~63), else 0. No parent wires.
 */
int
gj_false_sharing_risk_u(uintptr_t uA, uintptr_t uB)
{
	(void)NULL;
	return b4838_false_sharing_risk(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_false_sharing_risk_u(uintptr_t uA, uintptr_t uB)
    __attribute__((alias("gj_false_sharing_risk_u")));
