/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4780: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4780(void);
 *     - Returns the compile-time graph batch number for this TU (4780).
 *   uint32_t __gj_batch_id_4780  (alias)
 *   __libcgj_batch4780_marker = "libcgj-batch4780"
 *
 * Exclusive continuum CREATE-ONLY (4771-4780: job_priority_u,
 * job_id_next_u, job_state_is_done_u, job_state_is_run_u,
 * job_state_is_pend_u, job_timeout_u, job_retry_ok_u,
 * job_backoff_u, job_score_u, batch_id_4780).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4780_marker[] = "libcgj-batch4780";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4780_id(void)
{
	return 4780u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4780 - report this TU's graph batch number.
 *
 * Always returns 4780. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4780(void)
{
	(void)NULL;
	return b4780_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4780(void)
    __attribute__((alias("gj_batch_id_4780")));
