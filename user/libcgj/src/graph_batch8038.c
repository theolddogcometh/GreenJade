/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8038: free action kind from is_null flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_free_action_kind_u_8038(uint32_t ptr_is_null);
 *     - Return 0 (noop) if null, else 1 (reclaim).
 *   uint32_t __gj_free_action_kind_u_8038  (alias)
 *   __libcgj_batch8038_marker = "libcgj-batch8038"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Unique gj_free_action_kind_u_8038 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8038_marker[] = "libcgj-batch8038";

/* Soft free action kinds (match 8034/8035 catalog). */
#define B8038_ACTION_NOOP     ((uint32_t)0u)
#define B8038_ACTION_RECLAIM  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8038_action_kind(uint32_t u32PtrIsNull)
{
if (u32PtrIsNull != 0u)
return B8038_ACTION_NOOP;
return B8038_ACTION_RECLAIM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_free_action_kind_u_8038 - soft free-path action kind selector.
 *
 * ptr_is_null: non-zero if pointer is NULL, else 0
 *
 * Returns 0 for null (noop) or 1 for non-null (reclaim). Catalog gate
 * only; does not call free(3). No parent wires.
 */
uint32_t
gj_free_action_kind_u_8038(uint32_t u32PtrIsNull)
{
(void)NULL;
return b8038_action_kind(u32PtrIsNull);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_free_action_kind_u_8038(uint32_t u32PtrIsNull)
    __attribute__((alias("gj_free_action_kind_u_8038")));
