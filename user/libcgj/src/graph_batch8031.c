/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8031: free(NULL) is OK catalog stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_free_null_ok_8031(void);
 *     - Return 1: ISO C free(NULL) is defined (no-op) and always OK.
 *   uint32_t __gj_free_null_ok_8031  (alias)
 *   __libcgj_batch8031_marker = "libcgj-batch8031"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Unique gj_free_null_ok_8031 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8031_marker[] = "libcgj-batch8031";

/* Catalog: free(NULL) is always permitted (ISO C). */
#define B8031_FREE_NULL_OK  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8031_free_null_ok(void)
{
return B8031_FREE_NULL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_free_null_ok_8031 - soft free(NULL) validity flag.
 *
 * Always returns 1. Catalog constant only; does not call free(3).
 * No parent wires.
 */
uint32_t
gj_free_null_ok_8031(void)
{
(void)NULL;
return b8031_free_null_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_free_null_ok_8031(void)
    __attribute__((alias("gj_free_null_ok_8031")));
