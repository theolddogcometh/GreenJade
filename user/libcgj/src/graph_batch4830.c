/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4830: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4830(void);
 *     - Returns the compile-time graph batch number for this TU (4830).
 *   uint32_t __gj_batch_id_4830  (alias)
 *   __libcgj_batch4830_marker = "libcgj-batch4830"
 *
 * Exclusive continuum CREATE-ONLY (4821-4830: lock_token_u,
 * lock_owner_u, lock_seq_u, lock_is_free_u, lock_match_u,
 * lock_next_seq_u, lock_backoff_u, lock_deadline_u, lock_expired_u,
 * batch_id_4830). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4830_marker[] = "libcgj-batch4830";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4830_id(void)
{
	return 4830u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4830 - report this TU's graph batch number.
 *
 * Always returns 4830. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4830(void)
{
	(void)NULL;
	return b4830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4830(void)
    __attribute__((alias("gj_batch_id_4830")));
