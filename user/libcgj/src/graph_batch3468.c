/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3468: uint32_t ring pop (count-based, wave-
 * unique rename).
 *
 * Surface (unique symbols):
 *   int gj_ring_u32_pop_u2(uint32_t *buf, size_t cap, size_t *head,
 *                          size_t *tail, size_t *count, uint32_t *out);
 *     — Dequeue one uint32_t from tail into *out. Returns 0 on success,
 *       -1 if empty or bad args. Renamed from gj_ring_u32_pop_u
 *       (collision with batch2433).
 *   int __gj_ring_u32_pop_u2  (alias)
 *   __libcgj_batch3468_marker = "libcgj-batch3468"
 *
 * Ring design (count-based head/tail):
 *   head  — next write index (API symmetry; unused by pop)
 *   tail  — next read index (mod cap)
 *   count — elements currently stored (0..cap)
 *   empty when count == 0
 *
 * CREATE-ONLY exclusive continuum wave (3461-3470). Distinct from
 * gj_ring_u32_pop_u (batch2433) and gj_ring_u32_pop (batch534). Unique
 * _u2 surface — no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3468_marker[] = "libcgj-batch3468";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b3468_next(size_t i, size_t cap)
{
	size_t n;

	n = i + 1u;
	if (n >= cap) {
		n = 0u;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_pop_u2 — dequeue one uint32_t from a count-based ring.
 *
 * Advances tail modulo cap and decrements count. Does not touch head.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 *
 * Wave-unique rename: gj_ring_u32_pop_u is owned by batch2433.
 */
int
gj_ring_u32_pop_u2(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                   size_t *count, uint32_t *out)
{
	size_t t;

	(void)head;

	if (buf == NULL || tail == NULL || count == NULL || out == NULL ||
	    cap == 0u) {
		return -1;
	}
	if (*count == 0u) {
		return -1;
	}

	t = *tail;
	if (t >= cap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}
	*out = buf[t];
	*tail = b3468_next(t, cap);
	(*count)--;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u32_pop_u2(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                         size_t *count, uint32_t *out)
    __attribute__((alias("gj_ring_u32_pop_u2")));
