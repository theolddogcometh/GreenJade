/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6335: frame pacing slack (us).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fpq_slack_us_6335(void);
 *     - Returns 500 (half-quantum slack margin inside the frame budget).
 *       Soft compile-time frame-pacing product stub.
 *   uint32_t __gj_fpq_slack_us_6335  (alias)
 *   __libcgj_batch6335_marker = "libcgj-batch6335"
 *
 * Exclusive continuum CREATE-ONLY (6331-6340: frame pacing quanta
 * stubs — fpq_quantum_us_6331, fpq_quanta_per_frame_6332,
 * fpq_target_fps_6333, fpq_budget_us_6334, fpq_slack_us_6335,
 * fpq_catchup_max_6336, fpq_max_skip_6337, fpq_enabled_6338,
 * fpq_ready_6339, batch_id_6340). Unique gj_fpq_slack_us_6335
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6335_marker[] = "libcgj-batch6335";

/* Pacing slack margin (us) inside the frame budget. */
#define B6335_FPQ_SLACK_US  500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6335_fpq_slack_us(void)
{
	return B6335_FPQ_SLACK_US;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpq_slack_us_6335 - report frame-pacing slack margin (us).
 *
 * Always returns 500. Soft compile-time frame-pacing quanta stub.
 * No parent wires.
 */
uint32_t
gj_fpq_slack_us_6335(void)
{
	(void)NULL;
	return b6335_fpq_slack_us();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fpq_slack_us_6335(void)
    __attribute__((alias("gj_fpq_slack_us_6335")));
