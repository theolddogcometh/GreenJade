/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6331: frame pacing quantum duration (us).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fpq_quantum_us_6331(void);
 *     - Returns 1000 (1 ms base pacing quantum). Soft compile-time
 *       frame-pacing product stub.
 *   uint32_t __gj_fpq_quantum_us_6331  (alias)
 *   __libcgj_batch6331_marker = "libcgj-batch6331"
 *
 * Exclusive continuum CREATE-ONLY (6331-6340: frame pacing quanta
 * stubs — fpq_quantum_us_6331, fpq_quanta_per_frame_6332,
 * fpq_target_fps_6333, fpq_budget_us_6334, fpq_slack_us_6335,
 * fpq_catchup_max_6336, fpq_max_skip_6337, fpq_enabled_6338,
 * fpq_ready_6339, batch_id_6340). Unique gj_fpq_quantum_us_6331
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6331_marker[] = "libcgj-batch6331";

/* Base frame-pacing quantum (us). */
#define B6331_FPQ_QUANTUM_US  1000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6331_fpq_quantum_us(void)
{
	return B6331_FPQ_QUANTUM_US;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpq_quantum_us_6331 - report base frame-pacing quantum (us).
 *
 * Always returns 1000. Soft compile-time timing stub for frame pacing
 * quanta. No parent wires.
 */
uint32_t
gj_fpq_quantum_us_6331(void)
{
	(void)NULL;
	return b6331_fpq_quantum_us();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fpq_quantum_us_6331(void)
    __attribute__((alias("gj_fpq_quantum_us_6331")));
