/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4755: pure-u64 bump reset to base (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bump_reset_u(uint64_t base);
 *     - Identity reset: return base as the new cursor (empty arena).
 *   uint64_t __gj_bump_reset_u  (alias)
 *   __libcgj_batch4755_marker = "libcgj-batch4755"
 *
 * Exclusive continuum CREATE-ONLY (4751-4760: bump_alloc_u,
 * bump_align_u, bump_remaining_u, bump_contains_u, bump_reset_u,
 * bump_mark_u, bump_rewind_u, bump_used_u, bump_ok_u, batch_id_4760).
 * Distinct from gj_bump_init (batch282) which writes *used = 0 —
 * unique gj_bump_reset_u pure u64 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4755_marker[] = "libcgj-batch4755";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4755_reset(uint64_t u64Base)
{
	return u64Base;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_reset_u - reset a bump cursor to the arena base.
 *
 * base: arena base address/offset
 *
 * Returns base (new empty watermark). Pure identity; useful for
 * compositional call sites. No parent wires.
 */
uint64_t
gj_bump_reset_u(uint64_t u64Base)
{
	(void)NULL;
	return b4755_reset(u64Base);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bump_reset_u(uint64_t u64Base)
    __attribute__((alias("gj_bump_reset_u")));
