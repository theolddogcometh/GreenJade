/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7639: getrusage known-who count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rusage_who_count_7639(void);
 *     - Return the number of standard RUSAGE_* who codes (3).
 *   uint32_t __gj_rusage_who_count_7639  (alias)
 *   __libcgj_batch7639_marker = "libcgj-batch7639"
 *
 * Exclusive continuum CREATE-ONLY (7631-7640: getrusage who stubs —
 * self_id, children_id, thread_id, is_self, is_children, is_thread,
 * who_ok, who_errorish, who_count, batch_id_7640). Unique
 * gj_rusage_who_count_7639 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7639_marker[] = "libcgj-batch7639";

/* SELF, CHILDREN, THREAD — three catalogued who codes. */
#define B7639_RUSAGE_WHO_COUNT  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7639_who_count(void)
{
	return B7639_RUSAGE_WHO_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rusage_who_count_7639 - number of standard getrusage who codes.
 *
 * Always returns 3 (SELF / CHILDREN / THREAD). Catalog count only;
 * does not call getrusage. No parent wires.
 */
uint32_t
gj_rusage_who_count_7639(void)
{
	(void)NULL;
	return b7639_who_count();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rusage_who_count_7639(void)
    __attribute__((alias("gj_rusage_who_count_7639")));
