/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8034: free action noop kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_free_action_noop_8034(void);
 *     - Return soft free-path action kind for NULL ptr (0 = noop).
 *   uint32_t __gj_free_action_noop_8034  (alias)
 *   __libcgj_batch8034_marker = "libcgj-batch8034"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Unique gj_free_action_noop_8034 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8034_marker[] = "libcgj-batch8034";

/* Soft free action: 0 = no-op (NULL pointer path). */
#define B8034_ACTION_NOOP  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8034_action_noop(void)
{
return B8034_ACTION_NOOP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_free_action_noop_8034 - soft free-path action kind for NULL.
 *
 * Always returns 0 (noop). Catalog constant only; does not call free(3).
 * No parent wires.
 */
uint32_t
gj_free_action_noop_8034(void)
{
(void)NULL;
return b8034_action_noop();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_free_action_noop_8034(void)
    __attribute__((alias("gj_free_action_noop_8034")));
