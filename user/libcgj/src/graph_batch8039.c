/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8039: free null policy id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_free_null_policy_8039(void);
 *     - Return soft free-null policy id (1 = ISO C null-ok).
 *   uint32_t __gj_free_null_policy_8039  (alias)
 *   __libcgj_batch8039_marker = "libcgj-batch8039"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Unique gj_free_null_policy_8039 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8039_marker[] = "libcgj-batch8039";

/* Soft free-null policy: 1 = ISO C free(NULL) is OK (no-op). */
#define B8039_NULL_POLICY_ISO  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8039_null_policy(void)
{
return B8039_NULL_POLICY_ISO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_free_null_policy_8039 - soft free(NULL) policy identifier.
 *
 * Always returns 1 (ISO C null-ok). Catalog constant only; does not
 * call free(3). No parent wires.
 */
uint32_t
gj_free_null_policy_8039(void)
{
(void)NULL;
return b8039_null_policy();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_free_null_policy_8039(void)
    __attribute__((alias("gj_free_null_policy_8039")));
