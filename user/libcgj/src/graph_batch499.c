/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch499: freestanding stream min/max tracker.
 *
 * Surface (unique symbols):
 *   void gj_stream_mm_init(int64_t *minv, int64_t *maxv, int *init);
 *     — Reset stream state: *init = 0. minv/maxv left untouched until first
 *       sample (or zeroed when non-NULL for a clean start).
 *   void gj_stream_mm_add(int64_t *minv, int64_t *maxv, int *init, int64_t x);
 *     — Fold sample x into running min/max. First sample (*init == 0)
 *       seeds both extremes and sets *init = 1; later samples only widen.
 *   void __gj_stream_mm_init  (alias)
 *   void __gj_stream_mm_add   (alias)
 *   __libcgj_batch499_marker = "libcgj-batch499"
 *
 * Tracks min and max over the entire stream (not a sliding window or last
 * sample only). Median would need storage; this API is O(1) state only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. NULL pointer args are no-ops.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch499_marker[] = "libcgj-batch499";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stream_mm_init — clear the "have sample" flag for a min/max stream.
 *
 * When minv/maxv are non-NULL they are zeroed so callers see a deterministic
 * idle state before any add. *init must be non-NULL to take effect.
 */
void
gj_stream_mm_init(int64_t *minv, int64_t *maxv, int *init)
{
	if (init == NULL) {
		return;
	}
	*init = 0;
	if (minv != NULL) {
		*minv = 0;
	}
	if (maxv != NULL) {
		*maxv = 0;
	}
}

/*
 * gj_stream_mm_add — update running min/max with sample x.
 *
 * Requires non-NULL minv, maxv, and init. If *init is 0, seed minv and maxv
 * to x and mark initialized. Otherwise tighten only when x is a new extreme.
 */
void
gj_stream_mm_add(int64_t *minv, int64_t *maxv, int *init, int64_t x)
{
	if (minv == NULL || maxv == NULL || init == NULL) {
		return;
	}
	if (*init == 0) {
		*minv = x;
		*maxv = x;
		*init = 1;
		return;
	}
	if (x < *minv) {
		*minv = x;
	}
	if (x > *maxv) {
		*maxv = x;
	}
}

/* ---- underscored aliases ----------------------------------------------- */

void __gj_stream_mm_init(int64_t *minv, int64_t *maxv, int *init)
    __attribute__((alias("gj_stream_mm_init")));

void __gj_stream_mm_add(int64_t *minv, int64_t *maxv, int *init, int64_t x)
    __attribute__((alias("gj_stream_mm_add")));
