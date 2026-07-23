/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1809: bulk discard of oldest ring bytes.
 *
 * Surface (unique symbols):
 *   size_t gj_ring_u8_discard_n(size_t cap, size_t *tail, size_t *count,
 *                               size_t n);
 *     — Drop up to n oldest bytes by advancing *tail and decreasing
 *       *count. Does not read or write the buffer payload. Returns the
 *       number of bytes discarded (0..n; clamped to occupancy).
 *   size_t __gj_ring_u8_discard_n  (alias)
 *   __libcgj_batch1809_marker = "libcgj-batch1809"
 *
 * Ring design (count-based head/tail, same as batch281):
 *   tail  — next read index (mod cap); advanced by bytes discarded
 *   count — live occupancy; decremented by bytes discarded
 *   head is not touched (dropped bytes become free space for push)
 *
 * Ring buffer advanced exclusive wave (1801–1809). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1809_marker[] = "libcgj-batch1809";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by k steps with modular wrap (any cap > 0). */
static size_t
b1809_add_mod(size_t i, size_t k, size_t cap)
{
	size_t n;

	/* k < cap in our call sites (clamped to count <= cap). */
	n = i + k;
	if (n >= cap) {
		n -= cap;
		if (n >= cap) {
			n %= cap;
		}
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_discard_n — drop up to n oldest bytes without reading them.
 *
 * cap:   total slot count (needed for modular tail advance)
 * tail:  next-read index (advanced by bytes discarded)
 * count: live occupancy (decremented by bytes discarded)
 * n:     maximum bytes to discard
 *
 * Returns bytes actually discarded. Returns 0 if NULL required args,
 * cap == 0, corrupt cursors, or n == 0.
 */
size_t
gj_ring_u8_discard_n(size_t cap, size_t *tail, size_t *count, size_t n)
{
	size_t t;
	size_t c;

	if (tail == NULL || count == NULL || cap == 0u) {
		return 0u;
	}
	if (n == 0u) {
		return 0u;
	}

	t = *tail;
	c = *count;
	if (t >= cap || c > cap) {
		/* Corrupt cursor — refuse rather than advance out of bounds. */
		return 0u;
	}

	if (n > c) {
		n = c;
	}

	*tail = b1809_add_mod(t, n, cap);
	*count = c - n;
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_u8_discard_n(size_t cap, size_t *tail, size_t *count,
                              size_t n)
    __attribute__((alias("gj_ring_u8_discard_n")));
