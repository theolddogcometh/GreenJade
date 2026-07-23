/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7637: getrusage who-ok membership test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rusage_who_ok_i_7637(int32_t nWho);
 *     - Return 1 if nWho is SELF(0)/CHILDREN(-1)/THREAD(1), else 0.
 *   uint32_t __gj_rusage_who_ok_i_7637  (alias)
 *   __libcgj_batch7637_marker = "libcgj-batch7637"
 *
 * Exclusive continuum CREATE-ONLY (7631-7640: getrusage who stubs —
 * self_id, children_id, thread_id, is_self, is_children, is_thread,
 * who_ok, who_errorish, who_count, batch_id_7640). Unique
 * gj_rusage_who_ok_i_7637 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7637_marker[] = "libcgj-batch7637";

/* Soft standard getrusage who codes. */
#define B7637_RUSAGE_SELF      ((int32_t)0)
#define B7637_RUSAGE_CHILDREN  ((int32_t)-1)
#define B7637_RUSAGE_THREAD    ((int32_t)1)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7637_who_ok(int32_t nWho)
{
	if (nWho == B7637_RUSAGE_SELF || nWho == B7637_RUSAGE_CHILDREN ||
	    nWho == B7637_RUSAGE_THREAD) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rusage_who_ok_i_7637 - 1 if nWho is a known RUSAGE_* who code.
 *
 * nWho: raw getrusage-style who code (no sys/resource.h / no syscall)
 *
 * Returns 1 for SELF(0)/CHILDREN(-1)/THREAD(1), else 0.
 * Pure membership stub. No parent wires.
 */
uint32_t
gj_rusage_who_ok_i_7637(int32_t nWho)
{
	(void)NULL;
	return b7637_who_ok(nWho);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rusage_who_ok_i_7637(int32_t nWho)
    __attribute__((alias("gj_rusage_who_ok_i_7637")));
