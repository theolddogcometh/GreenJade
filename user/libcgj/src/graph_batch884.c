/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch884: cooperative SPSC try-read for uint64_t.
 *
 * Pair with gj_spsc_try_write_u64 (batch883). Plain sequential (no atomics).
 * Distinct from gj_u64ring_pop (batch185) — no multi-def.
 *
 * Surface (unique symbols):
 *   int gj_spsc_try_read_u64(uint64_t *buf, size_t cap, size_t *w,
 *                            size_t *r, uint64_t *out);
 *     — Consumer: dequeue into *out if (w - r) > 0. Returns 0 / -1.
 *   int __gj_spsc_try_read_u64  (alias)
 *   __libcgj_batch884_marker = "libcgj-batch884"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch884_marker[] = "libcgj-batch884";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b884_is_pow2(size_t n)
{
	return n != 0u && (n & (n - 1u)) == 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spsc_try_read_u64 — non-blocking SPSC dequeue of one uint64_t.
 *
 * cap must be a power of two. Slot index is (*r & (cap - 1)).
 * Returns 0 on success (writes *out), -1 if empty or bad args.
 * *out is left untouched on failure.
 */
int
gj_spsc_try_read_u64(uint64_t *buf, size_t cap, size_t *w, size_t *r,
                     uint64_t *out)
{
	size_t uW;
	size_t uR;

	if (buf == NULL || w == NULL || r == NULL || out == NULL ||
	    !b884_is_pow2(cap)) {
		return -1;
	}

	uW = *w;
	uR = *r;
	if (uW == uR) {
		return -1;
	}

	*out = buf[uR & (cap - 1u)];
	*r = uR + 1u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_spsc_try_read_u64(uint64_t *buf, size_t cap, size_t *w, size_t *r,
                           uint64_t *out)
    __attribute__((alias("gj_spsc_try_read_u64")));
