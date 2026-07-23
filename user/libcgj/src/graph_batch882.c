/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch882: simplified byte-ring pop (tail+count only).
 *
 * Distinct from gj_ring_u8_pop (batch281) — no multi-def.
 *
 * Surface (unique symbols):
 *   int gj_ring_u8_pop_simple(uint8_t *buf, size_t cap, size_t *tail,
 *                             size_t *count, uint8_t *out);
 *     — Dequeue one byte from *tail into *out. Returns 0 / -1.
 *   int __gj_ring_u8_pop_simple  (alias)
 *   __libcgj_batch882_marker = "libcgj-batch882"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch882_marker[] = "libcgj-batch882";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_pop_simple — dequeue one byte using tail+count only.
 *
 * Advances *tail modulo cap and decrements *count. empty when *count == 0.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 */
int
gj_ring_u8_pop_simple(uint8_t *buf, size_t cap, size_t *tail, size_t *count,
                      uint8_t *out)
{
	size_t t;

	if (buf == NULL || tail == NULL || count == NULL || out == NULL ||
	    cap == 0u) {
		return -1;
	}
	if (*count == 0u) {
		return -1;
	}

	t = *tail;
	if (t >= cap) {
		return -1;
	}
	*out = buf[t];
	t++;
	if (t >= cap) {
		t = 0u;
	}
	*tail = t;
	(*count)--;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u8_pop_simple(uint8_t *buf, size_t cap, size_t *tail,
                            size_t *count, uint8_t *out)
    __attribute__((alias("gj_ring_u8_pop_simple")));
