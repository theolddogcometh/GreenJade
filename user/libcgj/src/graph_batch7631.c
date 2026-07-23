/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7631: getrusage RUSAGE_SELF who id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rusage_who_self_id_7631(void);
 *     - Return soft RUSAGE_SELF who code (0): calling process/thread.
 *   uint32_t __gj_rusage_who_self_id_7631  (alias)
 *   __libcgj_batch7631_marker = "libcgj-batch7631"
 *
 * Exclusive continuum CREATE-ONLY (7631-7640: getrusage who stubs —
 * self_id, children_id, thread_id, is_self, is_children, is_thread,
 * who_ok, who_errorish, who_count, batch_id_7640). Unique
 * gj_rusage_who_self_id_7631 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7631_marker[] = "libcgj-batch7631";

/* Soft RUSAGE_SELF: resource usage of the calling process (who = 0). */
#define B7631_RUSAGE_SELF  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7631_who_self_id(void)
{
	return B7631_RUSAGE_SELF;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rusage_who_self_id_7631 - soft RUSAGE_SELF who constant.
 *
 * Always returns 0. Catalog id only; does not call getrusage.
 * No parent wires.
 */
uint32_t
gj_rusage_who_self_id_7631(void)
{
	(void)NULL;
	return b7631_who_self_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rusage_who_self_id_7631(void)
    __attribute__((alias("gj_rusage_who_self_id_7631")));
