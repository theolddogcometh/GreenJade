/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6340: batch identity for frame pacing quanta.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6340(void);
 *     - Returns the compile-time graph batch number for this TU (6340).
 *   uint32_t __gj_batch_id_6340  (alias)
 *   __libcgj_batch6340_marker = "libcgj-batch6340"
 *
 * Exclusive continuum CREATE-ONLY (6331-6340: frame pacing quanta
 * stubs — fpq_quantum_us_6331, fpq_quanta_per_frame_6332,
 * fpq_target_fps_6333, fpq_budget_us_6334, fpq_slack_us_6335,
 * fpq_catchup_max_6336, fpq_max_skip_6337, fpq_enabled_6338,
 * fpq_ready_6339, batch_id_6340). Unique gj_batch_id_6340 surface
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6340_marker[] = "libcgj-batch6340";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6340_id(void)
{
	return 6340u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6340 - report this TU's graph batch number.
 *
 * Always returns 6340.
 */
uint32_t
gj_batch_id_6340(void)
{
	(void)NULL;
	return b6340_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6340(void)
    __attribute__((alias("gj_batch_id_6340")));
