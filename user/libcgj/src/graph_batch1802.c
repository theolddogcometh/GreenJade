/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1802: byte-ring single-byte pop.
 *
 * Surface (theme symbol for ring buffer advanced exclusive wave):
 *   int gj_ring8_pop(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
 *                      size_t *count, uint8_t *out);
 *     — Dequeue one byte from tail into *out. Returns 0 on success, -1 if
 *       empty or bad args.
 *   int __gj_ring8_pop  (alias)
 *   __libcgj_batch1802_marker = "libcgj-batch1802"
 *
 * Ring design (count-based head/tail, same as batch281):
 *   head  — next write index (accepted for API symmetry; unused)
 *   tail  — next read index (mod cap)
 *   count — bytes currently stored (0..cap)
 *   empty when count == 0
 *
 * Note: same public name as batch281 gj_ring8_pop — multi-def if both
 * linked into one DSO. Exclusive wave 1801–1809 keeps the theme surface.
 *
 * Ring buffer advanced exclusive wave (1801–1809). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1802_marker[] = "libcgj-batch1802";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b1802_next(size_t i, size_t cap)
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
 * gj_ring8_pop — dequeue one byte into *out.
 *
 * Advances tail modulo cap and decrements count. Does not touch head.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 */
int
gj_ring8_pop(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
               size_t *count, uint8_t *out)
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
	*tail = b1802_next(t, cap);
	(*count)--;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring8_pop(uint8_t *buf, size_t cap, size_t *head, size_t *tail,
                     size_t *count, uint8_t *out)
    __attribute__((alias("gj_ring8_pop")));
