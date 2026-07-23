/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9990: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9990(void);
 *     - Returns the compile-time graph batch number for this TU (9990).
 *   uint32_t __gj_batch_id_9990  (alias)
 *   __libcgj_batch9990_marker = "libcgj-batch9990"
 *
 * Milestone 10000 exclusive continuum CREATE-ONLY (9981-9990:
 * shell_green_10000, libcgj_green_10000, bar3_ready_10000,
 * product_score_10000, continuum_ready_10000, smoke_soft_10000,
 * dyn_soft_10000, milestone_tag_10000, continuum_wave_10000,
 * batch_id_9990). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols - avoid multi-def. Soft pure-data catalog
 * only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9990_marker[] = "libcgj-batch9990";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9990_id(void)
{
	return 9990u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9990 - report this TU's graph batch number.
 *
 * Always returns 9990. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_9990(void)
{
	(void)NULL;
	return b9990_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9990(void)
    __attribute__((alias("gj_batch_id_9990")));
