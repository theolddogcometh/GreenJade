/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8032: free(NULL) is no-op catalog stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_free_null_noop_8032(void);
 *     - Return 1: free(NULL) performs no deallocation (no-op).
 *   uint32_t __gj_free_null_noop_8032  (alias)
 *   __libcgj_batch8032_marker = "libcgj-batch8032"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Unique gj_free_null_noop_8032 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8032_marker[] = "libcgj-batch8032";

/* Catalog: free(NULL) is a pure no-op. */
#define B8032_FREE_NULL_NOOP  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8032_free_null_noop(void)
{
return B8032_FREE_NULL_NOOP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_free_null_noop_8032 - soft free(NULL) no-op flag.
 *
 * Always returns 1. Catalog constant only; does not call free(3).
 * No parent wires.
 */
uint32_t
gj_free_null_noop_8032(void)
{
(void)NULL;
return b8032_free_null_noop();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_free_null_noop_8032(void)
    __attribute__((alias("gj_free_null_noop_8032")));
