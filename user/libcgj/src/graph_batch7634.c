/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7634: getrusage is-RUSAGE_SELF who test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rusage_is_self_i_7634(int32_t nWho);
 *     - Return 1 if nWho is RUSAGE_SELF (0), else 0.
 *   uint32_t __gj_rusage_is_self_i_7634  (alias)
 *   __libcgj_batch7634_marker = "libcgj-batch7634"
 *
 * Exclusive continuum CREATE-ONLY (7631-7640: getrusage who stubs —
 * self_id, children_id, thread_id, is_self, is_children, is_thread,
 * who_ok, who_errorish, who_count, batch_id_7640). Unique
 * gj_rusage_is_self_i_7634 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7634_marker[] = "libcgj-batch7634";

/* Soft RUSAGE_SELF who code. */
#define B7634_RUSAGE_SELF  ((int32_t)0)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7634_is_self(int32_t nWho)
{
	return (nWho == B7634_RUSAGE_SELF) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rusage_is_self_i_7634 - 1 if nWho is RUSAGE_SELF (0).
 *
 * nWho: raw getrusage-style who code (no sys/resource.h / no syscall)
 *
 * Returns 1 when nWho equals 0, else 0. Pure equality stub.
 * No parent wires.
 */
uint32_t
gj_rusage_is_self_i_7634(int32_t nWho)
{
	(void)NULL;
	return b7634_is_self(nWho);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rusage_is_self_i_7634(int32_t nWho)
    __attribute__((alias("gj_rusage_is_self_i_7634")));
