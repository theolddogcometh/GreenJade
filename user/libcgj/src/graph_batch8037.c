/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8037: free should-skip reclaim when null.
 *
 * Surface (unique symbols):
 *   uint32_t gj_free_should_skip_u_8037(uint32_t ptr_is_null);
 *     - Return 1 if ptr is null (skip reclaim), else 0.
 *   uint32_t __gj_free_should_skip_u_8037  (alias)
 *   __libcgj_batch8037_marker = "libcgj-batch8037"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Unique gj_free_should_skip_u_8037 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8037_marker[] = "libcgj-batch8037";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8037_should_skip(uint32_t u32PtrIsNull)
{
return (u32PtrIsNull != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_free_should_skip_u_8037 - 1 if free path should skip reclaim.
 *
 * ptr_is_null: non-zero if pointer is NULL, else 0
 *
 * Returns 1 when null (no-op path), else 0. Soft gate; does not call
 * free(3). No parent wires.
 */
uint32_t
gj_free_should_skip_u_8037(uint32_t u32PtrIsNull)
{
(void)NULL;
return b8037_should_skip(u32PtrIsNull);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_free_should_skip_u_8037(uint32_t u32PtrIsNull)
    __attribute__((alias("gj_free_should_skip_u_8037")));
