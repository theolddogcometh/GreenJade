/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7633: getrusage RUSAGE_THREAD who id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rusage_who_thread_id_7633(void);
 *     - Return soft RUSAGE_THREAD who code (1): calling thread only.
 *   uint32_t __gj_rusage_who_thread_id_7633  (alias)
 *   __libcgj_batch7633_marker = "libcgj-batch7633"
 *
 * Exclusive continuum CREATE-ONLY (7631-7640: getrusage who stubs —
 * self_id, children_id, thread_id, is_self, is_children, is_thread,
 * who_ok, who_errorish, who_count, batch_id_7640). Unique
 * gj_rusage_who_thread_id_7633 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7633_marker[] = "libcgj-batch7633";

/* Soft RUSAGE_THREAD: resource usage of the calling thread (who = 1). */
#define B7633_RUSAGE_THREAD  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7633_who_thread_id(void)
{
	return B7633_RUSAGE_THREAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rusage_who_thread_id_7633 - soft RUSAGE_THREAD who constant.
 *
 * Always returns 1. Catalog id only; does not call getrusage.
 * No parent wires.
 */
uint32_t
gj_rusage_who_thread_id_7633(void)
{
	(void)NULL;
	return b7633_who_thread_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rusage_who_thread_id_7633(void)
    __attribute__((alias("gj_rusage_who_thread_id_7633")));
