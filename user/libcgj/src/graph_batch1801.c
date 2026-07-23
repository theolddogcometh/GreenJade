/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1801: byte-ring single-byte push.
 *
 * Surface (theme symbol for ring buffer advanced exclusive wave):
 *   int gj_ring8_push(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
 *                       size_t *count, uint8_t b);
 *     — Enqueue one byte at head. Returns 0 on success, -1 if full or bad args.
 *   int __gj_ring8_push  (alias)
 *   __libcgj_batch1801_marker = "libcgj-batch1801"
 *
 * Ring design (count-based head/tail, same as batch281):
 *   head  — next write index (mod cap)
 *   tail  — next read index (accepted for API symmetry; unused)
 *   count — bytes currently stored (0..cap)
 *   full when count == cap
 *
 * Note: same public name as batch281 gj_ring8_push — multi-def if both
 * linked into one DSO. Exclusive wave 1801–1809 keeps the theme surface.
 *
 * Ring buffer advanced exclusive wave (1801–1809). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1801_marker[] = "libcgj-batch1801";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b1801_next(size_t i, size_t cap)
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
 * gj_ring8_push — enqueue one byte.
 *
 * Advances head modulo cap and increments count. Does not touch tail.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 */
int
gj_ring8_push(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
                size_t *count, uint8_t b)
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
	buf[h] = b;
	*head = b1801_next(h, cap);
	(*count)++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring8_push(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
                      size_t *count, uint8_t b)
    __attribute__((alias("gj_ring8_push")));
