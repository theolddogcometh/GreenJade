/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2432: uint32_t ring push (count-based, _u surface).
 *
 * Surface (unique symbols):
 *   int gj_ring_u32_push_u(uint32_t *buf, size_t cap, size_t *head,
 *                          size_t *tail, size_t *count, uint32_t v);
 *     — Enqueue one uint32_t at head. Returns 0 on success, -1 if full
 *       or bad args.
 *   int __gj_ring_u32_push_u  (alias)
 *   __libcgj_batch2432_marker = "libcgj-batch2432"
 *
 * Ring design (count-based head/tail, exclusive advanced u32 wave):
 *   head  — next write index (mod cap)
 *   tail  — next read index (API symmetry; unused by push)
 *   count — elements currently stored (0..cap)
 *   full when count == cap
 *
 * Distinct from gj_ring_u32_push (batch533 classic one-empty-slot, no count)
 * and gj_ring_u32_push_force (batch1461). Unique _u surface — no multi-def.
 *
 * Ring advanced exclusive wave (2431–2439). Clean-room freestanding pure C
 * (integer/pointer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2432_marker[] = "libcgj-batch2432";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b2432_next(size_t i, size_t cap)
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
 * gj_ring_u32_push_u — enqueue one uint32_t into a count-based ring.
 *
 * Advances head modulo cap and increments count. Does not touch tail.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 */
int
gj_ring_u32_push_u(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                   size_t *count, uint32_t v)
{
	size_t h;

	(void)tail;

	if (buf == NULL || head == NULL || count == NULL || cap == 0u) {
		return -1;
	}
	if (*count >= cap) {
		return -1;
	}

	h = *head;
	if (h >= cap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}
	buf[h] = v;
	*head = b2432_next(h, cap);
	(*count)++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u32_push_u(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                         size_t *count, uint32_t v)
    __attribute__((alias("gj_ring_u32_push_u")));
