/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2434: uint32_t ring front peek (count-based, _u).
 *
 * Surface (unique symbols):
 *   int gj_ring_u32_peek_u(const uint32_t *buf, size_t cap, size_t tail,
 *                          size_t count, uint32_t *out);
 *     — Read the oldest element (at tail) into *out without dequeue.
 *       Returns 0 on success; -1 if empty or bad args. Does not mutate
 *       cursors or buffer.
 *   int __gj_ring_u32_peek_u  (alias)
 *   __libcgj_batch2434_marker = "libcgj-batch2434"
 *
 * Ring design (count-based head/tail, exclusive advanced u32 wave):
 *   tail  — next read index (mod cap); passed by value
 *   count — elements currently stored; empty when count == 0
 *
 * Distinct from gj_ring_u32_peek_nth (batch1462 classic one-empty-slot)
 * and gj_ring_u8_peek (batch1803). Unique _u surface — no multi-def.
 *
 * Ring advanced exclusive wave (2431–2439). Clean-room freestanding pure C
 * (integer/pointer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2434_marker[] = "libcgj-batch2434";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff occupancy is non-zero and not corrupt relative to cap. */
static int
b2434_can_peek(size_t cap, size_t count)
{
	if (cap == 0u) {
		return 0;
	}
	if (count == 0u || count > cap) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_peek_u — non-destructive read of the oldest ring element.
 *
 * buf/cap: caller-owned element storage
 * tail:    next-read index (by value; not modified)
 * count:   live occupancy
 * out:     destination for the peeked value (must be non-NULL)
 *
 * Returns 0 on success with *out written. Returns -1 if empty, NULL
 * required args, cap == 0, or corrupt tail. *out is left untouched on
 * failure.
 */
int
gj_ring_u32_peek_u(const uint32_t *buf, size_t cap, size_t tail, size_t count,
                   uint32_t *out)
{
	if (buf == NULL || out == NULL) {
		return -1;
	}
	if (b2434_can_peek(cap, count) == 0) {
		return -1;
	}
	if (tail >= cap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}

	*out = buf[tail];
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u32_peek_u(const uint32_t *buf, size_t cap, size_t tail,
                         size_t count, uint32_t *out)
    __attribute__((alias("gj_ring_u32_peek_u")));
