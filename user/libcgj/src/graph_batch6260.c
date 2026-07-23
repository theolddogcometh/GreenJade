/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6260: input poll pipeline ready aggregate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_poll_pipeline_ready_6260(uint32_t hz_ok, uint32_t budget_ok,
 *                                             uint32_t queue_ok);
 *     - Soft poll-pipeline-ready aggregate. Returns 1 when hz_ok,
 *       budget_ok, and queue_ok soft lamps are all nonzero; else 0.
 *       Pure data AND of three normalized lamps; does not start
 *       poll threads or HID readers.
 *   uint32_t __gj_input_poll_pipeline_ready_6260  (alias)
 *   __libcgj_batch6260_marker = "libcgj-batch6260"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_poll_pipeline_ready_6260 surface only; no
 * multi-def. Capstone of the 6251-6260 continuum. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6260_marker[] = "libcgj-batch6260";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6260_norm01(uint32_t u32V)
{
	return (u32V != 0u) ? 1u : 0u;
}

static uint32_t
b6260_pipeline_ready(uint32_t u32Hz, uint32_t u32Budget, uint32_t u32Queue)
{
	return b6260_norm01(u32Hz) & b6260_norm01(u32Budget) &
	       b6260_norm01(u32Queue);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_poll_pipeline_ready_6260 - poll pipeline ready aggregate.
 *
 * hz_ok:     soft poll-rate accept lamp
 * budget_ok: soft latency-budget lamp
 * queue_ok:  soft queue-depth configured lamp
 *
 * Returns 1 when all three are nonzero, else 0. Soft pure-data
 * pipeline aggregate (capstone of 6251-6260). No parent wires.
 */
uint32_t
gj_input_poll_pipeline_ready_6260(uint32_t hz_ok, uint32_t budget_ok,
                                  uint32_t queue_ok)
{
	(void)NULL;
	return b6260_pipeline_ready(hz_ok, budget_ok, queue_ok);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_poll_pipeline_ready_6260(uint32_t hz_ok, uint32_t budget_ok,
                                             uint32_t queue_ok)
    __attribute__((alias("gj_input_poll_pipeline_ready_6260")));
