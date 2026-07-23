/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch881: simplified byte-ring push (head+count only).
 *
 * Distinct from gj_ring_u8_push (batch281) — no multi-def.
 *
 * Surface (unique symbols):
 *   int gj_ring_u8_push_simple(uint8_t *buf, size_t cap, size_t *head,
 *                              size_t *count, uint8_t b);
 *     — Enqueue one byte at *head. Returns 0 on success, -1 if full/bad.
 *   int __gj_ring_u8_push_simple  (alias)
 *   __libcgj_batch881_marker = "libcgj-batch881"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch881_marker[] = "libcgj-batch881";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_push_simple — enqueue one byte using head+count only.
 *
 * Advances *head modulo cap and increments *count. full when *count == cap.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 */
int
gj_ring_u8_push_simple(uint8_t *buf, size_t cap, size_t *head, size_t *count,
                       uint8_t b)
{
	size_t h;

	if (buf == NULL || head == NULL || count == NULL || cap == 0u) {
		return -1;
	}
	if (*count >= cap) {
		return -1;
	}

	h = *head;
	if (h >= cap) {
		return -1;
	}
	buf[h] = b;
	h++;
	if (h >= cap) {
		h = 0u;
	}
	*head = h;
	(*count)++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u8_push_simple(uint8_t *buf, size_t cap, size_t *head,
                             size_t *count, uint8_t b)
    __attribute__((alias("gj_ring_u8_push_simple")));
