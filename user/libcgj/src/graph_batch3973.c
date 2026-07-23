/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3973: arena remaining capacity (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_arena_remaining_u(size_t used, size_t cap);
 *     - Bytes still free in a [0, cap) virtual span given watermark
 *       used: returns cap - used when used <= cap, else 0 (overused
 *       or corrupt watermark treated as empty remaining).
 *   size_t __gj_arena_remaining_u  (alias)
 *   __libcgj_batch3973_marker = "libcgj-batch3973"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980). Unique
 * gj_arena_remaining_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3973_marker[] = "libcgj-batch3973";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3973_remaining(size_t cbUsed, size_t cbCap)
{
	if (cbUsed > cbCap) {
		return 0u;
	}
	return cbCap - cbUsed;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_remaining_u - free bytes left given used watermark and cap.
 *
 * used: current watermark
 * cap:  total capacity
 *
 * Returns cap - used, or 0 if used > cap.
 * No parent wires.
 */
size_t
gj_arena_remaining_u(size_t cbUsed, size_t cbCap)
{
	(void)NULL;
	return b3973_remaining(cbUsed, cbCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arena_remaining_u(size_t cbUsed, size_t cbCap)
    __attribute__((alias("gj_arena_remaining_u")));
