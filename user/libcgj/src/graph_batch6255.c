/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6255: input report-rate clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_report_rate_clamp_6255(uint32_t hz);
 *     - Soft report-rate clamp into discrete gaming steps. Maps into
 *       {125, 250, 500, 1000, 2000, 4000, 8000} by ceiling to the next
 *       known step (values above 8000 → 8000; 0 → 0). Pure data.
 *   uint32_t __gj_input_report_rate_clamp_6255  (alias)
 *   __libcgj_batch6255_marker = "libcgj-batch6255"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_report_rate_clamp_6255 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6255_marker[] = "libcgj-batch6255";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6255_rate_clamp(uint32_t u32Hz)
{
	if (u32Hz == 0u)
		return 0u;
	if (u32Hz <= 125u)
		return 125u;
	if (u32Hz <= 250u)
		return 250u;
	if (u32Hz <= 500u)
		return 500u;
	if (u32Hz <= 1000u)
		return 1000u;
	if (u32Hz <= 2000u)
		return 2000u;
	if (u32Hz <= 4000u)
		return 4000u;
	return 8000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_report_rate_clamp_6255 - report-rate discrete clamp.
 *
 * hz: requested soft report rate in Hertz
 *
 * Returns nearest ceiling step in the soft set, or 0 if hz is 0.
 * Soft pure-data clamp. No parent wires.
 */
uint32_t
gj_input_report_rate_clamp_6255(uint32_t hz)
{
	(void)NULL;
	return b6255_rate_clamp(hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_report_rate_clamp_6255(uint32_t hz)
    __attribute__((alias("gj_input_report_rate_clamp_6255")));
