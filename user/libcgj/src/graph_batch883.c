/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch883: cooperative SPSC try-write for uint64_t.
 *
 * Free-running write/read indices over a power-of-two capacity buffer.
 * Plain sequential (no atomics) — single producer / single consumer or
 * cooperative use only. Distinct from gj_u64ring_push (batch185).
 *
 * Surface (unique symbols):
 *   int gj_spsc_try_write_u64(uint64_t *buf, size_t cap, size_t *w,
 *                             size_t *r, uint64_t v);
 *     — Producer: enqueue v if (w - r) < cap. Returns 0 / -1.
 *   int __gj_spsc_try_write_u64  (alias)
 *   __libcgj_batch883_marker = "libcgj-batch883"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch883_marker[] = "libcgj-batch883";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b883_is_pow2(size_t n)
{
	return n != 0u && (n & (n - 1u)) == 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spsc_try_write_u64 — non-blocking SPSC enqueue of one uint64_t.
 *
 * cap must be a power of two. Slot index is (*w & (cap - 1)).
 * Returns 0 on success, -1 if full or bad args.
 */
int
gj_spsc_try_write_u64(uint64_t *buf, size_t cap, size_t *w, size_t *r,
                      uint64_t v)
{
	size_t uW;
	size_t uR;
	size_t uUsed;

	if (buf == NULL || w == NULL || r == NULL || !b883_is_pow2(cap)) {
		return -1;
	}

	uW = *w;
	uR = *r;
	uUsed = uW - uR;
	if (uUsed >= cap) {
		return -1;
	}

	buf[uW & (cap - 1u)] = v;
	*w = uW + 1u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_spsc_try_write_u64(uint64_t *buf, size_t cap, size_t *w, size_t *r,
                            uint64_t v)
    __attribute__((alias("gj_spsc_try_write_u64")));
