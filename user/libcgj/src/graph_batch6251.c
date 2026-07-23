/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6251: input poll-rate Hz accept stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_poll_hz_ok_6251(uint32_t hz);
 *     - Soft poll-rate accept lamp. Returns 1 when hz is in the local
 *       soft gaming range [125, 8000], else 0. Pure data only; does not
 *       open HID devices or reprogram USB endpoints.
 *   uint32_t __gj_input_poll_hz_ok_6251  (alias)
 *   __libcgj_batch6251_marker = "libcgj-batch6251"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_poll_hz_ok_6251 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6251_marker[] = "libcgj-batch6251";

/* Soft gaming poll-rate band (Hz). */
#define B6251_HZ_MIN  125u
#define B6251_HZ_MAX  8000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6251_poll_hz_ok(uint32_t u32Hz)
{
	return (u32Hz >= B6251_HZ_MIN && u32Hz <= B6251_HZ_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_poll_hz_ok_6251 - input poll-rate Hz accept stub.
 *
 * hz: soft device report/poll rate in Hertz
 *
 * Returns 1 if hz is in [125, 8000], else 0. Soft pure-data lamp for
 * the 6251-6260 continuum. No parent wires.
 */
uint32_t
gj_input_poll_hz_ok_6251(uint32_t hz)
{
	(void)NULL;
	return b6251_poll_hz_ok(hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_poll_hz_ok_6251(uint32_t hz)
    __attribute__((alias("gj_input_poll_hz_ok_6251")));
