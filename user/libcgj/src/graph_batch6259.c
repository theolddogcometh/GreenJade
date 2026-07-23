/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6259: input lag score composite stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_lag_score_6259(uint32_t lag_us, uint32_t budget_us);
 *     - Soft lag score in [0, 100]. When budget_us is 0, returns 0.
 *       When lag_us >= budget_us, returns 0. Else returns
 *       100 - ((lag_us * 100) / budget_us). Higher is better (more
 *       headroom under budget). Pure integer arithmetic.
 *   uint32_t __gj_input_lag_score_6259  (alias)
 *   __libcgj_batch6259_marker = "libcgj-batch6259"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_lag_score_6259 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6259_marker[] = "libcgj-batch6259";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6259_lag_score(uint32_t u32Lag, uint32_t u32Budget)
{
	uint32_t u32Pct;

	if (u32Budget == 0u)
		return 0u;
	if (u32Lag >= u32Budget)
		return 0u;
	u32Pct = (u32Lag * 100u) / u32Budget;
	return 100u - u32Pct;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_lag_score_6259 - input lag headroom score [0, 100].
 *
 * lag_us:    soft lag estimate in microseconds
 * budget_us: soft latency budget in microseconds
 *
 * Returns headroom score (higher is better), or 0 if over budget /
 * budget is zero. Soft pure-data composite. No parent wires.
 */
uint32_t
gj_input_lag_score_6259(uint32_t lag_us, uint32_t budget_us)
{
	(void)NULL;
	return b6259_lag_score(lag_us, budget_us);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_lag_score_6259(uint32_t lag_us, uint32_t budget_us)
    __attribute__((alias("gj_input_lag_score_6259")));
