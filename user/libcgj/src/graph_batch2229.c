/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2229: linear open-address probe step.
 *
 * Surface (unique symbols):
 *   size_t gj_probe_linear_next(size_t i, size_t cap);
 *     - Return the next slot index after i in a linear probe sequence
 *       of length cap: (i + 1) mod cap. cap == 0 yields 0. i is not
 *       required to be < cap; the sum is reduced modulo cap when cap
 *       is non-zero.
 *   size_t __gj_probe_linear_next  (alias)
 *   __libcgj_batch2229_marker = "libcgj-batch2229"
 *
 * Post-2220 table exclusive wave (2221-2230). Unique
 * gj_probe_linear_next surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2229_marker[] = "libcgj-batch2229";

/* ---- freestanding helpers ---------------------------------------------- */

/* Next linear-probe index, or 0 when cap is 0. */
static size_t
b2229_next(size_t i, size_t cCap)
{
	if (cCap == 0u) {
		return 0u;
	}
	return (i + 1u) % cCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_probe_linear_next - advance one step in a linear open-address probe.
 *
 * i:   current slot index
 * cap: table capacity (0 is treated as empty -> return 0)
 *
 * Returns (i + 1) mod cap when cap > 0, else 0. Does not call libc.
 */
size_t
gj_probe_linear_next(size_t i, size_t cap)
{
	(void)NULL;
	return b2229_next(i, cap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_probe_linear_next(size_t i, size_t cap)
    __attribute__((alias("gj_probe_linear_next")));
