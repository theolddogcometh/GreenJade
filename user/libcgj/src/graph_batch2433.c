/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2433: uint32_t ring pop (count-based, _u surface).
 *
 * Surface (unique symbols):
 *   int gj_ring_u32_pop_u(uint32_t *buf, size_t cap, size_t *head,
 *                         size_t *tail, size_t *count, uint32_t *out);
 *     — Dequeue one uint32_t from tail into *out. Returns 0 on success,
 *       -1 if empty or bad args.
 *   int __gj_ring_u32_pop_u  (alias)
 *   __libcgj_batch2433_marker = "libcgj-batch2433"
 *
 * Ring design (count-based head/tail, exclusive advanced u32 wave):
 *   head  — next write index (API symmetry; unused by pop)
 *   tail  — next read index (mod cap)
 *   count — elements currently stored (0..cap)
 *   empty when count == 0
 *
 * Distinct from gj_ring_u32_pop (batch534 classic one-empty-slot, no count).
 * Unique _u surface — no multi-def.
 *
 * Ring advanced exclusive wave (2431–2439). Clean-room freestanding pure C
 * (integer/pointer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2433_marker[] = "libcgj-batch2433";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b2433_next(size_t i, size_t cap)
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
 * gj_ring_u32_pop_u — dequeue one uint32_t from a count-based ring.
 *
 * Advances tail modulo cap and decrements count. Does not touch head.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 */
int
gj_ring_u32_pop_u(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
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
	*tail = b2433_next(t, cap);
	(*count)--;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u32_pop_u(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                        size_t *count, uint32_t *out)
    __attribute__((alias("gj_ring_u32_pop_u")));
