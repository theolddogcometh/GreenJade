/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8036: free null-ok gate by is_null flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_free_null_ok_u_8036(uint32_t ptr_is_null);
 *     - Return 1 always: free is catalog-ok for NULL and non-NULL args.
 *   uint32_t __gj_free_null_ok_u_8036  (alias)
 *   __libcgj_batch8036_marker = "libcgj-batch8036"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Unique gj_free_null_ok_u_8036 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8036_marker[] = "libcgj-batch8036";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8036_free_null_ok(uint32_t u32PtrIsNull)
{
/* ISO C: free(NULL) OK; free(non-NULL) also OK when ptr is live. */
(void)u32PtrIsNull;
return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_free_null_ok_u_8036 - soft free-arg validity for null/non-null.
 *
 * ptr_is_null: non-zero if pointer is NULL, else 0
 *
 * Always returns 1 (both NULL and non-NULL args catalog-ok for free).
 * Soft check; does not call free(3). No parent wires.
 */
uint32_t
gj_free_null_ok_u_8036(uint32_t u32PtrIsNull)
{
(void)NULL;
return b8036_free_null_ok(u32PtrIsNull);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_free_null_ok_u_8036(uint32_t u32PtrIsNull)
    __attribute__((alias("gj_free_null_ok_u_8036")));
