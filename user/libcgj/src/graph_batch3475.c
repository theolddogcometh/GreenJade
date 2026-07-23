/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3475: freestanding uint64 stack length query
 * (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_stack_u64_len_u(size_t sp);
 *     - Return the live depth of a depth-based u64 stack. Passes through
 *       the caller-owned depth watermark. Useful as a typed surface /
 *       graph node for occupancy probes without exposing raw fields.
 *       Always returns the input sp (by value).
 *   size_t __gj_stack_u64_len_u  (alias)
 *   __libcgj_batch3475_marker = "libcgj-batch3475"
 *
 * Exclusive continuum CREATE-ONLY (3471-3480). Distinct from
 * gj_queue_u32_len_u (batch2665) / gj_deque_u32_len_u (batch3326) by
 * public stack_u64_len_u name — unique surface; no multi-def.
 * No parent wires. Caller buffers only (depth is a by-value watermark).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3475_marker[] = "libcgj-batch3475";

/* ---- freestanding helpers ---------------------------------------------- */

/* Identity pass-through of depth (keeps TU non-trivial for -Werror). */
static size_t
b3475_id(size_t cSp)
{
	return cSp;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u64_len_u - report live depth of a depth-based u64 stack.
 *
 * sp: caller-owned depth watermark (0..cap for a valid stack)
 *
 * Returns sp unchanged. Does not validate against capacity (capacity is
 * not available on this surface); callers pair with push/pop full/empty
 * checks when needed.
 */
size_t
gj_stack_u64_len_u(size_t cSp)
{
	(void)NULL;
	return b3475_id(cSp);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_stack_u64_len_u(size_t cSp)
    __attribute__((alias("gj_stack_u64_len_u")));
