/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6337: frame pacing max skip frames.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fpq_max_skip_6337(void);
 *     - Returns 2 (max frames the pacer may skip when deeply late).
 *       Soft compile-time frame-pacing product stub.
 *   uint32_t __gj_fpq_max_skip_6337  (alias)
 *   __libcgj_batch6337_marker = "libcgj-batch6337"
 *
 * Exclusive continuum CREATE-ONLY (6331-6340: frame pacing quanta
 * stubs — fpq_quantum_us_6331, fpq_quanta_per_frame_6332,
 * fpq_target_fps_6333, fpq_budget_us_6334, fpq_slack_us_6335,
 * fpq_catchup_max_6336, fpq_max_skip_6337, fpq_enabled_6338,
 * fpq_ready_6339, batch_id_6340). Unique gj_fpq_max_skip_6337
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6337_marker[] = "libcgj-batch6337";

/* Maximum frames the pacer may skip when deeply late. */
#define B6337_FPQ_MAX_SKIP  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6337_fpq_max_skip(void)
{
	return B6337_FPQ_MAX_SKIP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpq_max_skip_6337 - report max frames the pacer may skip.
 *
 * Always returns 2. Soft compile-time frame-pacing quanta stub.
 * No parent wires.
 */
uint32_t
gj_fpq_max_skip_6337(void)
{
	(void)NULL;
	return b6337_fpq_max_skip();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fpq_max_skip_6337(void)
    __attribute__((alias("gj_fpq_max_skip_6337")));
