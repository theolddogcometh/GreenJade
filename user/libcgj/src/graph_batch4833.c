/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4833: cache-line index of an address.
 *
 * Surface (unique symbols):
 *   uintptr_t gj_cacheline_index_u(uintptr_t addr);
 *     - Return floor(addr / 64), the cache-line index of addr.
 *   uintptr_t __gj_cacheline_index_u  (alias)
 *   __libcgj_batch4833_marker = "libcgj-batch4833"
 *
 * Exclusive continuum CREATE-ONLY (4831-4840: cacheline_size_u,
 * cacheline_align_u, cacheline_index_u, cacheline_offset_u,
 * cacheline_span_u, prefetch_hint_read_u, prefetch_hint_write_u,
 * false_sharing_risk_u, pad_to_line_u, batch_id_4840). Unique
 * gj_cacheline_index_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4833_marker[] = "libcgj-batch4833";

/* log2 of assumed cache-line width (64). */
#define B4833_CL_SHIFT  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uintptr_t
b4833_cacheline_index(uintptr_t uAddr)
{
	return uAddr >> B4833_CL_SHIFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cacheline_index_u - cache-line ordinal of an address.
 *
 * addr: absolute or relative address as an integer.
 * Returns addr / 64 (floor). No parent wires.
 */
uintptr_t
gj_cacheline_index_u(uintptr_t uAddr)
{
	(void)NULL;
	return b4833_cacheline_index(uAddr);
}

/* ---- underscored alias ------------------------------------------------- */

uintptr_t __gj_cacheline_index_u(uintptr_t uAddr)
    __attribute__((alias("gj_cacheline_index_u")));
