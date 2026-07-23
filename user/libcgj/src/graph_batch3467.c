/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3467: uint32_t ring push (count-based, wave-
 * unique rename).
 *
 * Surface (unique symbols):
 *   int gj_ring_u32_push_u2(uint32_t *buf, size_t cap, size_t *head,
 *                           size_t *tail, size_t *count, uint32_t v);
 *     — Enqueue one uint32_t at head. Returns 0 on success, -1 if full
 *       or bad args. Renamed from gj_ring_u32_push_u (collision with
 *       batch2432).
 *   int __gj_ring_u32_push_u2  (alias)
 *   __libcgj_batch3467_marker = "libcgj-batch3467"
 *
 * Ring design (count-based head/tail):
 *   head  — next write index (mod cap)
 *   tail  — next read index (API symmetry; unused by push)
 *   count — elements currently stored (0..cap)
 *   full when count == cap
 *
 * CREATE-ONLY exclusive continuum wave (3461-3470). Distinct from
 * gj_ring_u32_push_u (batch2432), gj_ring_u32_push (batch533), and
 * gj_ring_u32_push_force (batch1461). Unique _u2 surface — no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3467_marker[] = "libcgj-batch3467";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b3467_next(size_t i, size_t cap)
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
 * gj_ring_u32_push_u2 — enqueue one uint32_t into a count-based ring.
 *
 * Advances head modulo cap and increments count. Does not touch tail.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 *
 * Wave-unique rename: gj_ring_u32_push_u is owned by batch2432.
 */
int
gj_ring_u32_push_u2(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
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
	*head = b3467_next(h, cap);
	(*count)++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u32_push_u2(uint32_t *buf, size_t cap, size_t *head, size_t *tail,
                          size_t *count, uint32_t v)
    __attribute__((alias("gj_ring_u32_push_u2")));
