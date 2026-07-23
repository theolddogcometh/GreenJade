/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8033: free(NULL) leaves errno OK stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_free_null_errno_ok_8033(void);
 *     - Return 1: free(NULL) does not require errno to change.
 *   uint32_t __gj_free_null_errno_ok_8033  (alias)
 *   __libcgj_batch8033_marker = "libcgj-batch8033"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Unique gj_free_null_errno_ok_8033 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8033_marker[] = "libcgj-batch8033";

/* Catalog: free(NULL) path does not force errno mutation. */
#define B8033_FREE_NULL_ERRNO_OK  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8033_free_null_errno_ok(void)
{
return B8033_FREE_NULL_ERRNO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_free_null_errno_ok_8033 - soft free(NULL) errno-stability flag.
 *
 * Always returns 1. Catalog constant only; does not touch errno or
 * call free(3). No parent wires.
 */
uint32_t
gj_free_null_errno_ok_8033(void)
{
(void)NULL;
return b8033_free_null_errno_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_free_null_errno_ok_8033(void)
    __attribute__((alias("gj_free_null_errno_ok_8033")));
