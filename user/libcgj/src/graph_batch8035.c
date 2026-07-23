/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8035: free action reclaim kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_free_action_reclaim_8035(void);
 *     - Return soft free-path action kind for non-NULL ptr (1 = reclaim).
 *   uint32_t __gj_free_action_reclaim_8035  (alias)
 *   __libcgj_batch8035_marker = "libcgj-batch8035"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Unique gj_free_action_reclaim_8035 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8035_marker[] = "libcgj-batch8035";

/* Soft free action: 1 = reclaim (non-NULL pointer path). */
#define B8035_ACTION_RECLAIM  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8035_action_reclaim(void)
{
return B8035_ACTION_RECLAIM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_free_action_reclaim_8035 - soft free-path action kind for non-NULL.
 *
 * Always returns 1 (reclaim). Catalog constant only; does not call free(3).
 * No parent wires.
 */
uint32_t
gj_free_action_reclaim_8035(void)
{
(void)NULL;
return b8035_action_reclaim();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_free_action_reclaim_8035(void)
    __attribute__((alias("gj_free_action_reclaim_8035")));
