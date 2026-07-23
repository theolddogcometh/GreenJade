/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1304: Classic head/tail ring deque occupancy (one-empty-slot design).
 *
 * Surface (unique symbols):
 *   size_t gj_deque_depth_ring(size_t head, size_t tail, size_t cap);
 *     — used = (tail - head) mod cap for classic ring (0 .. cap-1).
 *       cap < 2 or cursor out of range → 0.
 *   size_t __gj_deque_depth_ring  (alias)
 *   __libcgj_batch1304_marker = "libcgj-batch1304"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>

const char __libcgj_batch1304_marker[] = "libcgj-batch1304";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_depth_ring — occupancy of a classic head/tail ring deque.
 *
 * Same layout as batch703–706 / 677–678: empty when head == tail; usable
 * capacity is (cap - 1). used = (tail - head) mod cap.
 * Returns 0 if cap < 2 or either cursor is out of [0, cap).
 */
size_t
gj_deque_depth_ring(size_t uHead, size_t uTail, size_t uCap)
{
	if (uCap < 2u || uHead >= uCap || uTail >= uCap) {
		return 0u;
	}
	if (uTail >= uHead) {
		return uTail - uHead;
	}
	return (uCap - uHead) + uTail;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_deque_depth_ring(size_t uHead, size_t uTail, size_t uCap)
    __attribute__((alias("gj_deque_depth_ring")));

