/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6256: input latency budget check stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_latency_budget_ok_6256(uint32_t lag_us, uint32_t budget_us);
 *     - Soft latency-budget lamp. Returns 1 when lag_us <= budget_us
 *       and budget_us != 0; else 0. Pure compare; does not measure
 *       end-to-end path latency.
 *   uint32_t __gj_input_latency_budget_ok_6256  (alias)
 *   __libcgj_batch6256_marker = "libcgj-batch6256"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_latency_budget_ok_6256 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6256_marker[] = "libcgj-batch6256";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6256_budget_ok(uint32_t u32Lag, uint32_t u32Budget)
{
	if (u32Budget == 0u)
		return 0u;
	return (u32Lag <= u32Budget) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_latency_budget_ok_6256 - latency budget check stub.
 *
 * lag_us:    soft measured/estimated lag in microseconds
 * budget_us: soft allowed latency budget in microseconds
 *
 * Returns 1 if lag_us <= budget_us and budget is nonzero, else 0.
 * Soft pure-data budget lamp. No parent wires.
 */
uint32_t
gj_input_latency_budget_ok_6256(uint32_t lag_us, uint32_t budget_us)
{
	(void)NULL;
	return b6256_budget_ok(lag_us, budget_us);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_latency_budget_ok_6256(uint32_t lag_us, uint32_t budget_us)
    __attribute__((alias("gj_input_latency_budget_ok_6256")));
