/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6333: frame pacing target FPS.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fpq_target_fps_6333(void);
 *     - Returns 60 (default paced target frames per second). Soft
 *       compile-time frame-pacing product stub.
 *   uint32_t __gj_fpq_target_fps_6333  (alias)
 *   __libcgj_batch6333_marker = "libcgj-batch6333"
 *
 * Exclusive continuum CREATE-ONLY (6331-6340: frame pacing quanta
 * stubs — fpq_quantum_us_6331, fpq_quanta_per_frame_6332,
 * fpq_target_fps_6333, fpq_budget_us_6334, fpq_slack_us_6335,
 * fpq_catchup_max_6336, fpq_max_skip_6337, fpq_enabled_6338,
 * fpq_ready_6339, batch_id_6340). Unique gj_fpq_target_fps_6333
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6333_marker[] = "libcgj-batch6333";

/* Default frame-pacing target FPS. */
#define B6333_FPQ_TARGET_FPS  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6333_fpq_target_fps(void)
{
	return B6333_FPQ_TARGET_FPS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpq_target_fps_6333 - report frame-pacing target FPS.
 *
 * Always returns 60. Soft compile-time frame-pacing quanta stub.
 * No parent wires.
 */
uint32_t
gj_fpq_target_fps_6333(void)
{
	(void)NULL;
	return b6333_fpq_target_fps();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fpq_target_fps_6333(void)
    __attribute__((alias("gj_fpq_target_fps_6333")));
