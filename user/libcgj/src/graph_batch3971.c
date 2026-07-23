/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3971: offset-only arena bump without align (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_arena_bump_u(size_t *used, size_t cap, size_t need);
 *     - Bump-allocate need bytes from a [0, cap) virtual span. Advances
 *       *used by need and returns the start offset on success. No
 *       alignment step (byte-granular). NULL used, used > cap, or
 *       overflow / OOM → (size_t)-1 (watermark unchanged on failure).
 *       need == 0 still succeeds and returns the current *used.
 *   size_t __gj_arena_bump_u  (alias)
 *   __libcgj_batch3971_marker = "libcgj-batch3971"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980): arena_bump_u,
 * arena_align_u, arena_remaining_u, arena_contains_u, pool_push_free_u,
 * pool_pop_free_u, slab_offset_u, slab_capacity_u, buddy_order_u,
 * batch_id_3980. Distinct from gj_arena_bump (batch545 pointer form),
 * gj_arena_alloc_u (batch3342 aligned offset form), and
 * gj_arena_bump_alloc (batch1523) — unique gj_arena_bump_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3971_marker[] = "libcgj-batch3971";

/* Failure sentinel for offset-only bump. */
#define B3971_FAIL  ((size_t)-1)

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3971_bump(size_t *pUsed, size_t cbCap, size_t cbNeed)
{
	size_t uCur;
	size_t uEnd;

	if (pUsed == NULL) {
		return B3971_FAIL;
	}
	uCur = *pUsed;
	if (uCur > cbCap) {
		return B3971_FAIL;
	}
	if (cbNeed > cbCap - uCur) {
		return B3971_FAIL;
	}
	uEnd = uCur + cbNeed;
	*pUsed = uEnd;
	return uCur;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_bump_u - byte-granular bump; return start offset.
 *
 * used: in/out watermark (advanced on success)
 * cap:  total capacity in bytes of the virtual span
 * need: payload size (0 returns current *used without consuming)
 *
 * Returns start offset on success, or (size_t)-1 on failure.
 * No parent wires.
 */
size_t
gj_arena_bump_u(size_t *pUsed, size_t cbCap, size_t cbNeed)
{
	(void)NULL;
	return b3971_bump(pUsed, cbCap, cbNeed);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arena_bump_u(size_t *pUsed, size_t cbCap, size_t cbNeed)
    __attribute__((alias("gj_arena_bump_u")));
