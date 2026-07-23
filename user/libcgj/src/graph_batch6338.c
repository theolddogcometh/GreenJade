/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6338: frame pacing quanta enabled stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fpq_enabled_6338(void);
 *     - Returns 1 (frame-pacing quanta path considered enabled for this
 *       continuum). Soft compile-time product stub; not a live pacer
 *       probe.
 *   uint32_t __gj_fpq_enabled_6338  (alias)
 *   __libcgj_batch6338_marker = "libcgj-batch6338"
 *
 * Exclusive continuum CREATE-ONLY (6331-6340: frame pacing quanta
 * stubs — fpq_quantum_us_6331, fpq_quanta_per_frame_6332,
 * fpq_target_fps_6333, fpq_budget_us_6334, fpq_slack_us_6335,
 * fpq_catchup_max_6336, fpq_max_skip_6337, fpq_enabled_6338,
 * fpq_ready_6339, batch_id_6340). Unique gj_fpq_enabled_6338
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6338_marker[] = "libcgj-batch6338";

/* Frame-pacing quanta enabled stub (1 = path enabled). */
#define B6338_FPQ_ENABLED  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6338_fpq_enabled(void)
{
	return B6338_FPQ_ENABLED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpq_enabled_6338 - report frame-pacing quanta enabled stub.
 *
 * Always returns 1. Soft pure-data product stub; does not arm a live
 * pacer. No parent wires.
 */
uint32_t
gj_fpq_enabled_6338(void)
{
	(void)NULL;
	return b6338_fpq_enabled();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fpq_enabled_6338(void)
    __attribute__((alias("gj_fpq_enabled_6338")));
