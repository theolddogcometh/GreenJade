/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4617: dense index clamp into [0, n) (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_idx_clamp_u(size_t i, size_t n);
 *     - Clamp i into the last valid index of a length-n span:
 *       n > 0 → min(i, n - 1); n == 0 → 0.
 *   size_t __gj_idx_clamp_u  (alias)
 *   __libcgj_batch4617_marker = "libcgj-batch4617"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Unique gj_idx_clamp_u
 * surface only; no multi-def. Distinct from gj_u32_clamp01_u
 * (batch4001) and gj_u32_clamp_u (batch2703). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4617_marker[] = "libcgj-batch4617";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4617_idx_clamp(size_t szI, size_t szN)
{
	size_t szLast;

	if (szN == (size_t)0u) {
		return (size_t)0u;
	}
	szLast = szN - (size_t)1u;
	if (szI < szLast) {
		return szI;
	}
	return szLast;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_idx_clamp_u - clamp a dense index into the last valid slot of [0, n).
 *
 * i: index to clamp
 * n: length / exclusive upper bound
 *
 * Returns min(i, n - 1) when n > 0, else 0. No parent wires.
 */
size_t
gj_idx_clamp_u(size_t szI, size_t szN)
{
	(void)NULL;
	return b4617_idx_clamp(szI, szN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_idx_clamp_u(size_t szI, size_t szN)
    __attribute__((alias("gj_idx_clamp_u")));
