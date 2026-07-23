/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6252: input lag microseconds estimate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_lag_us_6252(uint32_t poll_hz);
 *     - Soft one-period lag estimate in microseconds: 1000000 / poll_hz
 *       when poll_hz != 0, else 0. Integer division only; does not
 *       sample hardware timestamps.
 *   uint32_t __gj_input_lag_us_6252  (alias)
 *   __libcgj_batch6252_marker = "libcgj-batch6252"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_lag_us_6252 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6252_marker[] = "libcgj-batch6252";

#define B6252_US_PER_S  1000000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6252_lag_us(uint32_t u32Hz)
{
	if (u32Hz == 0u)
		return 0u;
	return B6252_US_PER_S / u32Hz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_lag_us_6252 - one-period input lag estimate (us).
 *
 * poll_hz: soft poll/report rate in Hertz
 *
 * Returns 1000000/poll_hz for nonzero rates, else 0. Soft pure-data
 * lag estimate. No parent wires.
 */
uint32_t
gj_input_lag_us_6252(uint32_t poll_hz)
{
	(void)NULL;
	return b6252_lag_us(poll_hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_lag_us_6252(uint32_t poll_hz)
    __attribute__((alias("gj_input_lag_us_6252")));
