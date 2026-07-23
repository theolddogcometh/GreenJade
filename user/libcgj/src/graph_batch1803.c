/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1803: non-destructive byte-ring front peek.
 *
 * Surface (unique symbols):
 *   int gj_ring_u8_peek(const uint8_t *buf, size_t cap, size_t tail,
 *                       size_t count, uint8_t *out);
 *     — Read the oldest byte (at tail) into *out without dequeue.
 *       Returns 0 on success; -1 if empty or bad args. Does not mutate
 *       cursors or buffer.
 *   int __gj_ring_u8_peek  (alias)
 *   __libcgj_batch1803_marker = "libcgj-batch1803"
 *
 * Ring design (count-based head/tail, same as batch281):
 *   tail  — next read index (mod cap); passed by value
 *   count — bytes currently stored; empty when count == 0
 *
 * Distinct from gj_ring_u32_peek_nth (batch1462) — unique u8 front peek.
 *
 * Ring buffer advanced exclusive wave (1801–1809). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1803_marker[] = "libcgj-batch1803";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_peek — non-destructive read of the oldest ring byte.
 *
 * buf/cap: caller-owned byte storage
 * tail:    next-read index (by value; not modified)
 * count:   live occupancy
 * out:     destination for the peeked byte (must be non-NULL)
 *
 * Returns 0 on success with *out written. Returns -1 if empty, NULL
 * required args, cap == 0, or corrupt tail. *out is left untouched on
 * failure.
 */
int
gj_ring_u8_peek(const uint8_t *buf, size_t cap, size_t tail, size_t count,
                uint8_t *out)
{
	if (buf == NULL || out == NULL || cap == 0u) {
		return -1;
	}
	if (count == 0u || count > cap) {
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

int __gj_ring_u8_peek(const uint8_t *buf, size_t cap, size_t tail,
                      size_t count, uint8_t *out)
    __attribute__((alias("gj_ring_u8_peek")));
