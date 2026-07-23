/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6336: frame pacing catch-up quanta max.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fpq_catchup_max_6336(void);
 *     - Returns 4 (max catch-up quanta when the pacer is behind). Soft
 *       compile-time frame-pacing product stub.
 *   uint32_t __gj_fpq_catchup_max_6336  (alias)
 *   __libcgj_batch6336_marker = "libcgj-batch6336"
 *
 * Exclusive continuum CREATE-ONLY (6331-6340: frame pacing quanta
 * stubs — fpq_quantum_us_6331, fpq_quanta_per_frame_6332,
 * fpq_target_fps_6333, fpq_budget_us_6334, fpq_slack_us_6335,
 * fpq_catchup_max_6336, fpq_max_skip_6337, fpq_enabled_6338,
 * fpq_ready_6339, batch_id_6340). Unique gj_fpq_catchup_max_6336
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6336_marker[] = "libcgj-batch6336";

/* Maximum catch-up quanta when behind the pace clock. */
#define B6336_FPQ_CATCHUP_MAX  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6336_fpq_catchup_max(void)
{
	return B6336_FPQ_CATCHUP_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpq_catchup_max_6336 - report max catch-up quanta when behind.
 *
 * Always returns 4. Soft compile-time frame-pacing quanta stub.
 * No parent wires.
 */
uint32_t
gj_fpq_catchup_max_6336(void)
{
	(void)NULL;
	return b6336_fpq_catchup_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fpq_catchup_max_6336(void)
    __attribute__((alias("gj_fpq_catchup_max_6336")));
