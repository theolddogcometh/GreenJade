/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7635: getrusage is-RUSAGE_CHILDREN who test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rusage_is_children_i_7635(int32_t nWho);
 *     - Return 1 if nWho is RUSAGE_CHILDREN (-1), else 0.
 *   uint32_t __gj_rusage_is_children_i_7635  (alias)
 *   __libcgj_batch7635_marker = "libcgj-batch7635"
 *
 * Exclusive continuum CREATE-ONLY (7631-7640: getrusage who stubs —
 * self_id, children_id, thread_id, is_self, is_children, is_thread,
 * who_ok, who_errorish, who_count, batch_id_7640). Unique
 * gj_rusage_is_children_i_7635 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7635_marker[] = "libcgj-batch7635";

/* Soft RUSAGE_CHILDREN who code. */
#define B7635_RUSAGE_CHILDREN  ((int32_t)-1)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7635_is_children(int32_t nWho)
{
	return (nWho == B7635_RUSAGE_CHILDREN) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rusage_is_children_i_7635 - 1 if nWho is RUSAGE_CHILDREN (-1).
 *
 * nWho: raw getrusage-style who code (no sys/resource.h / no syscall)
 *
 * Returns 1 when nWho equals -1, else 0. Pure equality stub.
 * No parent wires.
 */
uint32_t
gj_rusage_is_children_i_7635(int32_t nWho)
{
	(void)NULL;
	return b7635_is_children(nWho);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rusage_is_children_i_7635(int32_t nWho)
    __attribute__((alias("gj_rusage_is_children_i_7635")));
